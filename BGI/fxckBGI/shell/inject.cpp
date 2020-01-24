#include <windows.h>
#include <stdint.h>
#include "shell.h"
#include "fxckBGI.h"

#define CODE_OFFSET 1024

//__declspec(naked) int LoadLib()
//{
//	__asm{
//		//_emit 0xcc
//		pushad
//		call lbl
//		lbl:
//		pop ebx
//		sub ebx,CODE_OFFSET+6
//		
//		lea eax,[ebx+4]
//		push eax
//		lea eax,[ebx+8]
//		push eax
//		xor eax,eax
//		push eax
//		push eax
//		call [ebx]
//		popad
//		_emit 0xff
//		_emit 0xff
//		_emit 0xff
//		_emit 0xff
//		_emit 0xff
//	}
//}

static const uint8_t LoadLib[] = {
    0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x81, 0xEB, 0x06, 0x04, 0x00, 0x00, 0x8D, 0x43, 0x04,
    0x50, 0x8D, 0x43, 0x08, 0x50, 0x33, 0xC0, 0x50, 0x50, 0xFF, 0x13, 0x61
};

int InjectToProcess(HANDLE process, HANDLE thread, wchar_t* dllPath, DecoprFunc func)
{
    BYTE* CodeAddr = (BYTE*)VirtualAllocEx(process, 0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!CodeAddr)
        return -1;
    DWORD strLen = lstrlen(dllPath);
    PWCH StringAddr = (PWCH)VirtualAllocEx(process, 0, (strLen + 1) * 2, MEM_COMMIT, PAGE_READWRITE);
    if (!StringAddr)
        return -1;

    MyDataStruct mds;

    mds.dllName.Buffer = StringAddr;
    mds.dllName.Length = (WORD)(strLen * 2);
    mds.dllName.MaximumLength = (strLen + 1) * 2;

    mds.funcAddr = func;
    mds.loadDll = LdrLoadDll;

    auto jmpPos = sizeof(LoadLib);

    CONTEXT ctt;
    ctt.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
    NTSTATUS val = NtGetContextThread(thread, &ctt);
    if (!NT_SUCCESS(val))
        return -1;

    DWORD readBytes;
    BYTE inst[8];
    BOOL rslt = WriteProcessMemory(process, CodeAddr, &mds, sizeof(mds), &readBytes);
    if (!rslt)
    {
        wchar_t err[100];
        wsprintfW(err, L"1, bytes: %d, Err:%d", readBytes, GetLastError());
        MessageBox(nullptr, err, nullptr, 0);
        return -1;
    }
    rslt = rslt && WriteProcessMemory(process, CodeAddr + CODE_OFFSET, LoadLib, jmpPos, &readBytes);
    if (!rslt)
    {
        wchar_t err[100];
        wsprintfW(err, L"2, bytes: %d, Err:%d", readBytes, GetLastError());
        MessageBox(nullptr, err, nullptr, 0);
        return -1;
    }
    inst[0] = 0xe9;
    *(DWORD*)(&inst[1]) = ctt.Eip - ((DWORD)CodeAddr + CODE_OFFSET + jmpPos + 5);
    rslt = rslt && WriteProcessMemory(process, CodeAddr + CODE_OFFSET + jmpPos, inst, 5, &readBytes);
    if (!rslt)
    {
        wchar_t err[100];
        wsprintfW(err, L"3, bytes: %d, Err:%d", readBytes, GetLastError());
        MessageBox(nullptr, err, nullptr, 0);
        return -1;
    }
    rslt = rslt && WriteProcessMemory(process, StringAddr, dllPath, strLen * 2, &readBytes);
    if (!rslt)
    {
        wchar_t err[100];
        wsprintfW(err, L"4, bytes: %d, Err:%d", readBytes, GetLastError());
        MessageBox(nullptr, err, nullptr, 0);
        return -1;
    }

    ctt.Eip = (DWORD)CodeAddr + CODE_OFFSET;
    if (!NT_SUCCESS(NtSetContextThread(thread, &ctt)))
        return -1;

    return 0;
}

int PipeComm(HANDLE pipe, DWORD val)
{
    BOOL fConnected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!fConnected)
    {
        return -1;
    }

    DWORD readBytes;
    WriteFile(pipe, &val, 4, &readBytes, 0);
    if (readBytes != 4)
    {
        DisconnectNamedPipe(pipe);
        return -1;
    }

    ReadFile(pipe, &val, 4, &readBytes, 0);

    if (val != 'OVER')
    {
        DisconnectNamedPipe(pipe);
        return -1;
    }
    DisconnectNamedPipe(pipe);
    return 0;
}