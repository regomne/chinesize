#pragma once
#include <Windows.h>

struct Segment
{
	LPVOID lpAddr;
	int nSize;
	int** lpRestruct;
};

struct AinSegs
{
	Segment Version;
	Segment Code;
	Segment Function;
	Segment GlobalVar;
	Segment GlobalSet;
	Segment Structs;
	Segment Message0;
	Segment Main;
	Segment MessageFunc;
	Segment HLL;
	Segment SwitchData;
	Segment GameVersion;
	Segment SLBL;
	Segment Strings;
	Segment FileName;
	Segment OnJump;
	Segment FunctionType;
	Segment KeyC;
	Segment ObjG;
};