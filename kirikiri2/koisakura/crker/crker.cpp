#include <windows.h>
#include <ilhook.h>

void NewExeScript(wchar_t* script)
{
    if(!wcscmp(script,L"@set (PACKED=1)"))
    {
        wcscpy(script,L"@set (PACKED=0)");
    }
}

int HookExeScript()
{
    auto func=(PVOID)0x455800;
    BYTE* buff=(BYTE*)VirtualAlloc(0,1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

    HookSrcObject src;
    HookStubObject stub;

    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff,1000) ||
        !Hook32(&src,0,&stub,NewExeScript,"c"))
    {
        MessageBox(0,L"Can't hook func.",0,0);
        return FALSE;
    }

}

DWORD WINAPI DllMain(_In_ HANDLE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved)
{
    DisableThreadLibraryCalls((HMODULE)_HDllHandle);
    switch (_Reason)
    {
    case DLL_PROCESS_ATTACH:
        HookExeScript();
        break;
    default:
        break;
    }
    return TRUE;
}