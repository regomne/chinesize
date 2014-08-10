#pragma once

#include <Windows.h>
#include "..\shell\fxckBGI.h"
#include <string>

#define DECOMPTYPE_CDECL 1
#define DECOMPTYPE_FASTCALL 2

extern DecoprFunc g_DecompressFile;
extern int g_DecompType;
extern HWND g_hwndMain;

extern HANDLE g_workingThread;
extern HANDLE g_paused;
extern HANDLE g_needStop;

extern std::wstring g_extPath;

DWORD WINAPI ExtractThread(LPVOID param);

typedef struct
{
    CHAR    Magic[0xC];    // "PackFile    " or "BURIKO ARC20"
    ULONG   FileCount;
} BGI_ARC_HEADER;

typedef struct
{
    CHAR    FileName[0x10];
    ULONG   Offset;
    ULONG   Size;
    ULONG   Unknown1;
    ULONG   DataSize;
} BGI_ARC_ENTRY;

typedef struct
{
    CHAR    FileName[0x60];
    ULONG   Offset;
    ULONG   Size;
    ULONG   Unknown1;
    ULONG   DataSize;
	ULONG	Unknown2[4];
} BGI_ARC_ENTRY2;

#pragma pack(1)
typedef struct
{
    CHAR    Magic[0x10];  // "CompressedBG___"
    USHORT  Width;
    USHORT  Height;
    USHORT  BitsPerPixel;
	USHORT  unk1;
    ULONG   Reserve[2];
    ULONG   Unknown;
    ULONG   Key;
    ULONG   CompressedSize;
    BYTE    CheckSum;
    BYTE    CheckXor;
    USHORT  _Pad;
} BGI_COMPRESSED_BG_HEADER;

typedef struct
{
    CHAR    Magic[0x10];  // "DSC FORMAT 1.00"
    ULONG   Key;
    ULONG   DecompressSize;
    LONG    DataSize;
    ULONG   Unknown;
    BYTE    Dict[0x200];
} BGI_DSC_FORMAT_HEADER;

typedef struct
{
    CHAR    Magic[0x10];  // "SDC FORMAT 1.00"
    ULONG   Key;
    ULONG   CompressedSize;
    ULONG   DecompressedSize;
    USHORT  CheckSum;
    USHORT  CheckXor;
} BGI_SDC_FORMAT_HEADER;

typedef struct
{
    USHORT Width;
    USHORT Height;
    USHORT BitsPerPixel;
    USHORT Unknown;
    USHORT BppIndex;
    USHORT Unknown2;
    USHORT Unknown3;
    USHORT Unknown4;
} BGI_GRP_HEADER;

typedef struct
{
    ULONG cbSize;
    ULONG Magic;            // == TAG4('bw  ')
    ULONG AudioSteamSize;
    ULONG Samples;
    ULONG Rate;
    ULONG Channels;
    BYTE  Unknown1[0x18];
    ULONG State;
    BYTE  Unknown2[0xC];
} BGI_SOUND_HEADER;

#pragma pack()

enum PicType
{
	PIC_TYPE_PNG=0,
	PIC_TYPE_JPEG,
};

struct Options
{
	BOOL isBlend;
	BOOL isEncode;
	PicType picType;
};

struct MemStream
{
	BYTE* start;
	BYTE* cur;
	DWORD len;
};