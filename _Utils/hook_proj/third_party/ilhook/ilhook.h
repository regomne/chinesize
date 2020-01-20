#pragma once

#include <windows.h>

#define MAX_PATCH_LENGTH 0x20

#define HOOKFUNC __cdecl

enum PatchType
{
    PT_WIN32API,
    PT_CALL,
    PT_ANY,
};

enum StubOptions
{
	//directly return after newFunc called. if not set, jmp to source function.
	STUB_DIRECTLYRETURN=1,

	//set eax after newFunc called.
	STUB_OVERRIDEEAX=(1<<1),

    //jmp to dest addr when newFunc return.
    STUB_JMP_ADDR_AFTER_RETURN=(1<<2),

    //jmp to the return val of newFunc.
    STUB_JMP_EAX_AFTER_RETURN = (1 << 3),
};

struct CodePattern
{
public:
    BYTE* pattern;
    BYTE* mask;
    int length;
};

struct HookSrcObject
{
    void* addr;
    PatchType type;
    CodePattern pattern;

	struct Instruction
	{
		BYTE offset;
		BYTE length;
		WORD jmpType;
		void* destAddr;
	};
	Instruction insts[5];
	int instCount;

	//for CodePattern
	BYTE _pat[MAX_PATCH_LENGTH];
    
    HookSrcObject()
    {

    }

    explicit HookSrcObject(const HookSrcObject& that)
    {
        *this = that;
    }

    HookSrcObject& operator=(const HookSrcObject& that)
    {
        memcpy(this,&that,sizeof(HookSrcObject));
        pattern.pattern=_pat;
        return *this;
    }
};

struct HookStubObject
{
	void* addr;
	int length;
	StubOptions options;
	int retnVal;
};

struct Registers
{
    DWORD eflags;
	DWORD edi;
	DWORD esi;
	DWORD ebp;
	DWORD esp;
	DWORD ebx;
	DWORD edx;
	DWORD ecx;
	DWORD eax;
};

bool InitializeHookSrcObject(HookSrcObject* obj,void* addr,bool forceAny=false);
bool InitializePattern(CodePattern* pattern,BYTE* code,BYTE* mask,DWORD len);
bool InitializeStubObject(HookStubObject* obj,void* addr,int length,int retvVal=0,DWORD options=0);

bool CalcOriAddress(HookSrcObject* obj, void** addr);
bool IsPatternMatch(void* buff,CodePattern* pat);

bool PatchHookSrc(HookSrcObject* srcObj,void* destAddr);
bool GenerateMovedCode(HookSrcObject* srcObj,BYTE* destAddr,int* length);
bool GenerateStub(HookSrcObject* srcObj,HookStubObject* stubObj,void* newFunc,const char* funcArgs);

bool Hook32(HookSrcObject* srcObj,CodePattern* pre,HookStubObject* stubObj,void* newFunc,const char* funcArgs);
bool UnHook32(HookSrcObject* srcObj);

//in asmhelper.cpp

bool GetOpInfo(BYTE* addr,int* opLength,void** relativeDestAddr);

/*
*  seq:
*  a -- eax
*  b -- ebx
*  c -- ecx
*  d -- edx
*  w -- esp
*  x -- ebp
*  y -- esi
*  z -- edi
*  1-9 and A-M -- parameter 1 to 22
*  \x01-\x16 -- parameter ref 1 to 22
*  f -- orignal function pointer
*  r -- pointer to Registers struct
*  s -- pointer of hook source address
*/
bool GeneratePushInsts(const char* seq,BYTE* addr,int* length,DWORD** oriFuncAddr,DWORD srcAddr);
