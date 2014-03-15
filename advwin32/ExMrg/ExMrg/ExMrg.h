#pragma once

#include "stdafx.h"

enum MrgEncType:WORD
{
	MRG_RAW=0x0,
	MRG_LZSS,
	MRG_ENCANDLZ,
	MRG_ENC,
};

#pragma pack(1)
struct MrgFileEntry
{
	CHAR name[14];
	ULONG uncompressLen;
	MrgEncType type;
	FILETIME ftime;
	ULONG fileOffset;
};

struct MrgHeader
{
	CHAR magic[4];
	WORD key1;
	WORD key2;
	ULONG indexLen;
	ULONG fileCount;
};

#pragma pack()

class scoped_buffer {
public:
	scoped_buffer(SIZE_T size) { ptr_ = new BYTE[size]; }
	~scoped_buffer() { delete[] ptr_; }

	// Did the allocation succeed.
	bool alloc_failed() { return ptr_ == NULL; }
	BYTE* get() { return ptr_; }
private:
	BYTE* ptr_;
};
