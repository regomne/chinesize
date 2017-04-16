#include "stdafx.h"
#include "MalieExec.h"
#include "Malie_VMParse.h"


MalieExec::MalieExec(char *lpFileName)
{
	execstream bin(lpFileName);
	DWORD dwCnt = bin.readdw();
	for (;dwCnt;--dwCnt)//skip var
	{
		DWORD szLen = bin.readdw();
		_llseek(hFile,szLen&0x7FFFFFFF,FILE_CURRENT);
		GetNext(hFile);
		_llseek(hFile,sizeof(DWORD)*4,FILE_CURRENT);	
	}
	//至于这里为啥要跳过一个DW我也忘了、最初的解析里面没有写
	//  [7/20/2013 Azure]
	_llseek(hFile,sizeof(DWORD),FILE_CURRENT);//skip 0x3130 dwCnt = 0x5F4

	//////////////////////////////////////////////////////////////////////////
	// Function parse block
	_lread(hFile,&dwCnt,sizeof(DWORD));
//	freopen("functionList.txt","wt,ccs=UNICODE",stdout);
	for (;dwCnt;--dwCnt)//skip function
	{
		DWORD szLen;char temp[1000];DWORD ch[3];
		VM_FUNCTION func;
		_lread(hFile,&szLen,sizeof(DWORD));
		_lread(hFile,temp,(szLen&0x7FFFFFFF));
		_lread(hFile,ch,3*sizeof(DWORD));
		func.dwID = ch[0];
		func.wstrName = (WCHAR *)temp;
		func.dwVMCodeOffset = ch[2];
		func.dwReserved0 = ch[1];
		funcList[func.wstrName] = func;
		vecFuncList.push_back(func);
//		wprintf(L"%d,%ls,%d,0x%.4X\n",ch[0],temp,ch[1],ch[2]);
	}

	fprintf(stderr,"VM_FUNCTION:%d\n",funcList.size());

	//////////////////////////////////////////////////////////////////////////
	// Label parse block
	_lread(hFile,&dwCnt,sizeof(DWORD));
	for (;dwCnt;--dwCnt)//skip label
	{
		DWORD szLen;char temp[1000];DWORD ch;
		MALIE_LABEL label;
		_lread(hFile,&szLen,sizeof(DWORD));
		_lread(hFile,temp,(szLen&0x7FFFFFFF));
		_lread(hFile,&ch,sizeof(DWORD));
		label.wstrName = (WCHAR *)temp;
		label.dwVMCodeOffset = ch;
		labelList[label.wstrName] = label;
	}

	fprintf(stderr,"LABEL:%d\n",labelList.size());

	//////////////////////////////////////////////////////////////////////////
	// VM_DATA : just read to new area
	_lread(hFile,&szVM_DATA,sizeof(DWORD));
	//dump original scene
	pVM_DATA = new unsigned char[szVM_DATA];
	_lread(hFile,pVM_DATA,szVM_DATA);

	//////////////////////////////////////////////////////////////////////////
	// VM_CODE : just read to new area
	_lread(hFile,&szVM_CODE,sizeof(DWORD));//size
	pVM_CODE = new unsigned char [szVM_CODE];
	_lread(hFile,pVM_CODE,szVM_CODE);

//	fprintf(stderr,"system_onInit:0x%X\n",func_List.find(L"system_onInit")->second.dwVMCodeOffset);
	//////////////////////////////////////////////////////////////////////////
	// strTable
	DWORD tmp = _llseek(hFile,0,FILE_CURRENT);
	_lread(hFile,&szStrTable,sizeof(DWORD));//size
	pStrTable = new unsigned char[szStrTable];
	_lread(hFile,pStrTable,szStrTable);
	
	///////////////////////////////////////////
	// strIndex
	_lread(hFile,&cntStrIndex,sizeof(DWORD));//pair count
	pStrIndex = new STRING_INFO[cntStrIndex];
	_lread(hFile,pStrIndex,cntStrIndex*sizeof(STRING_INFO));

	_lclose(hFile);
	
}


MalieExec::~MalieExec(void)
{
	delete pStrTable;
	delete pStrIndex;
	delete pVM_DATA;
	delete pVM_CODE;
}


DWORD MalieExec::GetFuncOffset(wstring funcName)
{
	map<wstring,VM_FUNCTION>::iterator it = funcList.find(funcName);
	if (it!=funcList.end())
	{
		return it->second.dwVMCodeOffset;
	}
	return 0;
}


DWORD MalieExec::GetFuncOffset(int funcId)
{
	if (funcId<vecFuncList.size())
	{
		return move(vecFuncList[funcId].dwVMCodeOffset);
	}
	return 0;
}


wstring MalieExec::GetFuncName(int funcId)
{
	if (funcId<vecFuncList.size())
	{
		return move(vecFuncList[funcId].wstrName);
	}
	return wstring();
}


