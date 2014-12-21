#include <windows.h>

BOOL CreateAndInject(TCHAR* appName, TCHAR* dllName);

#pragma comment(linker, "/entry:main3")

int main2()
{
    wchar_t exeName[]=L"mecha_con_start.exe";
    if(!CreateAndInject(exeName,L"fucktpm.dll"))
    {
        MessageBox(0,L"无法启动和注入程序！",0,0);
        return 0;
    }

    return 0;
}

void main3()
{
    ExitProcess(main2());
}