
#pragma once



#include	"common.h"




class CTextWriter
{
	struct Msg{
		string str;
		dword offset;
		dword oriHash;
		dword hash2;
	};

	vector<Msg> msgTbl;
	dword lastOffset;

public:
			CTextWriter();
	void	addLine(string str,dword off,dword oriHash,dword hash2);
	void	addLine(const char* str,dword off,dword oriHash, dword hash2);
	void	write( dword offset, dword hash );
			~CTextWriter();
};

