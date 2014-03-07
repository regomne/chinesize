#include <windows.h>

BOOL CreateAndInject(TCHAR* appName, TCHAR* dllName);

#pragma comment(linker, "/entry:main3")

int main2()
{
    wchar_t exeName[]=L"hikari.exe";
	CreateAndInject(exeName,L"chn.dll");
    return 0;
}

void main3()
{
    ExitProcess(main2());
}