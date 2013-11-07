unit CSX_JumpIndex;

interface

uses Windows, SysUtils, ConsoleUtils, Classes, CSX_Reading, CSX_Export;

type
  TJump = record
    FuncName: TBytes;
    Offset: DWord;
    OffsetDelta: Integer;          // todo: remove, it's not used
    DistanceParamOffset: Integer;  // FuncName ... [dw] Distance  -  "..." is this field, in bytes.
    Distance: DWord;               // current jump distance, abs. offset in a .csx = Offset + Distance

    Jump1PrefixSize, Jump2PrefixSize: Byte;  // size of JumpXPrefix in bytes:
                                             //   0 - don't check prefix (always accept next dword as jump address),
                                             // 1-4 - byte, word or dword prefix,
                                             //  5+ - no X branch in JumpXPrefix (e.g. J2PfSize = 5 means the command,
                                             //       don't have second address part).
    Jump1Prefix, Jump2Prefix: DWord;  // only alter address if it's prefixed with this combination. Sample:
                                      //   FuncName .. +distance bytes .. [ [Jump1Prefix DW_Jump [J2Pf DW_Jump]] ]

    Modified: Boolean;
  end;

  TJumpIndex = class
  protected
    JumpCount: DWord;
    Jumps: array[0..$2FFFD] of TJump;

    function GetJump(Index: DWord): TJump;

    function IsFirstCharofAJump(const Ch: DWord): Boolean;
    function TryAddingJumpAt(const CSX: TStream): Boolean;
    function AreBytesEqual(const First, Second: Pointer; const Size: DWord): Boolean;
    procedure TryAddingReferencedJumperAt(const Offset: DWord; const CSX: TStream);
    procedure AddJumpsAt(CSX: TStream; const JumpTemplate: TJump); // considers jump prefixes and calls AddJump.
    procedure AddJump(const FuncName_: TBytes; const Offset_: DWord;
      const DistanceParamOffset_: Integer; const Distance_: DWord);
    function FindNearestJumpAt(const Offset: DWord): Integer;
  public
    constructor Create;

    procedure Clear;
    procedure CollectFrom(CSX: TStream; UpToOffset: DWord);
    procedure ModifyStrAt(const Offset: DWord; const Delta: Integer);
    procedure ShiftJumpsBefore(Offset: DWord; const Delta: Integer);
    function ApplyModificationsTo(const CSX: TStream): DWord;

    function FindWrappingJumpOf(const Offset, StartFrom: DWord): Integer;
    property Jump[Index: DWord]: TJump read GetJump; default;
    function DumpJumps: String;  // debug method

    procedure SaveTo(FileName: String); overload;
    procedure SaveTo(const Stream: TStream); overload;
    procedure LoadFrom(FileName: String); overload;
    procedure LoadFrom(const Stream: TStream); overload;
  end;

function CreateJumpIndex(ReturnIndex: Boolean = False): TJumpIndex;
procedure OutputJumpIndex;

implementation

const
  SavedSignature: array[0..3] of Char = 'CJIx';
  RegenCLI = 'You can regenerate it using this command:'#10#9'csx.exe <pack.csx> makejumpindex';
  JumpRecordSize = 16;  // Excluding FuncName and Modified.
  NoJumpAddress = $FF;  // see JumpXPrefixSize for "5+" case.
  MinDistance = 7;  // inclusive - this or more.
  MaxDistance = $FFFFF;

var
  CotophaJumps: array[0..4] of TJump;
  RefJumperFuncName: TBytes;

{ Interface to the main program }

function CreateJumpIndex(ReturnIndex: Boolean = False): TJumpIndex;
begin
  OpenCSX(Path);

  WriteLn('Building jump index table:');

  Result := TJumpIndex.Create;
  try
    try
      Arc.Position := ImageSection;
      Result.CollectFrom(Arc, ImageSectionEnd);

      WriteLn('Saving it:');
        Result.SaveTo( ChangeFileExt(StringsPath, '.cji') );

      WriteLn('Jump index table has ', Result.JumpCount, ' records.');
      FreeAndNIL(Arc);
    except
      ReturnIndex := False;
      raise;
    end;
  finally
    if not ReturnIndex then
      FreeAndNIL(Result);
  end;
end;

procedure OutputJumpIndex;
var
  Index: TJumpIndex;
