program CSX;

{$APPTYPE CONSOLE}

uses
  Windows,
  SysUtils,
  ConsoleUtils,
  CSX_Reading,
  CSX_Export,
  CSX_Import,
  CSX_JumpIndex;
  
// note: there also exists some kind of 'compiled' CSX where
//       function names are not in Unicode but are 'mangled'
//       and often using binary codes (not within Unicode range).

const
  Header = '~ Cotopha scripts (CSX) strings extractor/compiler ~   v30.4.10'#10 +
           '  by Proger_XP, mailme@smtp.ru'#10;
  Help = #10 +
         '    csx.exe <pack.csx> [[!]import|export-no-names|export [<strings.utf> [<original.csx>]]]'#10 +
         #10 +
         '  Use "!" to disable waiting before closing.'#10 +
         '  Use "export-no-names" to export texts from games that fail to be exported'#10 +
         'using normal export command. This won''t make difference for the import command'#10 +
         'but you won''t be able to change character names.'#10 +
         '  If for some reason you''ve lost a jump table index file you can'#10 +
         'regenerate it using this command (the syntax is the same as for exporting):'#10 +
         '    csx.exe <pack.csx> makejumpindex'#10 +
         #10 +
         '  You can also customize branching indication for export (with/ without names)'#10 +
         'operations. For this, append "#n" to the operation name. Examples:'#10 +
         '    csx.exe yosuga.csx export#0   - this will disable branching display;'#10 +
         '    csx.exe yosuga.csx export-no-names#1'#10 +
         '                                  - indicate branching using indentation;'#10 +
         '    csx.exe yosuga.csx export#2   - include detailed branching info in the'#10 +
         '                                    strings file, marking jumps using lines'#10 +
         '                                    "v branch till" and "^ branch end".'#10 +
         #10 +
         '  Note that this setting does not affect importing in any way so use whatever'#10 +
         'looks better to you. The "#0" mode ("turned off") is the default.'#10 +
         '  Also note that CSX files tend to abuse branching instructions so it might'#10 +
         'look "unnatural" in a string file.'
         ;

var
  Operation: String;
  NoWait: Boolean;

procedure OutputConfig;
begin
  WriteLn(Format('CSX file (for export) is %s'#10 +
                  'Original (untranslated) CSX file (for import) is %s'#10 +
                  'Jump index table file (for both) is %s',
                  [Path, OriPath, ChangeFileExt(StringsPath, '.cji')]));

  if (Operation <> 'makejumpindex') and (Operation <> 'outputjumpindex') then
    WriteLn(#10'Strings file (UTF-8) is ' + StringsPath);

  if (Operation <> 'import') and (Operation <> 'makejumpindex') and (Operation <> 'outputjumpindex') then
  begin
    WriteLn('There will be ', LinesPerFile, ' lines per one file.');

    Write('Branching indication mode: ');
    case Option_BranchDisplay of
    0: WriteLn('turned off.');
    1: WriteLn('indentation only.');
      else
        WriteLn('detailed.');
    end;
  end;

  if not Option_CollectCharacters then
    WriteLn('Exporting without names - mode on.');
  WriteLn
end;

function IsDirectory(var Path: String): Boolean;
var
  SR: TWin32FindData;
  Handle: DWord;
begin
  Result := False;
  if Path[Length(Path)] = '\' then
    Path := Copy(Path, 1, Length(Path) - 1);

  Handle := FindFirstFile(PChar(Path), SR);
  if Handle <> INVALID_HANDLE_VALUE then
  begin
    Result := SR.dwFileAttributes and FILE_ATTRIBUTE_DIRECTORY <> 0;
    Windows.FindClose(Handle)
  end
end;

begin
  WriteLn(Header);

  Path := Trim(ParamStr(1));
  if Path = '' then
    Error(' You should pass a path to a CSX file as the first parameter.'#10#9 + Help);

  StringsPath := Trim(ParamStr(3));
  if StringsPath = '' then
    StringsPath := ChangeFileExt(Path, '.utf');
  if IsDirectory(StringsPath) then
    StringsPath := StringsPath + '\' + ChangeFileExt(ExtractFileName(Path), '.utf');

  Operation := Trim(ParamStr(2));
  if Operation = '' then
    if ExtractFileExt(Path) = '.utf' then
    begin
      StringsPath := Path;
      Path := ChangeFileExt(Path, '.csx');
      Operation := 'import';
    end
      else
        Operation := 'export';  

  NoWait := Operation[1] = '!';
  if NoWait then
    Operation := Copy(Operation, 2, $FF);

  if Operation[ Length(Operation) - 1 ] = '#' then
    try
      Option_BranchDisplay := StrToInt( Operation[Length(Operation)] );
      if Option_BranchDisplay > 2 then
        Option_BranchDisplay := 2;
      Operation := Copy(Operation, 1, Length(Operation) - 2);
    except
    end;

  OriPath := Trim(ParamStr(4));
  if OriPath = '' then
    OriPath := ChangeFileExt(Path, '_original.csx');

  Option_CollectCharacters := Operation <> 'export-no-names';
  OutputConfig;

{  OpenCSX(Path);
  writeln (Functions[FindWhichFunc($11C414)].name);
  readln;

{  S := #$4F#$0;
  UpdateString($11C593, TBytes(S)); }

  if Operation = 'import' then
    ImportStrings
    else if Operation = 'makejumpindex' then
      CreateJumpIndex
      else if Operation = 'outputjumpindex' then
        OutputJumpIndex
        else
          ExtractAndSaveStrings;

  WriteLn(#10#13'Success.');
  if not NoWait then
    ReadLn
end.
