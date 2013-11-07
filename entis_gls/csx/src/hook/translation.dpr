library translation;

uses
  Windows;

type
  PImageImportDescriptor = ^TImageImportDescriptor;
  TImageImportDescriptor = packed record
    OriginalFirstThunk: DWORD;
    TimeDateStamp: DWORD;
    ForwarderChain: DWORD;
    Name: DWORD;
    FirstThunk: DWORD;
  end;

{$R *.res}

procedure Error(Msg: PChar);
begin
  MessageBox(0, Msg, 'Wanko to Lily translation hook', 0);
  ExitProcess(1)
end;

procedure FixNewlines(var Str: PWideChar);
var
  I: Word;
  CleanStr: WideString;
begin
  CleanStr := Str;

  for I := Length(CleanStr) downto 1 do
    if CleanStr[I] = #10 then
      Delete(CleanStr, I, 1);

  Move(CleanStr[1], Str^, lstrlenw(Str) * 2);
end;

function HookText(DC: HDC; var TM: TTextMetricA): BOOL; stdcall;
const
  HookTo = $00463A6B;
var
  Ret: DWord;
  Str: PWideChar;
begin
  asm
    MOV EAX, SS:ESP+12
    MOV Ret, EAX
    MOV EAX, SS:ESP+28
    MOV Str, EAX
  end;
                                                   
  if (Ret = HookTo) and (DWord(Str) > $10000) and (lstrlenw(Str) > 30) then
    FixNewlines(Str);

  Result := GetTextMetricsA(DC, TM)
end;

procedure Setup;
var
  ImageBase, Old: Cardinal;
  PEHeader: PImageNtHeaders;
  PImport: PImageImportDescriptor;
  PRVA_Import: LPDWORD;
  ProcAddress: Pointer;
  Found: Boolean;
begin
  ImageBase := GetModuleHandle(nil);
  PEHeader := Pointer(Int64(ImageBase) + PImageDosHeader(ImageBase)._lfanew);
  PImport := Pointer(PEHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress + ImageBase);
  ProcAddress := GetProcAddress(GetModuleHandle('gdi32.dll'), 'GetTextMetricsA');
  if ProcAddress = NIL then
    Error('GetProcAddress(TextOutA)');
  Found := False;
  while PImport.Name <> 0 do
  begin
    PRVA_Import := LPDWORD(pImport.FirstThunk + ImageBase);
    while PRVA_Import^ <> 0 do
    begin
      if PPointer(PRVA_Import)^ = ProcAddress then
      begin
        Found := True;
        if not VirtualProtect(PPointer(PRVA_Import), 4, PAGE_READWRITE, Old) then
          Error('do- VirtualProtect()');
        PPointer(PRVA_Import)^ := @HookText;
        if not VirtualProtect(PPointer(PRVA_Import), 4, Old, Old) then
          Error('un- VirtualProtect()')
      end;
      Inc(PRVA_Import)
    end;
    Inc(PImport)
  end;
  if Found = False then
    Error('No GetTextMetricsA in the import table. Wrong EXE? Wrong game? Wrong version?')
end;

begin
  Setup
end.
