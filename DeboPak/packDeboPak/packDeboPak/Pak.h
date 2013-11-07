#pragma once

#pragma pack(1)

struct PakHeader{
	BYTE magic[4];		// "PAK\0"
	WORD index_info_offset;
	WORD unknown0;		// 6
	WORD resreved0;		// 0
	WORD zero;			// 0
	DWORD resreved1;		// 0
};

struct PakInfo{
	DWORD index_info_size;
	DWORD reserved0;		// 0
	DWORD dir_entries;
	DWORD uncomprlen;
	DWORD comprlen;
	DWORD reserved1;		// 0
};

struct PakEntry{
	QWORD offset;			// from 0
	QWORD uncomprlen;
	QWORD comprlen;
	DWORD flags;
	FILETIME CreateTime;
	FILETIME LastAccess;
	FILETIME LastWrite;
	//s8 *name;
};

#pragma pack()