begin
  Index := TJumpIndex.Create;
  try
    WriteLn('Loading jump index table...');
      Index.LoadFrom( ChangeFileExt(StringsPath, '.cji') );

    WriteLn;
    WriteLn(Index.DumpJumps);
  finally
    Index.Free;
  end;
end;

{ TJumpIndex }

constructor TJumpIndex.Create;
begin
  Clear;
end;

procedure TJumpIndex.Clear;
begin
  JumpCount := 0;
end;

procedure TJumpIndex.CollectFrom(CSX: TStream; UpToOffset: DWord);
var
  Ch: Word;
  StartedFrom, OldPos: DWord;
begin
  ResetProgress;
  StartedFrom := CSX.Position;

  while CSX.Position < UpToOffset do
  begin
    CSX.Read(Ch, 2);

    OldPos := CSX.Position - 1;

    if IsFirstCharofAJump(Ch) then
    begin
      CSX.Seek(-6, soFromCurrent);
      if not TryAddingJumpAt(CSX) then
        CSX.Position := OldPos;
    end
      else
        CSX.Position := OldPos;;

    UpdateProgress(CSX.Position - StartedFrom + 1, UpToOffset - StartedFrom);
  end;
end;

function TJumpIndex.IsFirstCharofAJump(const Ch: DWord): Boolean;
var
  I: Word;
begin
  if Ch and $FF = Ch then   // to speed up matching: known jump functions names are all English.
    for I := 0 to Length(CotophaJumps) - 1 do
      if CotophaJumps[I].FuncName[0] = Ch then
      begin
        Result := True;
        Exit;
      end;

  Result := False;
end;

function TJumpIndex.TryAddingJumpAt(const CSX: TStream): Boolean;
var
  FuncNameLength: DWord;
  I: Word;
  FuncName: TBytes;
begin
  CSX.Read(FuncNameLength, 4);
  if FuncNameLength < $FF then
  begin
    SetLength(FuncName, FuncNameLength * 2);
    CSX.Read(FuncName[0], FuncNameLength * 2);

    for I := 0 to Length(CotophaJumps) - 1 do
      if (Length(CotophaJumps[I].FuncName) = Integer(FuncNameLength * 2)) and
         AreBytesEqual(@CotophaJumps[I].FuncName[0], @FuncName[0], FuncNameLength * 2) then
        begin
          AddJumpsAt(CSX, CotophaJumps[I]);
          Result := True;
          Exit;
        end;
  end;

  Result := False;
end;

procedure TJumpIndex.AddJumpsAt(CSX: TStream; const JumpTemplate: TJump);
  function CheckPrefix(Prefix: DWord; Size: Byte): Boolean;
  var
    PfRead: DWord;
  begin
    case Size of
    0:      Result := True;
    1..4:   Result := (CSX.Read(PfRead, Size) = Size) and CompareMem(@PfRead, @Prefix, Size);
    else    Result := False;
    end;
  end;

  function TryAdding(JumpPrefix: DWord; JumpPfSize: Byte): Boolean;
  var
    Distance: DWord;
  begin
    Result := False;

    if CheckPrefix(JumpPrefix, JumpPfSize) then
    begin
      CSX.Read(Distance, 4);
      if Distance < MaxDistance then
      begin
        TryAddingReferencedJumperAt(CSX.Position + Distance, CSX);
        AddJump(JumpTemplate.FuncName, CSX.Position - 4, 0, Distance);

        Result := True;
      end;
    end;
  end;

begin
  with JumpTemplate do
  begin
    CSX.Seek(DistanceParamOffset, soFromCurrent);
    if TryAdding(Jump1Prefix, Jump1PrefixSize) then
      TryAdding(Jump2Prefix, Jump2PrefixSize);
  end;
end;

procedure TJumpIndex.TryAddingReferencedJumperAt(const Offset: DWord; const CSX: TStream);
var
  OldPos, Distance: DWord;
  B: Byte;
begin
  OldPos := CSX.Position;
  try
    CSX.Position := Offset;
    CSX.Read(B, 1);
    if B = $06 then
    begin
      CSX.Read(Distance, 4);
      if Distance < MaxDistance then
      begin
        TryAddingReferencedJumperAt(CSX.Position + Distance, CSX);
        AddJump(RefJumperFuncName, CSX.Position - 4, 0, Distance);
      end;
    end;
  finally
    CSX.Position := OldPos;
  end;
end;

procedure TJumpIndex.AddJump(const FuncName_: TBytes; const Offset_: DWord;
  const DistanceParamOffset_: Integer; const Distance_: DWord);
