// ExMrg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ExMrg.h"
using namespace std;

HANDLE gConsoleHandle;
#define CONST_STRING_WITH_LEN(x) x,_tcslen(x)

void LzssUncompress(BYTE *uncompr, BYTE *compr, DWORD comprlen)
{
	unsigned int act_uncomprlen = 0;
	/* compr中的当前字节中的下一个扫描位的位置 */
	unsigned int curbit = 0;
	/* compr中的当前扫描字节 */
	unsigned int curbyte = 0;
	unsigned int nCurWindowByte = 0xfee;
	unsigned int win_size = 4096;
	BYTE win[4096];
	WORD flag = 0;

	memset(win, 0, nCurWindowByte);
	while (curbyte<comprlen) {
		flag >>= 1;
		if (!(flag & 0x0100))
			flag = compr[curbyte++] | 0xff00;

		if (flag & 1) {
			unsigned char data;

			data = compr[curbyte++];
			win[nCurWindowByte++] = data;
			uncompr[act_uncomprlen++] = data;
			/* 输出的1字节放入滑动窗口 */
			nCurWindowByte &= win_size - 1;
		} else {
			unsigned int copy_bytes, win_offset;
			unsigned int i;

			win_offset = compr[curbyte++];
			copy_bytes = compr[curbyte++];
			win_offset |= (copy_bytes & 0xf) << 8;
			copy_bytes >>=4;
			copy_bytes += 3;

			for (i = 0; i < copy_bytes; i++) {	
				unsigned char data;

				data = win[(win_offset + i) & (win_size - 1)];				
				uncompr[act_uncomprlen++] = data;
				/* 输出的1字节放入滑动窗口 */
				win[nCurWindowByte++] = data;
				nCurWindowByte &= win_size - 1;	
			}
		}
	}
}

bool DecMrgIndex(MrgHeader& hdr,BYTE* index)
{
	BYTE key;
	if(hdr.key1==2 && hdr.key2==1)
		key=0x96;
	else
		throw string("Header not supported.");

	int i;
	BYTE* p=index;
	i=hdr.indexLen-sizeof(MrgHeader);
	for(int j=0;j<hdr.indexLen-sizeof(MrgHeader);j++)
	{
		p[j]=((p[j]<<1)|(p[j]>>7))^key;
		key+=i;
		i--;
	}
	return true;
}

ULONG GetMask(ULONG val)
{
	if(val<=0x100)
		return 0xff;
	else if(val<=0x200)
		return 0x1ff;
	else if(val<=0x400)
		return 0x3ff;
	else if(val<=0x800)
		return 0x7ff;
	else if(val<=0x1000)
		return 0xfff;
	else if(val<=0x2000)
		return 0x1fff;
	else if(val<=0x4000)
		return 0x3fff;
	else if(val<=0x8000)
		return 0x7fff;
	else
		return 0xffff;
}

DWORD DecAFile(BYTE* toDec,BYTE* newMem)
{
	DWORD decTbl1[0x101];
	DWORD decTbl2[0x101];
	DWORD dataRawLen=*(DWORD*)toDec^*(DWORD*)(toDec+0x104);
	BYTE* p=toDec+4;

	decTbl1[0]=0;
	for(int i=0;i<0x100;i++)
	{
		decTbl1[i+1]=p[i]+decTbl1[i];
		decTbl2[i]=p[i];
	}

	ULONG mask=GetMask(decTbl1[0x100]);

	scoped_buffer tbuff(decTbl1[0x100]);
	p=tbuff.get();
	for(int i=0;i<0x100;i++)
		for(int j=0;j<decTbl2[i];j++)
			*p++=i;

	BYTE* p2=toDec+0x104;
	DWORD xor=(*p2<<24)+(*(p2+1)<<16)+(*(p2+2)<<8)+*(p2+3);
	p2+=4;
	int factor=0x10000/decTbl1[0x100];

	BYTE* pn=newMem;
	DWORD key1=-1,key2=0;
	p=tbuff.get();
	while(pn<newMem+dataRawLen)
	{
		key1=((key1>>8)*factor)>>8;
		DWORD val=p[(xor-key2)/key1];
		key2+=decTbl1[val]*key1;
		key1*=decTbl2[val];
		*pn++=(BYTE)val;
		while(! (((key1+key2)^key2)&0xff000000))
		{
			key1<<=8;
			key2<<=8;
			xor=(xor<<8)|*p2++;
		}
		while(! (key1&~mask))
		{
			key1=(~key2)&mask;
			key1<<=8;
			key2<<=8;
			xor=(xor<<8)|*p2++;
		}
	}
	return dataRawLen;
}

