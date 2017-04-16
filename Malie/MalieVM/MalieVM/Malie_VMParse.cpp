#include "stdafx.h"
#include "Malie_VMParse.h"


CMalie_VMParse::CMalie_VMParse(MalieExec *p)
	:fdiasm(fopen("diasm.txt","w"))
{
	VM_DATA = p->GetVMDataBase();
	pMalieExec = p;
}


CMalie_VMParse::~CMalie_VMParse(void)
{
	fclose(fdiasm);
}

void CMalie_VMParse::diasm(DWORD offset,DWORD maxSize)
{
	pCurrent = pMalieExec->GetVMCodeBase()+offset;
	fprintf(fdiasm,"diasm offset %X\n",offset);
	for (DWORD i=0;i<maxSize;)
	{
		fprintf(fdiasm,"%06X  ",offset);
		size_t szCode = VMParse(pCurrent);
		i+=szCode;
		pCurrent+=szCode;
		offset+=szCode;
	}
}

size_t CMalie_VMParse::ParseJmpTable(vector<DWORD> &jmpTable)
{
	fprintf(stderr,"\n\nScenario jump table start:\n");
	unsigned char * pJmpTableStart = pCurrent;
	int MALIE_END = pMalieExec->GetFuncId(L"MALIE_END");
	for (;*pCurrent!=0x33;)
	{
		size_t szCode = VMParse(pCurrent);
		if (*pCurrent == 0x2D)
		{
			if (pParma==MALIE_END)
			{
				break;
			}
		}
		else if (*pCurrent == 0)
		{
			fprintf(stderr,"%8X\n",pParma);
			jmpTable.push_back(pParma);
		}
		pCurrent+=szCode;
	}
	fprintf(stderr,"Scenrio jump table end.\n\n\n");
	return pCurrent-pJmpTableStart;
}

vector<Malie_Moji> CMalie_VMParse::ParseScenario(vector<wstring> &chapterName,vector<DWORD> &chapterIndex)
{
	vector<Malie_Moji> v;
	DWORD offset = pMalieExec->GetFuncOffset(L"maliescenario");
	pCurrent = pMalieExec->GetVMCodeBase()+offset;
	const int _ms_message = pMalieExec->GetFuncId(L"_ms_message");
	const int MALIE_NAME = pMalieExec->GetFuncId(L"MALIE_NAME");
	const int MALIE_LABLE = pMalieExec->GetFuncId(L"MALIE_LABLE");
	const int tag = pMalieExec->GetFuncId(L"tag");
	const int FrameLayer_SendMessage = pMalieExec->GetFuncId(L"FrameLayer_SendMessage");
	const int System_GetResult = pMalieExec->GetFuncId(L"System_GetResult");
	Malie_Moji moji;
	vector<DWORD> jmpTable;
	vector<DWORD>::iterator it;
	vector<DWORD> selectTable;
	fprintf(stderr,"Start parsing scenario p-code...\n\n");
	for (;*pCurrent!=0x33;)
	{
		fprintf(fdiasm,"%06X  ",offset);
		size_t szCode = VMParse(pCurrent);
		if (jmpTable.size())
		{
			if (it!=jmpTable.end()&&offset>*it)
			{
				it++;
				chapterIndex.push_back(v.size());
			}
		}
		if (*pCurrent == 0x2D||*pCurrent==0x4||*pCurrent==3)//vCall
		{
			if (pParma==tag)
			{
				WCHAR Title[0x100];
				wchar_t * pTag = wcsstr(pLastString,L"<chapter");
				if (pTag)
				{
					swscanf(pLastString,L"<chapter name='%ls'>",Title);
					pTag = wcschr(Title,L'\'');
					if (pTag)
					{
						*pTag=0;
						fwprintf(stderr,L"Title found: %ls\r\n",Title);
						chapterName.push_back(Title);
					}
					chapterIndex.push_back(v.size());
//					fprintf(stdout,"%ls\n",Title);
				}
			}
			else if (pParma==_ms_message)
			{
				vmStack.pop();
				moji.index = vmStack.top()&~0x80000000;
				while(!vmStack.empty()) vmStack.pop();
				vmStack.push(0);
				v.push_back(moji);
				moji.name = L"";
				selectTable.clear();
			}
			else if (pParma==MALIE_NAME)
			{
				moji.name = pLastString;
			}
			else if (pParma==MALIE_LABLE&&!v.size())
			{
				if (!wcscmp(pLastString,L"_link"))
				{
					szCode+=ParseJmpTable(jmpTable);
					it = jmpTable.begin();
					offset+=szCode;
					continue;
				}
			}
			else if (pParma == System_GetResult)
			{
				for each (DWORD x in selectTable)
				{
					fprintf(stderr, "Select %ls\n", x);
				}
			}
			else if (pParma == FrameLayer_SendMessage && vmStack.size()>4)
			{
				vmStack.pop(); vmStack.pop(); vmStack.pop(); vmStack.pop();
				int loc = vmStack.top();
				if (loc>0)
				{
					selectTable.push_back(loc);
				}
			}
		}
		pCurrent+=szCode;
		offset+=szCode;
	}
	fprintf(stderr,"Done.\n\n");
	fflush(fdiasm);
	return move(v);
}