int MalieExec::GetFuncId(wstring funcName)
{
	map<wstring,VM_FUNCTION>::iterator it = funcList.find(funcName);
	if (it!=funcList.end())
	{
		return it->second.dwID;
	}
	return -1;
}
WCHAR buf_out[0x1000];
int len=0;
int MalieExec::ParseString(DWORD dwIndex)
{
	bool fl_rub=0,fl_vol=0;
	WCHAR *pLine=new WCHAR[pStrIndex[dwIndex].length/2+1];
	memset(pLine,0,sizeof(WCHAR)*(pStrIndex[dwIndex].length/2+1));
	memcpy(pLine,&pStrTable[pStrIndex[dwIndex].offset],pStrIndex[dwIndex].length);//○000000○ ●000000●
	WCHAR *pBuf=pLine;
	for (;DWORD(pBuf-pLine)<=pStrIndex[dwIndex].length/2;++pBuf)
	{
		switch (*pBuf)
		{
		case 0:
			if (fl_rub||fl_vol) fl_rub=fl_vol=0;
			buf_out[len++]=EOSTR;
			break;
		case 1:
			pBuf+=4;
			break;
		case 2:
			++pBuf;
			break;	
		case 3:
			pBuf+=2;
			break;
		case 4:
			++pBuf;
			break;
		case 5:
			pBuf+=2;
			break;
		case 6:
			pBuf+=2;
			break;
		case 0xA:
			if (fl_rub) buf_out[len++]=STRUB;
			else buf_out[len++]=L'\n';
			break;
		case 7:
			switch (*++pBuf)
			{
			case 0x0001://递归调用文字读取，然后继续处理（包含注释的文字）
				buf_out[len++]=TO_RUB;
				fl_rub=1;
				break;
			case 0x0004://下一句自动出来
				buf_out[len++]=NXL;
				break;
			case 0x0006://代表本句结束
				buf_out[len++]=TO_RTN;
				break;
			case 0x0007://递归调用文字读取然后wcslen，跳过不处理。应该是用于注释
				++pBuf;
				pBuf+=wcslen(pBuf);
				break;
			case 0x0008://LoadVoice 后面是Voice名
				buf_out[len++]=TO_VOL;
				fl_vol=1;
				break;
			case 0x0009://LoadVoice结束
				buf_out[len++]=EOVOL;
				break;
			}
			break;
		default:
			buf_out[len++]=*pBuf;
		}
// 		buf_out[len++]=EOPAR;
// 		buf_out[len++]=L'\n';
// 		buf_out[len++]=L'\n';
	}
	buf_out[len++]=EOPAR;
	buf_out[len++]=L'\n';
	buf_out[len++]=L'\n';
	wprintf(L"○%08d○\n%s●%08d●\n%s◆%08d◆\n\n\n",dwIndex,buf_out,dwIndex,buf_out,dwIndex);
	delete pLine;
	return 0;
}


int MalieExec::ExportStrByCode(void)
{
	CMalie_VMParse vm(this);
	vector<wstring> chapterName;
	vector<DWORD> chapterIndex;
	vector<DWORD> chapterRagon;
	vector<Malie_Moji> && moji = vm.ParseScenario(chapterName,chapterIndex);

	if (!chapterName.size())
	{
		vector<DWORD>::iterator it = unique(chapterIndex.begin(),chapterIndex.end());
		chapterIndex.erase(it,chapterIndex.end());
	}
	if (chapterIndex.size())
	{
		chapterRagon = chapterIndex;
		chapterRagon.erase(chapterRagon.begin());
		chapterRagon.push_back(moji.size());
		for (int i=0;i<chapterIndex.size();++i)
		{
			WCHAR name[0x100];
			if (i<chapterName.size())
			{
				swprintf(name,0x100,L"%02d %ls.txt",i,chapterName[i].c_str());
			}
			else
			{
				swprintf(name,0x100,L"%02d.txt",i);
			}
			_wfreopen(name,L"wt,ccs=UNICODE",stdout);
			for_each(moji.begin()+chapterIndex[i],moji.begin()+chapterRagon[i],[&](Malie_Moji x)
			{
				len=0;
				memset(buf_out,0,sizeof(WCHAR)*0x1000);
				if (!x.name.empty())
				{
					swprintf(buf_out,1000,L"%ls※",x.name.c_str());
					len+=x.name.size()+1;
				}
				ParseString(x.index);
			});
		}
	}
	else
	{
		_wfreopen(L"MalieMoji.txt",L"wt,ccs=UNICODE",stdout);
		for_each(moji.begin(),moji.end(),[&](Malie_Moji x)
		{
			len=0;
			memset(buf_out,0,sizeof(WCHAR)*0x1000);
			if (!x.name.empty())
			{
				swprintf(buf_out,1000,L"%ls※",x.name.c_str());
				len+=x.name.size()+1;
			}
			ParseString(x.index);
		});
	}
	return 0;
}


unsigned char * MalieExec::GetVMCodeBase(void)
{
	return pVM_CODE;
}


unsigned char * MalieExec::GetVMDataBase(void)
{
	return pVM_DATA;
}
