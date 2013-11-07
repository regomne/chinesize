unit CSX_Reading;

interface            

uses Windows, Classes;

type
  TBytes = array of Byte;

type
  TSection = packed record
    SectionName: array[0..7] of Char;
    SectionSize: DWord;
    Unk_UsuallyZero: DWord;
  end;

type
  { Function Section }
  TFuncHeader = packed record
    Unk_UsuallyZero: DWord;
    EntriesCount: DWord;
  end;
  TFunc = packed record
    Offset_InImage: DWord;
    Len: DWord;
    Name: WideString;
  end;


var
  Arc: TStream;
  Path: String;

var
  { image, function, global, data, linkinf }
  ImageSection: DWord = 0;
  ImageSectionEnd: DWord = 0;
  FuncSection: DWord = 0;
  FuncTableOffset, FuncOffset: DWord;
  FuncTable: array of DWord;
  Functions: array of TFunc;
  SortedFunctions: array of TFunc; // the same as Functions but sorted by Offset_InImage


{ Inner function that opens a file that is supposed to be a CSX pack }
procedure OpenCSX(CSXPath: String);

{ Updates an inner global offset delta }
procedure UpdateDelta(Delta: Integer);


(* Reads Unicode string *)
function UnicodeRead(out Length: DWord; Stream: TStream = NIL): TBytes;

(* Seads Section into TSection *)
function ReadSection(Name: String = ''; ID: PString = NIL): TSection;

(* Collects and fills section addresses in the CSX pack *)
procedure ReadSectionAddresses;

(* Fills variables related to "function" section *)
procedure ReadFunctions;

(* Outputs a current section, i.e. tries to read a section from the current
   Arc stream position. Restores stream's pointer. Diagnostic function. *)
procedure OutSection;

(* Returns a function index to which a gived address belongs. Offset is absolute *)
function FindWhichFunc(Offset: DWord): DWord;

implementation

uses ConsoleUtils, SysUtils;

procedure Reindex;
begin
  ReadSectionAddresses;
  ReadFunctions
end;
        
  procedure LoadArcStreamFrom(const Path: WideString; IntoStream: TStream);
  var
    ArcFile: TFileStream;
  begin
    ArcFile := TFileStream.Create(Path, fmOpenRead or fmShareDenyWrite);
    try
      IntoStream.Size := ArcFile.Size;
      IntoStream.CopyFrom(ArcFile, 0);
    finally
      ArcFile.Free;
    end;
  end;

  procedure SaveArcStreamTo(const Path: WideString; FromStream: TStream);
  var
    ArcFile: TFileStream;
  begin
    ArcFile := TFileStream.Create(Path, fmCreate);
    try
      ArcFile.CopyFrom(FromStream, 0);
    finally
      ArcFile.Free;
    end;
  end;

procedure OpenCSX;
begin
  Path := CSXPath;
  if not FileExists(Path) then
    Error('Cannot open CSX file!');

  Arc := TMemoryStream.Create;
  LoadArcStreamFrom(Path, Arc);

  Reindex
end;

procedure UpdateDelta;
const
  Update: array[0..3] of Pointer = (@ImageSectionEnd,
    @FuncSection, @FuncTableOffset, @FuncOffset);
var
  I: Byte;
begin
  for I := 0 to Length(Update) - 1 do
    Inc(DWord(Update[I]^), Delta)
end;


function UnicodeRead;
begin
  if Stream = NIL then
    Stream := Arc;

  Stream.Read(Length, 4);
  SetLength(Result, Length * 2);
  Stream.Read(Result[0], Length * 2)
end;

function ReadSection;
var
  SID: String;
begin
  FillMemory(@Result, SizeOf(Result), 0);
  Arc.Read(Result, SizeOf(TSection));

  SID := Trim(String(Result.SectionName));
  if ID <> NIL then
    ID^ := SID;

  if (Name <> '') and (SID <> Name) then
    Error(Format('Function section name at @%.4x is not ''%s'': %s',
                 [Arc.Position, Name, Result.SectionName]))
end;

procedure ReadSectionAddresses;
var
  Section: TSection;
  Name: String;
begin
  Arc.Position := $40;

  while Arc.Position < Arc.Size do
  begin
    Section := ReadSection('', @Name);
    if Name = 'image' then
    begin
      ImageSection := Arc.Position - SizeOf(TSection);
      ImageSectionEnd := Arc.Position + Section.SectionSize
    end
      else if Name = 'function' then
        FuncSection := Arc.Position - SizeOf(TSection);

    if (ImageSection <> 0) and (FuncSection <> 0) then
      Break;

    Arc.Seek(Section.SectionSize, soFromCurrent)
  end
end;

procedure ReadFunctions;
var
  Section: TSection;
  FuncTableLength: DWord;
  FuncHeader: TFuncHeader;
  I: DWord;
begin
  WriteLn('Reading functions section...');
  Arc.Position := FuncSection;

  Section := ReadSection('function');

  Arc.Read(FuncTableLength, 4);
  
  FuncTableOffset := Arc.Position;
  SetLength(FuncTable, FuncTableLength);
  Arc.Read(FuncTable[0], FuncTableLength * 4);

  Arc.Read(FuncHeader, SizeOf(TFuncHeader));
  FuncOffset := Arc.Position;

  SetLength(Functions, FuncHeader.EntriesCount);
  for I := 0 to FuncHeader.EntriesCount - 1 do
    with Functions[I] do
    begin
      Arc.Read(Offset_InImage, 4);
      Name := WideString(UnicodeRead(Len))
    end;

//  OutSection
end;

procedure OutSection;
var
  Section: TSection;
begin
  Arc.Read(Section, SizeOf(TSection));
  Arc.Seek(-SizeOf(TSection), soFromCurrent);
  WriteLn('Current section is ''' + Section.SectionName + '''' +
          ' (@' + IntToHex(Arc.Position, 8) + ')')
end;

function FindWhichFunc;
var
  I, MinOffset: DWord;
begin
  Result := DWord(-1);
  MinOffset := DWord(-1);
  Dec(Offset, ImageSection);

  for I := 0 to Length(FuncTable) - 1 do
    if (FuncTable[I] > Offset) and (MinOffset > FuncTable[I] - Offset) then
    begin
      MinOffset := FuncTable[I] - Offset;
      Result := I;
    end;
end;


initialization

finalization
  try
    if Arc <> NIL then
    begin
      SaveArcStreamTo(Path, Arc);
      Arc.Free
    end
  except
  end;

end.
