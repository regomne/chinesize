unit CSX_Updating;

interface

uses Windows, CSX_Reading, CSX_JumpIndex;

const
  Option_OutputFuncTableUpdate = False;
  Option_OutputFuncIndexUpdate = False;  
  Option_OutputStringUpdate = False;


(* Updates a DWord *)
function UpdateInteger(Offset: DWord; Delta: Integer; Abs: Boolean = False): DWord;

(* Corrects offsets in a "function" section (in both tables) *)
procedure AdjustFuncOffsets(AbsoluteOffset: DWord; Delta: Integer);

(* Corrects common sizes in a CSX file (excluding "function" offsets) *)
procedure AdjustSizes(Delta: Integer);

(* Relocates a portion of the file *)
procedure Relocate(NewSize, OldSize: DWord);

{ Offset must point to a Unicode sequence, not string length }
{ Str must be Unicode }
(* Writes a string, updating its length and relocating the file *)
procedure WriteString(Offset: DWord; Str: TBytes);

(* Performs a full update of the CSX file *)
procedure UpdateString(const Offset: DWord; const Str: TBytes; const Jumps: TJumpIndex);

implementation

uses ConsoleUtils, Classes, SysUtils,
     CSX_Import;

function UpdateInteger;
var
  Size: DWord;
begin
  Arc.Position := Offset;
  Arc.Read(Size, 4);
  Result := Size;

  if Abs then
    Size := Delta
    else
      Inc(Size, Delta);
  Arc.Seek(-4, soFromCurrent);
  Arc.Write(Size, 4)
end;

procedure AdjustFuncOffsets;
var
  Offset, I, C: DWord;
begin
  if (AbsoluteOffset < ImageSection) or (AbsoluteOffset > FuncSection) then
    Error('Invalid AbsoluteOffset');

  Offset := AbsoluteOffset - ImageSection;

  Arc.Position := FuncTableOffset;
  C := 0;
  for I := 0 to Length(FuncTable) - 1 do
  begin
    if FuncTable[I] > Offset then
    begin
      Inc(FuncTable[I], Delta);
      Inc(C)
    end;
    Arc.Write(FuncTable[I], 4)
  end;

  if Option_OutputFuncTableUpdate then
    WriteLn('Updated ', C, ' records in the table');

  Arc.Position := FuncOffset;
  C := 0;
  for I := 0 to Length(Functions) - 1 do
    with Functions[I] do
    begin
      if Offset_InImage > Offset then
      begin
        Inc(Offset_InImage, Delta);
        Inc(C)
      end;
      Arc.Write(Offset_InImage, 4);
      Arc.Seek(4 + Len * 2, soFromCurrent)
    end;

  if Option_OutputFuncIndexUpdate then
    WriteLn('Updated ', C, ' records in the index');
//  OutSection
end;

procedure AdjustSizes;
begin
  { change CSX contents size (size of all sections) }
  UpdateInteger($38, Delta);
  { change the size of image section }
  UpdateInteger(ImageSection + 8, Delta)
end;

procedure Relocate;
var
  BufSize, Pos: DWord;
  Buf: Pointer;
begin
  Pos := Arc.Position;
  UpdateDelta(NewSize - OldSize);

  Arc.Seek(OldSize, soFromCurrent);
  BufSize := Arc.Size - Arc.Position;
  GetMem(Buf, BufSize);
  Arc.Read(Buf^, BufSize);

  Arc.Position := Pos + NewSize;
  Arc.Write(Buf^, BufSize);
  FreeMem(Buf, BufSize)
end;

procedure WriteString;
var
  PrevLen: DWord;
begin
  if Length(Str) = 0 then
    Exit;
  if Length(Str) mod 2 <> 0 then
    Error('Non-Unicode string?  (length cannot be divided by 2)');

  PrevLen := UpdateInteger(Offset - 4, Length(Str) div 2, True);
  Relocate(Length(Str), PrevLen * 2);

  Arc.Position := Offset;
  Arc.Write(Str[0], Length(Str))
end;

procedure UpdateString;
var
  Delta: Integer;
  Len: DWord;
begin
  if Length(Str) = 0 then
    Exit;

  Arc.Position := Offset - 4;
  Arc.Read(Len, 4);
  if Len > 200 then
  begin
    WriteLn('It''s no good... string length is too big.'#10'     Are you trying to update a different file, not the one these strings were exported from?');
    Exit
  end;

  Delta := Length(Str) - Len * 2;

  if Option_OutputStringUpdate then
    WriteLn(Format('Updating string @%.8x  %d -> %d  (%d)',
                   [Offset, Len, Length(Str) div 2, Delta]));

  if Delta <> 0 then
  begin
    AdjustFuncOffsets(Offset, Delta);
    AdjustSizes(Delta);
    Jumps.ModifyStrAt(Offset, Delta);
    Jumps.ShiftJumpsBefore(Offset, Delta);
  end;
  WriteString(Offset, Str)
end;

end.