var
  Index: Integer;

  procedure InsertBlankJump;
  var
    I: Integer;
    Temp: TJump;
  begin
    for I := JumpCount - 1 downto Index + 1 do
    begin
      Temp := Jumps[I];
      Jumps[I] := Jumps[I - 1];
      Jumps[I - 1] := Temp;
    end;
  end;

begin
  if Distance_ >= MinDistance then
  begin
    Index := FindNearestJumpAt(Offset_);
    if (Index <> -1) and (Jumps[Index].Offset = Offset_) then
      Exit;

    Inc(JumpCount);
    if Integer(JumpCount) >= Length(Jumps) then
      Error('Too many jumps in this script!');

    if Index = -1 then
      Index := JumpCount - 1
      else
        InsertBlankJump;

    with Jumps[Index] do
    begin
      SetLength(FuncName, Length(FuncName_));
      Move(FuncName_[0], FuncName[0], Length(FuncName_));

      Offset := Offset_;
      OffsetDelta := 0;
      DistanceParamOffset := DistanceParamOffset_;
      Distance := Distance_;
      Modified := False;
    end;
  end;
end;

function TJumpIndex.AreBytesEqual(const First, Second: Pointer; const Size: DWord): Boolean;
var
  I: DWord;
begin
  Result := True;
  I := 0;
  while (I < Size) and Result do
  begin
    Result := PByte( DWord(First) + I )^ = PByte( DWord(Second) + I )^;
    Inc(I)
  end
end;

procedure TJumpIndex.SaveTo(FileName: String);
var
  F: TFileStream;
begin
  F := TFileStream.Create(FileName, fmCreate);
  try
    SaveTo(F);
  finally
    F.Free;
  end;
end;

procedure TJumpIndex.SaveTo(const Stream: TStream);
  procedure UnicodeWrite(const Str: TBytes; Stream: TStream);
  var
    Len: DWord;
  begin
    Len := Length(Str) div 2;
    Stream.Write(Len, 4);
    Stream.Write(Str[0], Len * 2);
  end;

var
  I: DWord;
begin
  Stream.Write(SavedSignature[0], 4);
  Stream.Write(JumpCount, 4);

  if JumpCount <> 0 then
  begin
    ResetProgress;

    for I := 0 to JumpCount - 1 do
    begin
      UnicodeWrite(Jumps[I].FuncName, Stream);
      Stream.Write( Jumps[I].Offset, JumpRecordSize );
      UpdateProgress(I + 1, JumpCount);
    end;
  end;
end;

procedure TJumpIndex.LoadFrom(FileName: String);
var
  F: TFileStream;
begin
  if not FileExists(FileName) then
    Error('The jump index table file cannot be found. ' + RegenCLI);

  F := TFileStream.Create(FileName, fmOpenRead or fmShareDenyWrite);
  try
    LoadFrom(F);
  finally
    F.Free;
  end;
end;

procedure TJumpIndex.LoadFrom(const Stream: TStream);
var
  Sign: array[0..3] of Char;
  Count, Len: DWord;
begin
  CLear;

  Stream.Read(Sign[0], 4);
  if Sign <> SavedSignature then
    Error('The jump index file is corrupted. ' + RegenCLI);

  Stream.Read(Count, 4);
  JumpCount := Count;

  if Count <> 0 then
  begin
    ResetProgress;

    for Count := 0 to JumpCount - 1 do
    begin
      Jumps[Count].FuncName := UnicodeRead(Len, Stream);
      Stream.Read( Jumps[Count].Offset, JumpRecordSize );
      Jumps[Count].Modified := False;

      UpdateProgress(Count + 1, JumpCount);
    end;
  end;
end;

procedure TJumpIndex.ModifyStrAt(const Offset: DWord; const Delta: Integer);
var
  Index: Integer;
begin
  Index := 0;
  repeat
    Index := FindWrappingJumpOf(Offset, Index);
    if Index <> -1 then
    begin
      Inc(Jumps[Index].Distance, Delta);
      Jumps[Index].Modified := True;
      Inc(Index);
    end;
  until Index = -1;
end;

function TJumpIndex.FindWrappingJumpOf(const Offset, StartFrom: DWord): Integer;
begin
  if JumpCount > StartFrom then
    for Result := StartFrom to JumpCount - 1 do
      if (Jumps[Result].Offset <= Offset) and
         (Jumps[Result].Offset + Jumps[Result].Distance >= Offset) then
        Exit
        else if Jumps[Result].Offset > Offset then
          // since the index is sorted (ascending) we're sure that
          // the following offsets won't match.
          Break;

  Result := -1;
