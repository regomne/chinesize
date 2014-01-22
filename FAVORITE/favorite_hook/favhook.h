#pragma once

#include <windows.h>

#pragma pack(1)

struct StackEntry
{
	UCHAR   type;
	UCHAR   unk;
	USHORT  stackBase;
	ULONG   value;

};

struct ScriptObject
{
	BYTE unk[4];                        // 0x000

	PVOID ScriptBuffer;                     // 0x004
	StackEntry StackEntry[0x100];  // 0x008

	BYTE unk2[0x14];                     // 0x808

	PVOID NextOpOfffset;                // 0x81C
	PVOID StackTop;                     // 0x820

};

struct StringTable
{
	DWORD* offsets;
	int cnt;
	char** stringList;
};
#pragma pack()
