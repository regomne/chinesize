// MalieVM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <locale.h>
#include <conio.h>
#include "MalieExec.h"
#include "Malie_VMParse.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL,"Japanese");
//	MalieExec exec("D:\\Software\\deCrypt\\KDays\\MalieSystem\\Dies irae  -Amantes amentes-\\exec.dat");
	MalieExec exec(R"(d:\Program Files\sousyu1\data\system\exec.dat)");
	exec.ExportStrByCode();
	_getch();
 	//CMalie_VMParse vm(&exec);//0xF8Aexec.GetFuncOffset(L"_ms_message")
 	//vm.diasm(exec.GetFuncOffset(L"maliescenario"),0x100000);
// 	freopen("out.txt","wt,ccs=UNICODE",stdout);
// 	exec.ExportStrByCode();
//	printf("0x%X",exec.GetFuncOffset(L"system_onInit"));
	return 0;
}