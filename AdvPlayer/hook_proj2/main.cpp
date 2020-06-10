#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <D3D9.h>

#include "ilhook.h"
#include "FileReader.h"
#include "FuncHelper.h"
#include "TxtSupport.h"

#include "ws2.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
	DP(nullptr, 0x44ddd,"\x75\x81","\xb8\xa1") //quote
};
#undef DP

__declspec(naked) void D3dCreate9()
{
	__asm jmp Direct3DCreate9
}

bool IsHalf(const string& s)
{
	for (auto c : s)
	{
		if ((unsigned char)c >= 0x80)
		{
			return false;
		}
	}
	return true;
}

void HOOKFUNC MyCFI(LPLOGFONTW lfi)
{
	//"ＭＳ ゴシック"
	if (wcscmp(lfi->lfFaceName, L"MS UI Gothic") == 0 ||
		wcscmp(lfi->lfFaceName, L"ＭＳ ゴシック") == 0)
	{
		wcscpy_s(lfi->lfFaceName, L"SimHei");
	}
}

void HOOKFUNC MyCW(wchar_t** strp)
{
	if (*strp && wcscmp(*strp, L"恋はそっと咲く花のように～二人は永遠に寄り添っていく～") == 0)
	{
		*strp = L"『恋心如花悄绚烂 ～二人永相伴～』";
	}
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason == DLL_PROCESS_ATTACH)
	{
		InitWs2();

		PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

		static const HookPointStruct points[] = {
			//_wfopen_s的所在函数，一个是函数头，一个函数中间call _wfopen_s用来读取arc的位置
		{ nullptr, 0x9220, MyOpenFile1, "12", 0, 0 }, //参数位置也需要适配
		{ nullptr, 0x98f0, MyOpenFile, "r", 0, 0 },

		//1.5版本 0F B6 ?? ?? ?? 89 5C 24 ?? 8B 2C 85 ?? ?? ?? 00 85 ED 0F 84
		//根据特征码找到一个函数，其内有三次对ReadInst函数的call，hook ReadInst的函数头
		//{ nullptr, 0xC6970, MyReadInst_v15, "rf", STUB_OVERRIDEEAX | STUB_DIRECTLYRETURN, 0x10 },

		//1.7版本 0f b6 45 ?? 33 ff 89 4d ?? 8b 04 85 ?? ?? ?? ?? 89 45 
		//根据特征码找到一个函数，其内有三次对ReadInst函数的call，hook ReadInst的函数头
		// 也可以直接根据extWs2中搜寻的指令表的地址，引用该指令表地址的指令所在的函数，即为上述要hook的函数
		//{ nullptr, 0xF0D40, MyReadInst_v17, "rf", STUB_OVERRIDEEAX | STUB_DIRECTLYRETURN, 0x14 },

		//1.9版本 根据extWs2中搜寻的指令表的地址，引用该指令表地址的指令所在的函数，即为上述要hook的函数
		//里面有两个局部变量偏移需要适配
		{ nullptr, 0xF0D40, MyReadInst_v19, "rf1234", STUB_OVERRIDEEAX | STUB_DIRECTLYRETURN, 0x10 },

		//1.5版本 8B 45 ?? 2B 45 ?? 8B 33 C7 44 24 ?? 01 00 00 00 3B F0 72 05 E8
		//此特征码头部即为hook地址，其下几十行之内有个参数是lpMultiByteStr的call
		//{ nullptr, 0xC6640, MySelString_v15, "r", 0, 0 },

		//1.7版本 C7 45 FC 02 00 00 00 8d 55 ?? 8b 4e ?? 03 0f e8
		//此特征码尾部的call指令即为hook地址，此处call的函数就是MyMbtowc hook位置所在的函数
		//或者找到MyReadInst_v17的特征码所在的函数，往上回溯一层(比较靠后)，找到Variant相关的函数，然后找参数是lpMultiByteStr的call
		//{ nullptr, 0xD6D98, MySelString_v17, "r", 0, 0 },

		//此位置在上面那个call之内，call MultiByteToWideChar的位置
		//{ nullptr, 0xC393B, MyMbtowc, "r", 0, 0 },

		//1.9版本，里面有两个局部变量偏移需要适配
        { nullptr, 0xf093f, MySelString_v19, "r", 0, 0 },

		//wcsncmp的第二个引用所在的函数头, 参数位置需要适配
		//{ nullptr, 0x410E0, MyChangeFont_v15, "r", 0, 0 },
		//{ nullptr, 0x410E0, MyChangeFont_v17, "\x04", 0, 0 },
		};

		if (!HookFunctions(points))
		{
			MessageBox(0, L"Hook 失败！", 0, 0);
			return TRUE;
		}

		static const HookPointStructWithName points2[] = {
			{ "gdi32.dll", "CreateFontIndirectW", MyCFI, "1", 0, 0 },
			{ "user32.dll", "CreateWindowExW", MyCW, "\x03", 0, 0 },
			{ "kernel32.dll", "MultiByteToWideChar", MyMbToWc2, "\x01\x02\x03\x04", 0, 0 },
		};
		if (!HookFunctions(points2))
		{
			MessageBox(0, L"Hook2 失败！", 0, 0);
		}
	}
	return TRUE;
}