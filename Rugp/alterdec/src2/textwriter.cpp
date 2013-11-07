
#include	"textwriter.h"

vector<dword> sOffsets;

/*
 *	CTextWriter
 */
CTextWriter::CTextWriter()
{
	lastOffset=0;
}


void CTextWriter::addLine(string str,dword off,dword oriHash,dword hash2)
{
	if(off)
	{
		lastOffset=off;
	}
	else
	{
		off=++lastOffset;
	}
	Msg m={str,off,oriHash,hash2};
	sOffsets.push_back(off);
	msgTbl.push_back(m);
}

void CTextWriter::addLine(const char* str,dword off,dword oriHash,dword hash2)
{
	if(off)
	{
		lastOffset=off;
	}
	else
	{
		off=++lastOffset;
	}
	Msg m={str,off,oriHash,hash2};
	sOffsets.push_back(off);
	msgTbl.push_back(m);
}


/*
 *	write
 *		offsetはファイル名に使用
 */
void CTextWriter::write( dword offset, dword hash )
{
	if ( msgTbl.empty() )
		return;

	char filename[16];

	sprintf( filename, "%08x.txt", offset );

	FILE *ft = fopen( filename, "wb" );
	if ( ft == NULL )
		throw "无法打开要写入的文本文件";

	sprintf( filename, "%08x.tbl", offset );
	FILE* fb=fopen(filename,"wb");
	if(fb==NULL)
		throw "无法打开要写入的表文件";

	for(int i=0;i<msgTbl.size();i++)
	{
		fwrite(msgTbl[i].str.c_str(),1,msgTbl[i].str.length(),ft);
		fwrite("\r\n",1,2,ft);
	}
	fclose( ft );

	fwrite(&offset,1,4,fb);
	fwrite(&hash,1,4,fb);
	dword temp=0;
	for(int i=0;i<msgTbl.size();i++)
	{
		fwrite(&msgTbl[i].offset,1,4,fb);
		fwrite(&msgTbl[i].oriHash,1,4,fb);
		fwrite(&msgTbl[i].hash2,1,4,fb);
		fwrite(&temp,1,4,fb);
	}

	fclose(fb);
}



/* 
 *	~CTextWriter
 */
CTextWriter::~CTextWriter()
{
}
