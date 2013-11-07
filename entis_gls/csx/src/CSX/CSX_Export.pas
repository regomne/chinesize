unit CSX_Export;

interface

uses Windows, CSX_Reading;

const
  Option_WriteNewScreen = False;
  Option_WriteFuncName = False;
  Option_WriteSection = False;

var
  // Garden has different format of text out commands or it doesn't have char names in them at all.
  Option_CollectCharacters: Boolean = True;
  // 0 - don't include in the output; 1 - only indent lines; 2 - detailed display marking branch start and finish.
  Option_BranchDisplay: Byte = 0;


type
  TBytesArray = array of TBytes;

  TExtractedString = record
    FuncName: String[4];
    Offset: DWord;
    S: TBytes;
    Extra: TBytes;
    Char: Word;
    CharOffset: DWord;
  end;
  TExtractedStrings = array of TExtractedString;


const
  {  all values are in chars, not bytes  }
  MinimumLength = 0;
  MaximumLength = 500;

  { Divide one big string file into several smaller files.
    This doesn't affect importing, only export use this option }
  LinesPerFile = 2000000;

var
  StringsPath: String;


(* Collects and returns strings from a CSX file (Wanko to Lily format) *)
function Wanko(var Speakers: TBytesArray): TExtractedStrings;

(* Extracts and saves the collected strings from a CSX file *)
procedure ExtractAndSaveStrings;

implementation

uses Classes, ConsoleUtils, SysUtils,
     CSX_Import, CSX_JumpIndex;

type
  TBStackItem = record
    OffsetOfText: DWord;  // offset of exported line which has pushed this record.
    Offset, EndOffset: DWord;
  end;

  TBStack = class
  protected
    FHeight: Word;
    FItems: array[0..$FFFF] of TBStackItem;
  public
    constructor Create;
    procedure Clear;

    procedure Push(const AOffsetOfText, AOffset, AEndOffset: DWord);
    procedure Pop;
    property Height: Word read FHeight;
    function OnTop: TBStackItem;
    function IsEmpty: Boolean;

    function Find(const Offset: DWord): Integer;
  end;

var
  Recollect: record
    Perform: Boolean;
    Count: DWord;
  end;

function Wanko;
type
  TFuncBytes = array[0..11] of Byte;
var
  Func, Speaker: TBytes;
  Ch: Char;
  Count, LastChar: DWord;
  LastCharOffset: DWord;

  function AreBytesEqual(const First, Second: Pointer; const Size: DWord): Boolean;
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

  function IsOutFunc(const Str: TBytes): Boolean;
  const
    Talk: TFuncBytes = (4, 0, 0, 0, Byte('T'), 0, Byte('a'), 0, Byte('l'), 0, Byte('k'), 0);
    Mess: TFuncBytes = (4, 0, 0, 0, Byte('M'), 0, Byte('e'), 0, Byte('s'), 0, Byte('s'), 0);
    FAs:  TFuncBytes = (1, 0, 0, 0, Byte('A'), 0, Byte('S'), 0, 0, 0, 0, 0);
    FCS:  TFuncBytes = (3, 0, 0, 0, Byte('C'), Byte('S'), $1C, Byte('Y'), Byte('P'), Byte('['), 0, 0);
  begin
    Result := AreBytesEqual(@Str[0], @Talk[0], Length(Str)) or
              AreBytesEqual(@Str[0], @Mess[0], Length(Str)) or
              AreBytesEqual(@Str[0], @FAs[0],  8) or
              AreBytesEqual(@Str[0], @FCS[0],  10);
  end;

  function IsSelectFunc(const Str: TBytes): Boolean;
  const
    AddS1: TFuncBytes = (9, 0, 0, 0, Byte('A'), 0, Byte('d'), 0, Byte('d'), 0, Byte('S'), 0);
    AddS2: TFuncBytes = (Byte('e'), 0, Byte('l'), 0, Byte('e'), 0, Byte('c'), 0, Byte('t'), 0, 0, 0);
  var
    Rest: TBytes;
  begin
    Result := AreBytesEqual(@Str[0], @AddS1[0], Length(Str));

    if Result then
    begin
      SetLength(Rest, 12);
      Arc.Read(Rest[0], 12);
      Rest[10] := 0;
      Rest[11] := 0;
      Result := AreBytesEqual(@Rest[0], @AddS2[0], Length(Rest));
      Arc.Seek(-12, soFromCurrent)
    end
  end;

  function Skip(S: TBytes): Boolean;
  begin
    Result := (
               (Length(S) = 2) and
               (
                ((S[0] = $0C) and (S[1] = $30)) or     // opening `
                ((S[0] = $0D) and (S[1] = $30))        // closing `
               )
              ) or (
               (Length(S) = 4) and                     // some garbage or control codes?
               ( (S[0] = $02) and (S[1] = $00) and (S[2] = $04) )
              ) or (
               (Length(S) = 2) and
               ( (S[0] = $02) and (S[1] = $00) )
              )
  end;

  function UnicodeBackwards: Boolean;
  var
    I, W: Word;
  begin
    Result := False;
    I := 0;
    repeat
      Arc.Read(W, 2);
      Arc.Seek(-4, soFromCurrent);
      Inc(I);

      if I >= 30 then
      begin
        Arc.Seek(I * 2 + 31, soFromCurrent);
        Exit
      end
    until (W <> 0) and (W + 2 = I);

    Arc.Seek(2, soFromCurrent);
    Result := True
  end;

  function AddChar: Word;
  var
    I: Word;
  begin
    Result := $FFFF;
    if Length(Speakers) <> 0 then
      for I := 0 to Length(Speakers) - 1 do
        if AreBytesEqual(@Speaker[0], @Speakers[I][0], Length(Speaker)) then
        begin
          Result := I;
          Break
        end;

    if Result = $FFFF then
    begin
      Result := Length(Speakers);
      SetLength(Speakers, Result + 1);
      Speakers[Result] := Speaker
    end
  end;

  procedure AddSpeaker(Back: Word);
  var
    PrevPos, Len: DWord;
  begin
    SetLength(Speaker, 0);
    PrevPos := Arc.Position;
    Arc.Seek(-35 - Back, soFromCurrent);
    if UnicodeBackwards then
    begin
      Speaker := UnicodeRead(Len);
      LastChar := AddChar;
      LastCharOffset := Arc.Position - Len * 2
    end;
    Arc.Position := PrevPos;
  end;

  function GetStrOffset: Byte;
  var
    FuncName: PChar;
  begin
    FuncName := PChar(@Func[4]);
    if (StrLComp(FuncName, 'AS', 2) = 0) or (StrLComp(FuncName, 'CS'#$1C'Y', 4) = 0) then
      Result := 1
      else
        Result := 4;
  end;

  function GetString: TBytes;
  var
    Len: DWord;
  begin
    Arc.Seek(GetStrOffset, soFromCurrent);
    Result := UnicodeRead(Len);

    if Option_CollectCharacters then
      if (Len = 1) and ((Result[0] = $0C) and (Result[1] = $30)) or
                       ((Result[0] = $08) and (Result[1] = $FF)) then
        AddSpeaker(Len * 2);

    if (Len < MinimumLength + 1) or (Len > MaximumLength - 1) or Skip(Result) then
      SetLength(Result, 0)
  end;

  procedure AddResult(DecOffset: Word);
  var
    WideFuncName: WideString;
  begin
    SetLength(Result, Length(Result) + 1);
    with Result[Length(Result) - 1] do
    begin
      SetLength(WideFuncName, High(FuncName));
      Move(Func[4], WideFuncName[1], Length(WideFuncName) * 2);
      FuncName := WideFuncName;
      Offset := Arc.Position - DecOffset;
    end
  end;

  procedure AddString(Str: TBytes);
  begin
    if Length(Str) <> 0 then
    begin
      AddResult(Length(Str));
      Result[Length(Result) - 1].S := Str;
      Inc(Count);

      if Length(Speaker) <> 0 then
        with Result[Length(Result) - 1] do
        begin
          Extra := Speaker;
          Char := LastChar;
          CharOffset := LastCharOffset
        end
    end
  end;

  procedure AddSelect;
  var
    Len: DWord;
  begin
    Arc.Seek(-27, soFromCurrent);

    if UnicodeBackwards then
    begin
      AddString(UnicodeRead(Len));

      SetLength(Result, Length(Result) + 1);
      Result[Length(Result) - 1].FuncName := 'Hitr';

      Arc.Seek(35, soFromCurrent)
    end
  end;

  procedure TryHitRet;
  const
    // "HitretNewPage"
    HitRet1: TFuncBytes = ($0D, 0, 0, 0, Byte('H'), 0, Byte('i'), 0, Byte('t'), 0, Byte('r'), 0);
    HitRet2: TFuncBytes = ($06, 0, 0, 0, Byte('H'), 0, Byte('i'), 0, Byte('t'), 0, Byte('r'), 0);
    HitRet3: TFuncBytes = ($08, 0, 0, 0, 0, 0, 0, $80, $FD, 0, 0, 0);
  begin
    Arc.Seek(-5, soFromCurrent);
    if Ch = #$FD then
      Arc.Seek(-4, soFromCurrent);

    if Arc.Read(Func[0], Length(HitRet1)) < Length(HitRet1) then
      Exit;

    if AreBytesEqual(@Func[0], @HitRet1[0], Length(Func)) or
       AreBytesEqual(@Func[0], @HitRet2[0], Length(Func)) or
       AreBytesEqual(@Func[0], @HitRet3[0], Length(Func)) then
    begin
      if Ch = #$FD then
      begin
        Move(WideString('Hitr')[1], Func[4], 8);
        AddResult(12);
      end
        else
          AddResult(8);
      SetLength(Speaker, 0)
    end;

    if Length(Result) >= 5000 then
      Recollect.Perform := True
  end;

begin
  Count := 0;
  SetLength(Func, 12);

  SetLength(Result, 0);
//  arc.Position := $68EEB0;

  while Arc.Position < ImageSectionEnd do
//  while Arc.Position < 02303885 do
  begin
    UpdateProgress(Arc.Position, Arc.Size, Format('(%d pieces so far)', [Recollect.Count + Count]));

    Arc.Read(Ch, 1);
    if Ch in ['T', 'M', 'A', 'C'] then
    begin
      Arc.Seek(-5, soFromCurrent);
      if Arc.Read(Func[0], 12) < 12 then
        Break;

      if IsOutFunc(Func) then
        AddString(GetString)
        else if IsSelectFunc(Func) then
          AddSelect
          else
            Arc.Seek(-7, soFromCurrent)
    end
      else if Ch in ['H', #$FD] then
        TryHitRet;

    if Recollect.Perform then
      Exit
  end;

  //if Count <> 0 then
//    WriteLn(Count, ' strings have been collected')
end;

procedure ExtractAndSaveStrings;
var
  S: TExtractedStrings;
  I: DWord;
  Save: TextFile;
  LineStart: DWord;
  Count, LastFileName: DWord;
  LastSection: String;
  Chars: TBytesArray;
  Jumps: TJumpIndex;
  BStack: TBStack;

  function BranchIndent(const Offset: DWord): String;
    function Indent: String;
    begin
      Result := StringOfChar(' ', BStack.Height * 2);
    end;

  var
    Index: Integer;
    EndOffset: DWord;
  begin
    Result := '';

    if Option_BranchDisplay <> 0 then
    begin
      while not BStack.IsEmpty and (BStack.OnTop.EndOffset < Offset) do
      begin
          EndOffset := BStack.OnTop.OffsetOfText;
        BStack.Pop;
          Result := Result + Indent + '^ branch ' + IntToHex(EndOffset, 1) + ' end'#13#10;
      end;

      Index := 0;
      repeat
        Index := Jumps.FindWrappingJumpOf(Offset, Index);
        if Index <> -1 then
        begin
          if BStack.Find( Jumps[Index].Offset ) = -1 then
          begin
            EndOffset := Jumps[Index].Offset + Jumps[Index].Distance;
            Result := Result + Indent + 'v branch till ~' + IntToHex(EndOffset, 1) + #13#10;
            BStack.Push(Offset, Jumps[Index].Offset, EndOffset);
          end;

          Inc(Index);
        end;
      until Index = -1;

      if Option_BranchDisplay = 1 then
        Result := Indent
        else
          Result := Result + Indent;
    end;
  end;

  procedure ANewLine;
  var
    LI: DWord;
    Line, Section: String;
  begin
    if I <> LineStart then
    begin
      Line := '';
      if Option_WriteNewScreen then
        Line := ' - new screen - '#10#13;

      if Option_WriteSection then
      begin
        LI := FindWhichFunc(S[LineStart].Offset);
        if LI = DWord(-1) then
        begin
          WriteLn(#10'Warning: there seems to be a bug, please report to me (with this CSX attached).'#10);
          Exit;
        end;

        Section := Functions[LI].Name;
        if LastSection <> Section then
        begin
          Line := ' ~ ' + Section + ' ~ '#10;
          LastSection := Section
        end
      end;

      Line := Line + BranchIndent( S[LineStart].Offset );

      Line := Line + '<';
      for LI := LineStart to I - 1 do
        Line := Line + IntToHex(S[LI].Offset, 1) + ',';
      if Length(S[LineStart].Extra) <> 0 then
        Line := Line + '*' + IntToStr(S[LineStart].Char) + ':' + IntToHex(S[LineStart].CharOffset, 1)
        else
          Line := Copy(Line, 1, Length(Line) - 1);
      Line := Line + '>';

      if Length(S[LineStart].Extra) <> 0 then
        Line := Line + ' ' + UTF8Encode( #$300C + WideString(S[LineStart].Extra) + #$300D );

      if Option_WriteFuncName then
        Line := Line + ' |' + S[LineStart].FuncName + '|';

      for LI := LineStart to I - 1 do
        Line := Line + UTF8Encode( WideString(S[LI].S) );

      WriteLn(Save, Line + '\=');
      Inc(Count)
    end;

    LineStart := I + 1
  end;

  procedure NewFile;
  begin
    CloseFile(Save);

    Inc(LastFileName);
    AssignFile(Save, ChangeFileExt(StringsPath, IntToStr(LastFileName) + '.utf'));

    Rewrite(Save)
  end;

  function AreBytesEqual(const B1: TBytes; const B2: TBytes): Boolean;
  var
    Ch: Byte;
  begin
    Ch := 0;
    while Ch < Length(B2) do
    begin
      Result := B1[Ch] = B2[Ch];
      if not Result then
        Break;
      Inc(Ch)
    end
  end;

  procedure WriteSpeakers;
  var
    I: Word;
  begin
    if Length(Chars) <> 0 then
    begin
      for I := 0 to Length(Chars) - 1 do
        WriteLn(Save, '- Character: ', UTF8Encode( WideString(Chars[I]) ), '\=');
      WriteLn(Save)
    end
  end;

begin
  Jumps := CreateJumpIndex(True);
  WriteLn;
  BStack := TBStack.Create;

  OpenCSX(Path);
  Arc.Position := ImageSection;
//  arc.Position := $2D9A06;

  SetLength(Chars, 0);

  Recollect.Perform := True;
  Recollect.Count := 0;

  AssignFile(Save, StringsPath);
  Rewrite(Save);

  WriteLn('Collecting strings...');
  ResetProgress;

  Count := 0;
  LastFileName := 0;

  while Recollect.Perform do
  begin
    Recollect.Perform := False;

    S := Wanko(Chars);
//    if Length(S) = 0 then
//      Error('No strings in this file');

    LineStart := 0;

    Inc(Recollect.Count, Length(S));

    for I := 0 to Length(S) - 1 do
    begin
      if S[I].FuncName = 'Hitr' then
        ANewLine;
      if (Count <> LinesPerFile * LastFileName) and (Count mod LinesPerFile = 0) then
        NewFile
    end;
    ANewLine
  end;

  WriteLn(LastFileName + 1, ' files with ', Count, ' lines total, ', Length(Chars), ' characters');

  if Length(Chars) <> 0 then
  begin
    WriteLn;
    WriteLn('Writing characters list to the end of ',
            ExtractFileName(ChangeFileExt(StringsPath, IntToStr(LastFileName) + '.utf')), '...');

    WriteLn(Save);
    WriteLn(Save);
    WriteSpeakers
  end;

  CloseFile(Save);
  BStack.Free;
  Jumps.Free;

  if not FileExists(OriPath) then
  begin
    FreeAndNIL(Arc);
    CopyFile(PChar(Path), PChar(OriPath), False)
  end
end;

{ TBStack }

constructor TBStack.Create;
begin
  Clear;
end;

procedure TBStack.Clear;
begin
  FHeight := 0;
end;

function TBStack.OnTop: TBStackItem;
begin
  if IsEmpty then
    Result.Offset := 0
    else
      Result := FItems[FHeight - 1];
end;

procedure TBStack.Pop;
begin
  if not IsEmpty then
    Dec(FHeight);
end;

procedure TBStack.Push(const AOffsetOfText, AOffset, AEndOffset: DWord);
begin
  with FItems[FHeight] do
  begin
    OffsetOfText := AOffsetOfText;
    Offset := AOffset;
    EndOffset := AEndOffset;
  end;

  Inc(FHeight);
  if FHeight >= $FFFF then
    raise Exception.Create('TBStack: too high!');
end;

function TBStack.IsEmpty: Boolean;
begin
  Result := FHeight = 0;
end;

function TBStack.Find(const Offset: DWord): Integer;
begin
  if not IsEmpty then
    for Result := FHeight - 1 downto 0 do
      if FItems[Result].Offset = Offset then
        Exit;

  Result := -1;
end;

end.