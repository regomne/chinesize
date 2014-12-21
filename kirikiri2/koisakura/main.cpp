#include <windows.h>

BOOL CreateAndInject(TCHAR* appName, TCHAR* dllName);

#pragma comment(linker, "/entry:main3")

int main2()
{
    wchar_t exeName[]=L"Áµ¤¬¤µ¤¯¤³¤í—@¤É¤­.exe";
    if(!CreateAndInject(exeName,L"crker.dll"))
    {
        MessageBox(0,L"Can't start Áµ¤¬¤µ¤¯¤³¤í—@¤É¤­.exe!",0,0);
        return 0;
    }

    return 0;
}

void main3()
{
    ExitProcess(main2());
}