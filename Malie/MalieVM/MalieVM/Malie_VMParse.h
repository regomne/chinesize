#pragma once
#include "MalieExec.h"
#include <windows.h>
#include <stack>
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef struct _Malie_Moji
{
	wstring name;
	DWORD index;
} Malie_Moji,*PMalie_Moji;

class CMalie_VMParse
{
public:
	CMalie_VMParse(MalieExec *p);
	~CMalie_VMParse(void);
	void diasm(DWORD offset,DWORD maxSize);
	size_t VMParse(unsigned char *pCode);
	DWORD ToNextFunction(int funcId);
	vector<Malie_Moji> ParseScenario(vector<wstring> &chapterName,vector<DWORD> &chapterIndex);
	size_t ParseJmpTable(vector<DWORD> &jmpTable);//return size of jump table
    map<int, wstring>& get_name_data_table() {
        return name_table_;
    }
    map<int, wstring>& get_sel_data_table() {
        return sel_table_;
    }
// 	DWORD GetStackTop()
// 	{
// 		return vmStack.top();
// 	}
private:
	unsigned char *pCurrent;
	int codeLen;
	DWORD pParma;
	wchar_t *pLastString;
	stack<DWORD> vmStack;
	unsigned char *VM_DATA;
	MalieExec *pMalieExec;
	FILE *fdiasm;

    map<int, wstring> name_table_;
    map<int, wstring> sel_table_;
};