void Extract(FILE* fpMrg,MrgHeader& hdr,BYTE* index)
{
	map<string,bool> extSet;
	MrgFileEntry* entry=(MrgFileEntry*)index;
	FILE* fps;
	for(DWORD i=0;i<hdr.fileCount;i++,entry++)
	{
		int tmp=string(entry->name).rfind('.');
		string ext=string(&entry->name[tmp+1]);
		if(extSet.count(ext))
			continue;
		extSet[ext]=true;

		scoped_buffer finalbf(entry->uncompressLen);
		switch(entry->type)
		{
		case MRG_ENC:
			{
				DWORD size=entry[1].fileOffset-entry->fileOffset;
				fseek(fpMrg,entry->fileOffset,SEEK_SET);
				scoped_buffer decbuff(size);
				fread(decbuff.get(),1,size,fpMrg);
				DecAFile(decbuff.get(),finalbf.get());
			}
			break;
		case MRG_ENCANDLZ:
			{
				DWORD size=entry[1].fileOffset-entry->fileOffset;
				fseek(fpMrg,entry->fileOffset,SEEK_SET);
				scoped_buffer decbuff(size);
				fread(decbuff.get(),1,size,fpMrg);
				scoped_buffer uncbuff(entry->uncompressLen);
				DWORD decSize=DecAFile(decbuff.get(),uncbuff.get());
				LzssUncompress(finalbf.get(),uncbuff.get(),decSize);
			}
			break;
		case MRG_LZSS:
			{
				DWORD size=entry[1].fileOffset-entry->fileOffset;
				fseek(fpMrg,entry->fileOffset,SEEK_SET);
				scoped_buffer uncbuff(size);
				fread(uncbuff.get(),1,size,fpMrg);
				LzssUncompress(finalbf.get(),uncbuff.get(),size);
			}
			break;
		case MRG_RAW:
			fseek(fpMrg,entry->fileOffset,SEEK_SET);
			fread(finalbf.get(),1,entry->uncompressLen,fpMrg);
			break;
		default:
			cout<<entry->name<<" unknown type."<<endl;
		}
		fps=fopen(entry->name,"wb");
		if(!fps)
		{
			cout<<entry->name<<" can't be created."<<endl;
			continue;
		}
		fwrite(finalbf.get(),1,entry->uncompressLen,fps);
		fclose(fps);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	gConsoleHandle=GetStdHandle(STD_OUTPUT_HANDLE);

	//DWORD dwTemp;
	//if(argc!=2 && argc!=3)
	//{
	//	cout<<"usage error."<<endl;
	//	return 0;
	//}

	FILE *fpMrg;
	//fpMrg=_wfopen(argv[1],L"rb");
	fpMrg=fopen("F:/Program Files/C2_DVD/game02.mrg","rb");
	if(!fpMrg)
	{
		cout<<"Can't open file."<<endl;
		return 0;
	}

	MrgHeader mrgHdr;
	int bytesRead=fread(&mrgHdr,1,sizeof(mrgHdr),fpMrg);

	scoped_buffer mindexbf(mrgHdr.indexLen);
	BYTE* mrgIndex=mindexbf.get()+sizeof(mrgHdr);
	fseek(fpMrg,sizeof(mrgHdr),SEEK_SET);
	bytesRead=fread(mrgIndex,1,mrgHdr.indexLen-sizeof(mrgHdr),fpMrg);
	try
	{
		bool rslt=DecMrgIndex(mrgHdr,mrgIndex);
	}
	catch(string s)
	{
		cout<<s.c_str()<<endl;
		return 0;
	}

	//if(argc==3)
		SetCurrentDirectory(L"F:/Program Files/C2_DVD/game02");

	Extract(fpMrg,mrgHdr,mrgIndex);

	fclose(fpMrg);

	return 0;
}

