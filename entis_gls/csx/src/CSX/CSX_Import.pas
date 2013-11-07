unit CSX_Import;

interface

const
  // updates speaker's name of untranslated entries too. If true, can be quite slow 
  Option_UpdateUntranslated = true;

var
  // original (untranslated) CSX pack
  OriPath: String;


(* Imports (updates) strings into a CSX file *)
procedure ImportStrings;

implementation

uses Windows, SysUtils, StrUtils,
     ConsoleUtils, CSX_JumpIndex, CSX_Reading, CSX_Updating, CSX_Export;

function CopyBytes(const Src: TBytes): TBytes;
begin
  SetLength(Result, Length(Src));
  CopyMemory(@Result[0], @Src[0], Length(Src))
end;

procedure ImportStrings;
var
  Strings: TextFile;
  Lines: array[0..100000] of String;
  Count, ScriptCount: Word;
  Chars: array[0..1000] of TBytes;
  Percent: Single;
  Jumps: TJumpIndex;

  procedure Individualize(const Offsets: TArray);
  var
    I: Word;
    Space: TBytes;
  begin
    if Length(Offsets) > 1 then
    begin
      SetLength(Space, 2);
      Space[0] := $20;
      for I := Length(Offsets) - 1 downto 1 do
        UpdateString( StrToInt('$' + Offsets[I]), Space, Jumps );
    end
  end;

  procedure ParseLine(Line: String);
  var
    P1, T1: Word;
    Offsets: TArray;     
    Str: TBytes;
    Char: Word;
    CharOffset: DWord;

    procedure GetCharOffset(var Offsets: TArray);
    var
      Str: String;
      P: Byte;
    begin
      Char := $FFFF;

      Str := Offsets[Length(Offsets) - 1];
      if Str[1] = '*' then
      begin
        P := Pos(':', Str);
        if P <> 0 then
        begin
          Char := StrToInt( Copy(Str, 2, P - 2) );
          CharOffset := StrToInt( '$' + Copy(Str, P + 1, $FFFF) );

          SetLength(Offsets, Length(Offsets) - 1)
        end
      end
    end;

    procedure UpdateChar;
    begin
      if (Char <> $FFFF) and (Length(Chars[Char]) <> 0) then
        UpdateString(CharOffset, Chars[Char], Jumps)
    end;

  begin
    if (Line = '') or (Line[1] <> '<') then
      Exit;

    P1 := Pos('>', Line);
    T1 := PosEx('\=', Line, P1) + 2;
    if (P1 = 0) or (T1 = 2) then
      Exit;

    Inc(ScriptCount);

    Offsets := Explode(',', Copy(Line, 2, P1 - 2));
    if Length(Offsets) = 0 then
      Error('Something is not right with your script... no <offsets>?');

    GetCharOffset(offsets);
    if Length(Offsets) = 0 then
      Error('Something is not right with your script... no <offsets> after character?');

    str := TBytes( UTF8Decode(Copy(Line, T1, $FFFF) ));
    if Length(Str) <> 0 then
    begin
      Individualize(Offsets);
      UpdateString( StrToInt('$' + Offsets[0]), Str, Jumps );
      Inc(Count)
    end;

    if (Length(Str) <> 0) or Option_UpdateUntranslated then
      UpdateChar
  end;

  procedure CleanUpFirstString(var Line: String);
  const
    TrimAway: array[0..2] of Char = (#$EF, #$BB, #$BF);
  begin
    if Line <> '' then
      // delete UTF-8 signature
      if (Line[1] = TrimAway[0]) and (Line[2] = TrimAway[1]) and (Line[3] = TrimAway[2]) then
        Line := Copy(Line, 4, $FFFF)
  end;

  procedure ReadChars(Path: String);
  const
    CHARACTER = '- Character: ';
  var
    Line: String;
    I, T1: Word;
  begin
    AssignFile(Strings, Path);
    Reset(Strings);

    try
      I := 0;

      while not EOF(Strings) do
      begin
        ReadLn(Strings, Line);
        if Copy(Line, 1, Length(CHARACTER)) = CHARACTER then
        begin
          T1 := Pos('\=', Line) + 2;
          if T1 <> 2 then
          begin
            Chars[I] := CopyBytes(TBytes(UTF8Decode(Copy(Line, T1, $FFFF))));
//            UTF8Decode(Copy(Line, T1, $FFFF), Chars[I]);
            Inc(I)
          end
        end
      end
    finally
      CloseFile(Strings)
    end;
  end;

  procedure ImportFiles(Path: String);        
  var
    FirstFile: Boolean;

    procedure ImportFile(Path: String);
    var
      I, LinesCount: Word;
    begin
      if FirstFile then
      begin
        FirstFile := False;
        Write('    * reading character list... ');
        ReadChars(Path);
        WriteLn('done.');
      end;

      AssignFile(Strings, Path);
      Reset(Strings);

      LinesCount := 0;

      try
        while not EOF(Strings) do
        begin
          ReadLn(Strings, Lines[LinesCount]);
          Inc(LinesCount)
        end
      finally
        CloseFile(Strings);
      end;

      if LinesCount = 0 then
        Error('No lines in this strings file');

      CleanUpFirstString(Lines[0]);

      ResetProgress;
      for I := LinesCount - 1 downto 0 do
      begin
        ParseLine( TrimLeft(Lines[I]) );
        
        if I mod 10 = 0 then
          UpdateProgress(I + 1, LinesCount);
      end;

      SetConsoleCursorPosition(GetConsole, GetXY(-1));
    end;
              
  var
    I: Byte;
    Strings: String;
  begin
    FirstFile := True;
    for I := 100 downto 1 do
    begin
      Strings := ChangeFileExt(Path, IntToStr(I) + '.utf');
      if FileExists(Strings) then
      begin
        WriteLn('  ', ExtractFileName(Strings), '...');
        ImportFile(Strings)
      end
    end;

    WriteLn('  ', ExtractFileName(Path), '...');
    ImportFile(Path); // first file doesn't have index appended to in its name

    SetConsoleCursorPosition(GetConsole, GetXY(-1));
    WriteLn( StringOfChar(' ', 79) )
  end;

begin
  Arc.Free;
  if not CopyFile(PChar(OriPath), PChar(Path), False) then
    Error('Error copying origianl CSX pack from ' + OriPath);
  OpenCSX(Path);

  Count := 0;
  ScriptCount := 0;

  WriteLn;

  Jumps := TJumpIndex.Create;
  try
    WriteLn('Loading jump index table:');
      Jumps.LoadFrom( ChangeFileExt(StringsPath, '.cji') );

    WriteLn;
    WriteLn('Importing strings...');
    ImportFiles(StringsPath);

    Writeln('Fixing jump offsets...');
      Jumps.ApplyModificationsTo(Arc);
  finally
    Jumps.Free;
  end;

  if Count <> 0 then
    WriteLn;

  if ScriptCount = 0 then
    Percent := 0
    else
      Percent := Count / ScriptCount;
  WriteLn(Format('%d strings updated (%d total; %.2f%% translated)', [Count, ScriptCount, Percent * 100]))
end;

end.