end;

function TJumpIndex.FindNearestJumpAt(const Offset: DWord): Integer;
begin
  if JumpCount <> 0 then
    for Result := 0 to JumpCount - 1 do
      if Jumps[Result].Offset >= Offset then
        Exit;

  Result := -1;
end;

procedure TJumpIndex.ShiftJumpsBefore(Offset: DWord; const Delta: Integer);
var
  I: DWord;
begin
  if JumpCount <> 0 then
    for I := 0 to JumpCount - 1 do
      if Jumps[I].Offset >= Offset then
      begin
        for Offset := I to JumpCount - 1 do
          Inc(Jumps[Offset].Offset, Delta);
        Break;
      end;
end;

function TJumpIndex.ApplyModificationsTo(const CSX: TStream): DWord;
var
  I: DWord;
begin
  Result := 0;

  if JumpCount <> 0 then
  begin
    ResetProgress;

    for I := 0 to JumpCount - 1 do
    begin
      with Jumps[I] do
        if Modified then
        begin
          CSX.Position := Integer(Offset) + OffsetDelta + DistanceParamOffset;
          CSX.Write(Distance, 4);
          Inc(Result);
        end;

      UpdateProgress(I + 1, JumpCount);
    end;
  end;
end;

function TJumpIndex.DumpJumps: String;
  function Bytes2WideString(const Bytes: TBytes): WideString;
  begin
    SetLength(Result, Length(Bytes) div 2);
    Move(Bytes[0], Result[1], Length(Bytes));
  end;

const
  EOLN = #13#10;
var
  I: DWord;
  Diff: Integer;
begin
  Result := 'Jump index table dump' + EOLN +
            '=====================' + EOLN;

  if JumpCount = 0 then
    Result := Result + 'Empty index.'
    else
    begin
      for I := 0 to JumpCount - 1 do
        with Jumps[I] do
        begin
          if I = 0 then
            Diff := 0
            else
              Diff := Offset - Jumps[I - 1].Offset;

          Result := Result + Format( '  %3d. %-15s /%d = %.8x .. %.5x %29s',
                                     [I + 1, Bytes2WideString(FuncName), DistanceParamOffset,
                                      Offset, Distance, '+ ' + IntToStr(Diff)] );

          if Modified then
            Result := Result + ' [M*]';

          Result := Result + EOLN;
        end;

      Result := Result + '_____________' + EOLN +
                         Format('End of index, %d total records.', [JumpCount]);
    end;
end;

function TJumpIndex.GetJump(Index: DWord): TJump;
begin
  Result := Jumps[Index];
end;

{ Initialization }

procedure WideString2Bytes(const WS: WideString; out Bytes: TBytes);
begin
  SetLength(Bytes, Length(WS) * 2);
  Move(WS[1], Bytes[0], Length(WS) * 2);
end;

initialization
  with CotophaJumps[0] do
  begin
    WideString2Bytes('IsGameClear', FuncName);
    DistanceParamOffset := 2;              
    Jump2PrefixSize := NoJumpAddress;
  end;
  with CotophaJumps[1] do
  begin
    WideString2Bytes('ChkFlagOn', FuncName);
    DistanceParamOffset := 2;            
    Jump2PrefixSize := NoJumpAddress;
  end;
  with CotophaJumps[2] do
  begin
    WideString2Bytes('ChkSelect', FuncName);
    DistanceParamOffset := 2;          
    Jump2PrefixSize := NoJumpAddress;
  end;
  with CotophaJumps[3] do
  begin
    WideString2Bytes('OnFlag', FuncName);
    DistanceParamOffset := 2;
    Jump2PrefixSize := NoJumpAddress;
  end;
  with CotophaJumps[4] do
  begin
    { "IsRecollectModeMode" > [ 0700 > dw:jmp > [0506 > dw:jmp] ] }
    WideString2Bytes('IsRecollectMode', FuncName);
    DistanceParamOffset := 0;
    Jump1PrefixSize := 2;
    Jump1Prefix := $0007;
    Jump2PrefixSize := 2;
    Jump2Prefix := $0605;
  end;

  WideString2Bytes('##RJ', RefJumperFuncName);
end.