size_t CMalie_VMParse::VMParse(unsigned char *pCode)
{
	size_t szCode = 1;
	DWORD temp;wchar_t *mask;
	switch (*pCode++)
	{
	case 0x0://vJmp len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
		fprintf(fdiasm,"jmp 0x%X\n",pParma);
		break;
	case 0x1://vJnz len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
// 		temp = vmStack.top();
// 		vmStack.pop();
		fprintf(fdiasm,"jnz 0x%X\n",pParma);
		break;
	case 0x2://vJz len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
// 		temp = vmStack.top();
// 		vmStack.pop();
		fprintf(fdiasm,"jz 0x%X\n",pParma);
		break;
	case 0x3://GetProcAddress len:4+1=5
		pParma= *(PDWORD)pCode;
		pCode+=4;
		temp=*pCode;
		szCode+=5;
		fprintf(fdiasm,"call %ls %d\n",pMalieExec->GetFuncName(pParma).c_str(),temp);
		break;
	case 0x4://GetProcAddress len:1+1=2
		pParma= *pCode;
		++pCode;
		temp=*pCode;
		szCode+=2;
		fprintf(fdiasm,"call %ls %d\n",pMalieExec->GetFuncName(pParma).c_str(),temp);
		break;
	case 0x5://去掉最低位、 len:指令位移&0xEF
		fprintf(fdiasm,"mask vEip\n");
		break;
	case 0x6://vPush R32 len:0
		fprintf(fdiasm,"push R32\n");
		break;
	case 0x7://vPop R32 len:0
		vmStack.pop();
		fprintf(fdiasm,"pop R32\n");
		break;
	case 0x8://vPush len:4
	case 0xD://vPush len:4
		pParma = *(PDWORD)pCode;
		vmStack.push(pParma | 0x80000000);
		szCode+=4;
		fprintf(fdiasm,"push 0x%X\n",pParma);
		break;
	case 0x9://vPushStr len:1
		pParma = *pCode;
		++szCode;
		pLastString = (wchar_t *)(VM_DATA+pParma);
		mask = (wchar_t *)wcschr((const wchar_t *)pLastString,L'\n');
		if (mask)
		{
			*mask=0;
		}
		vmStack.push(int(pParma + VM_DATA));
		fprintf(fdiasm, "push \"%ls\";#0x%06X\n", vmStack.top(), vmStack.top());
		break;
	case 0xA://vPushStr len:2
		pParma = *(PWORD)pCode;
		szCode+=2;
		pLastString = (wchar_t *)(VM_DATA+pParma);
		mask = (wchar_t *)wcschr((const wchar_t *)pLastString,L'\n');
		if (mask)
		{
			*mask=0;
		}
		vmStack.push(int(pParma + VM_DATA));
		fprintf(fdiasm, "push \"%ls\";#0x%06X\n", vmStack.top(), vmStack.top());
		break;
	case 0xB://none len:0
		break;
	case 0xC://vPushStr len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
		pLastString = (wchar_t *)(VM_DATA+pParma);
		mask = (wchar_t *)wcschr((const wchar_t *)pLastString,L'\n');
		if (mask)
		{
			*mask=0;
		}
		vmStack.push(int(pParma + VM_DATA));
		fprintf(fdiasm, "push \"%ls\";#0x%06X\n", vmStack.top(), vmStack.top());
		break;
	case 0xE://vPop len:0
		vmStack.pop();
		fprintf(fdiasm,"pop\n");
		break;
	case 0xF://vPush 0 len:0
		vmStack.push(0 | 0x80000000);
		fprintf(fdiasm,"push 0\n");
		break;
	case 0x10://无对应指令 len:0
		break;
	case 0x11://vPush len:1
		pParma = *pCode;
		vmStack.push(pParma | 0x80000000);
		++szCode;
		fprintf(fdiasm,"push 0x%X\n",pParma);
		break;
	case 0x12://vPush [sp] len:0
		fprintf(fdiasm,"push [sp]\n");
		break;
	case 0x13://vNeg len:0
		fprintf(fdiasm, "neg\n");
		break;
	case 0x14://vAdd len:0
		vmStack.pop();
		fprintf(fdiasm, "add\n");
		break;
	case 0x15://vSub len:0
		vmStack.pop();
		fprintf(fdiasm,"sub\n");
		break;
	case 0x16://vMul len:0
		vmStack.pop();
		fprintf(fdiasm,"mul\n");
		break;
	case 0x17://vDiv len:0
		vmStack.pop();
		fprintf(fdiasm,"div\n");
		break;
	case 0x18://vMod len:0
		vmStack.pop();
		fprintf(fdiasm,"mod\n");
		break;
	case 0x19://vAnd len:0
		vmStack.pop();
		fprintf(fdiasm,"and\n");
		break;
	case 0x1A://vOr len:0
		vmStack.pop();
		fprintf(fdiasm,"or\n");
		break;
	case 0x1B://vXor len:0
		vmStack.pop();
		fprintf(fdiasm,"xor\n");
		break;
	case 0x1C://vNot len:0
		fprintf(fdiasm,"not\n");
		break;
	case 0x1D://vBOOL(param) len:0
		fprintf(fdiasm,"BOOL(param)\n");
		break;
	case 0x1E://vBOOL(param1&&param2) len:0
		vmStack.pop();
		fprintf(fdiasm,"BOOL(param1&&param2)\n");
		break;
	case 0x1F://vBOOL(param1||param2)
		vmStack.pop();
		fprintf(fdiasm,"BOOL(param1||param2)\n");
		break;
	case 0x20://!vBOOL(param) len:0
		fprintf(fdiasm,"!BOOL(param)\n");
		break;
	case 0x21://vIsL len:0
		vmStack.pop();
		fprintf(fdiasm,"IsL\n");
		break;
	case 0x22://vIsLE len:0
		vmStack.pop();
		fprintf(fdiasm, "IsLE\n");
		break;
	case 0x23://vIsNLE len:0
		vmStack.pop();
		fprintf(fdiasm, "IsNLE\n");
		break;
	case 0x24://vIsNL len:0
		vmStack.pop();
		fprintf(fdiasm, "IsNL\n");
		break;
	case 0x25://vIsEQ len:0
		vmStack.pop();
		fprintf(fdiasm, "IsEQ\n");
		break;
	case 0x26://vIsNEQ len:0
		vmStack.pop();
		fprintf(fdiasm, "IsNEQ\n");
		break;
	case 0x27://vShl len:0
		vmStack.top();
		fprintf(fdiasm,"shl\n");
		break;
	case 0x28://vSar len:0
		vmStack.pop();
		fprintf(fdiasm,"sar\n");
		break;
	case 0x29://vInc len:0
		fprintf(fdiasm,"inc\n");
		break;
	case 0x2A://vDec len:0
		fprintf(fdiasm,"dec\n");
		break;
	case 0x2B://vAddReg len:0
		fprintf(fdiasm,"AddReg\n");
		break;
	case 0x2C://Debug len:0
		fprintf(fdiasm,"Debug\n");
		break;
	case 0x2D://vCall len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
		fprintf(fdiasm,"call %ls\n",pMalieExec->GetFuncName(pParma).c_str());
		vmStack.push(0 | 0x80000000);//ret val
		break;
	case 0x2E://vAdd len:0
		fprintf(fdiasm,"add\n");
		break;
	case 0x2F://vFPCopy len:0
		fprintf(fdiasm,"FPCopy\n");
	case 0x30://vFPGet len:0
		fprintf(fdiasm,"FPGet\n");
		break;
	case 0x31://vPush N len:4
		pParma = *(PDWORD)pCode;
		szCode+=4;
		fprintf(fdiasm,"initStack %d\n",pParma);
		break;
	case 0x32://vJmp len:1
		pParma = *pCode;
		++szCode;
		fprintf(fdiasm,"jmp short %X\n",pParma);
		break;
	case 0x33://vJmp len:1
		pParma = *pCode;
		++szCode;
		fprintf(fdiasm,"ret %X\n",pParma);
		break;
	default:
		fprintf(fdiasm,"Unknow opcode %X\n",*(pCode-1));
		break;
	}
	return szCode;
}