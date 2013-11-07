program Translate;

uses
  Windows;

{$R *.res}

var
  DLLPath, Cresus: PChar;
  hC, I, hT: Cardinal;
  PI: TProcessInformation;
  PLib, LL: Pointer;
  SI: TStartupInfo;


function ExtractFilePath(FileName: String): String;
var
  I: Word;
begin
  Result := '';
  I := Length(FileName);
  while (I > 1) and (FileName[I] <> '\') do
    Dec(I);
  Result := Copy(FileName, 1, I)
end;

function FileExists(const FileName: String): Boolean;
type
  LongRec = packed record
    case Integer of
      0: (Lo, Hi: Word);
      1: (Words: array [0..1] of Word);
      2: (Bytes: array [0..3] of Byte);
  end;

  // SysUtils.pas: 4937
  function FileAge(const FileName: String): Integer;
  var
    Handle: THandle;
    FindData: TWin32FindData;
    LocalFileTime: TFileTime;
  begin
    Handle := FindFirstFile(PChar(FileName), FindData);
    if Handle <> INVALID_HANDLE_VALUE then
    begin
      Windows.FindClose(Handle);
      if (FindData.dwFileAttributes and FILE_ATTRIBUTE_DIRECTORY) = 0 then
      begin
        FileTimeToLocalFileTime(FindData.ftLastWriteTime, LocalFileTime);
        if FileTimeToDosDateTime(LocalFileTime, LongRec(Result).Hi,
          LongRec(Result).Lo) then Exit;
      end;
    end;
    Result := -1;
  end;

begin
  Result := FileAge(FileName) <> -1;
end;


// SysUtils.pas: 4273
procedure CvtInt;
asm
        OR      CL,CL
        JNZ     @CvtLoop
@C1:    OR      EAX,EAX
        JNS     @C2
        NEG     EAX
        CALL    @C2
        MOV     AL,'-'
        INC     ECX
        DEC     ESI
        MOV     [ESI],AL
        RET
@C2:    MOV     ECX,10

@CvtLoop:
        PUSH    EDX
        PUSH    ESI
@D1:    XOR     EDX,EDX
        DIV     ECX
        DEC     ESI
        ADD     DL,'0'
        CMP     DL,'0'+10
        JB      @D2
        ADD     DL,('A'-'0')-10
@D2:    MOV     [ESI],DL
        OR      EAX,EAX
        JNE     @D1
        POP     ECX
        POP     EDX
        SUB     ECX,ESI
        SUB     EDX,ECX
        JBE     @D5
        ADD     ECX,EDX
        MOV     AL,'0'
        SUB     ESI,EDX
        JMP     @z
@zloop: MOV     [ESI+EDX],AL
@z:     DEC     EDX
        JNZ     @zloop
        MOV     [ESI],AL
@D5:
end;

function IntToStr(Value: Integer): String;
asm
        PUSH    ESI
        MOV     ESI, ESP
        SUB     ESP, 16
        XOR     ECX, ECX
        PUSH    EDX
        XOR     EDX, EDX
        CALL    CvtInt
        MOV     EDX, ESI
        POP     EAX
        CALL    System.@LStrFromPCharLen
        ADD     ESP, 16
        POP     ESI
end;


procedure Error(Msg: PChar);
begin
  MessageBox(0, PChar(Msg + ' (#' + IntToStr(GetLastError) + ')'#10#10 +
             '! MAKE SURE your antivirus is disabled !!'), 'Wanko to Lily translation hook', 0);
  Halt(1)
end;

begin
  DLLPath := PChar(ExtractFilePath(ParamStr(0)) + 'translation.dll');
  if not FileExists(DLLPath) then
    Error('No translation.dll found');
  Cresus := PChar(ExtractFilePath(ParamStr(0)) + 'wanko.exe');
  if not FileExists(Cresus) then
    Error('No wanko.exe in the current directory.'#10'Make sure you''ve copied the translation to the game''s directory');

  FillChar(SI, SizeOf(SI), 0);
  if not CreateProcess(Cresus, '', NIL, NIL, False, CREATE_SUSPENDED, NIL, PChar(ExtractFilePath(ParamStr(0))), SI, PI) then
    Error('Cannot create a process');
  hC := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_CREATE_THREAD or PROCESS_VM_OPERATION or PROCESS_VM_WRITE, False, PI.dwProcessId);
  if hC = 0 then
    Error('Cannot open a process');
  PLib := VirtualAllocEx(hC, NIL, Length(DLLPath), MEM_COMMIT, PAGE_READWRITE);
  if PLib = NIL then
     Error('Cannot call VirtualAllocEx');
  if not WriteProcessMemory(hC, PLib, DLLPath, Length(DLLPath), I) then
    Error('Cannot write to the process');
  LL := GetProcAddress(GetModuleHandle(Kernel32), 'LoadLibraryA');
  if LL = NIL then
    Error('Cannot GetProcAddress(LoadLibraryA)');
  hT := CreateRemoteThread(hC, NIL, 0, LL, PLib, 0, I);
  if hT = 0 then
    Error('Cannot create a thread');
  WaitForSingleObject(hT, INFINITE);
  if ResumeThread(PI.hThread) = $FFFFFFFF then
    Error('Cannot run the thread');

  VirtualFreeEx(hC, PLib, Length(DLLPath), MEM_DECOMMIT);
  CloseHandle(hT);
  CloseHandle(hC);
end.
