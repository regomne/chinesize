#ifndef _DAITEIKOKU_H_
#define _DAITEIKOKU_H_

#include "pragma_once.h"
#include <Windows.h>
#include <shlobj.h>
#include "my_headers.h"
#include "gal_common.h"

#if defined(MY_COMPILER_INTEL)
    #pragma warning(disable:1899)
#endif

#define MIN_FONT_HEIGHT 0x30
#define MAX_FONT_HEIGHT 0x190
#define FONT_STEP       0x8
#define FONT_COUNT      ((MAX_FONT_HEIGHT - MIN_FONT_HEIGHT) / FONT_STEP)
#define pfChipSpriteEngAllocMemory_RVA 0x1580

#pragma pack(1)

typedef struct
{    
    HDC   hDC;
    ULONG LastFontIndex;
    HFONT hFont[FONT_COUNT];
    LONG  Descent[FONT_COUNT];
} FONT_OUTLINE_INFO;

typedef struct
{
    PVOID Unknown1;
    PVOID Unknown2;
    ULONG Unknown3;
    PVOID pvBuffer;
    PVOID pvBufferEnd;
} ALICE_MEMORY;
/*
typedef struct
{
    USHORT BitsPerRow;
    ULONG  Offset;
    ULONG  Size;
} ALICE_CHAR_OUTLINE_ENTRY;

typedef struct
{
    ULONG Height;
    ULONG Descent;      //  descent：描述某个字符在基准线下有多少扫描线。
    ...
} ALICE_CHAR_INFO_IN_MEMORY_ENTRY;

*/

typedef struct
{
    PVOID pVTalble;
    PVOID Unknown[6];
    PCHAR pBufferBase;
    ULONG BufferSize;
    PCHAR pBuffer;
    PCHAR pBufferEnd;
} ALICE_INI_INFO;

struct STL_STRINGA
{
    PVOID Unknown;
    union
    {
        CHAR  Buffer[0x10];
        PCHAR pBuffer;
    };
    ULONG Length;
    ULONG BufferSize;

    operator PCHAR()
    {
        return GetBuffer();
    }

    FORCEINLINE PCHAR GetBuffer()
    {
        return BufferSize > sizeof(Buffer) - 1 ? pBuffer : Buffer;
    }
};

#pragma pack()

#endif // _DAITEIKOKU_H_