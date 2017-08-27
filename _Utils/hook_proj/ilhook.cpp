
#include <windows.h>
#include "ilhook.h"

bool GenerateStub(HookSrcObject* srcObj, HookStubObject* stubObj, void* newFunc, char* funcArgs)
{
    BYTE* oriFunc = 0;
    DWORD* newOriFuncPtr = 0;

    BYTE* pst = (BYTE*)stubObj->addr;
    BYTE* pstend = pst + stubObj->length;

    if (!CalcOriAddress(srcObj, (void**)&oriFunc))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

#define TEST_BUFF(cnt) if(pst+(cnt)>pstend) {SetLastError(ERROR_INSUFFICIENT_BUFFER);return false;}

    TEST_BUFF(10);
    *pst++ = 0x60; //pushad
    *pst++ = 0x9c; //pushfd

    if (funcArgs)
    {
        int length = pstend - pst;
        if (!GeneratePushInsts(funcArgs, pst, &length, &newOriFuncPtr, (DWORD)srcObj->addr))
            return false;
        pst += length;
    }

    //call xxxxxxxx
    *pst = 0xe8;
    *(DWORD*)(pst + 1) = (BYTE*)newFunc - (pst + 5);
    pst += 5;

    int argCnt = funcArgs ? strlen(funcArgs) : 0;
    if (argCnt != 0)
    {
        //add esp,XX
        *(WORD*)pst = 0xc483;
        *(pst + 2) = (BYTE)(argCnt * 4);
        pst += 3;
    }

    if (stubObj->options & (STUB_JMP_EAX_AFTER_RETURN | STUB_JMP_ADDR_AFTER_RETURN))
    {
        if (stubObj->options & STUB_JMP_ADDR_AFTER_RETURN)
        {
            TEST_BUFF(7);
            *pst++ = 0x9d; //popfd
            *pst++ = 0x61; //popad
            *pst++ = 0xe9; //jmp oriAddr
            *(DWORD*)pst = stubObj->retnVal - (int)(pst + 4);
            pst += 4;
        }
        else if (stubObj->options & STUB_JMP_EAX_AFTER_RETURN)
        {
            TEST_BUFF(10);
            *(DWORD*)pst = 0xfc244489; //mov [esp-4], eax
            pst += 4;
            *pst++ = 0x9d; //popfd
            *pst++ = 0x61; //popad
            *(DWORD*)pst = 0xd82464ff; //jmp dword ptr [esp-0x28]
            pst += 4;
        }
    }
    else
    {
        if (stubObj->options & STUB_OVERRIDEEAX)
        {
            TEST_BUFF(4);
            *(DWORD*)pst = 0x20244489; //mov [esp+20h],eax
            pst += 4;
        }
        TEST_BUFF(2);
        *pst++ = 0x9d; //popfd
        *pst++ = 0x61; //popad

        if (stubObj->options & STUB_DIRECTLYRETURN)
        {
            TEST_BUFF(3);
            if (stubObj->retnVal == 0)
                *pst++ = 0xc3; //ret
            else
            {
                *pst = 0xc2; //retn XX
                *(WORD*)(pst + 1) = (WORD)stubObj->retnVal;
                pst += 3;
            }
        }
    }

    if (!(stubObj->options & (STUB_DIRECTLYRETURN | STUB_JMP_ADDR_AFTER_RETURN | STUB_JMP_EAX_AFTER_RETURN))
        || newOriFuncPtr)
    {
        if (newOriFuncPtr)
            *newOriFuncPtr = (DWORD)pst;

        //write the stolen code
        if (srcObj->type == PT_ANY)
        {
            int len = pstend - pst;
            if (!GenerateMovedCode(srcObj, pst, &len))
                return false;
            pst += len;
        }

        TEST_BUFF(5);
        *pst = 0xe9; //jmp oriAddr
        *(DWORD*)(pst + 1) = oriFunc - (pst + 5);
        pst += 5;
    }
    return true;
}

bool GenerateMovedCode(HookSrcObject* srcObj, BYTE* destAddr, int* length)
{
    BYTE* p = destAddr;
    BYTE* pend = p + *length;

    HookSrcObject::Instruction* inst = srcObj->insts;
    for (int i = 0; i < srcObj->instCount; i++)
    {
        if (inst[i].jmpType)
        {
            if (p + 6 > pend)
            {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return false;
            }
            BYTE tp1 = inst[i].jmpType & 0xff;
            if (tp1 == 0xeb || tp1 == 0xe9)
                *p++ = 0xe9;
            else if (tp1 == 0xe8)
                *p++ = 0xe8;
            else if ((tp1 & 0xf0) == 0x70)
            {
                *p++ = 0xf;
                *p++ = 0x80 | (inst[i].jmpType & 0xf);
            }
            else
            {
                *p++ = tp1;
                *p++ = (inst[i].jmpType >> 8);
            }
            *(DWORD*)p = (BYTE*)inst[i].destAddr - (p + 4);
            p += 4;
        }
        else
        {
            if (p + inst[i].length > pend)
            {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return false;
            }
            memcpy(p, srcObj->pattern.pattern + inst[i].offset, inst[i].length);
            p += inst[i].length;
        }
    }
    *length = p - destAddr;
    return true;
}

bool IsPatternMatch(void* buff, CodePattern* pat)
{
    BYTE* p = (BYTE*)buff;
    for (int i = 0; i < pat->length; i++)
    {
        if (!(pat->mask) || pat->mask[i])
            if (p[i] != pat->pattern[i])
                return false;
    }
    return true;
}

bool CalcOriAddress(HookSrcObject* obj, void** addr)
{
    switch (obj->type)
    {
    case PT_CALL:
        *(BYTE**)addr = (BYTE*)obj->addr + 5 + *(DWORD*)&obj->pattern.pattern[1];
        break;
    case PT_WIN32API:
        *(BYTE**)addr = (BYTE*)obj->addr + 2;
        break;
    case PT_ANY:
        *(BYTE**)addr = (BYTE*)obj->addr + obj->pattern.length;
        break;
    default:
        return false;
    }
    return true;
}

bool PatchHookSrc(HookSrcObject* srcObj, void* destAddr)
{
    switch (srcObj->type)
    {
    case PT_CALL:
        if (*(BYTE*)srcObj->addr != 0xe8)
        {
            SetLastError(ERROR_BAD_ENVIRONMENT);
            return false;
        }
        *(DWORD*)((BYTE*)srcObj->addr + 1) = (BYTE*)destAddr - ((BYTE*)srcObj->addr + 5);
        break;
    case PT_WIN32API:
        if (*(WORD*)srcObj->addr != 0xff8b)
        {
            SetLastError(ERROR_BAD_ENVIRONMENT);
            return false;
        }
        *(WORD*)srcObj->addr = 0xf9eb; //jmp $-5
        *((BYTE*)srcObj->addr - 5) = 0xe9; //jmp
        *(DWORD*)((BYTE*)srcObj->addr - 4) = (BYTE*)destAddr - (BYTE*)srcObj->addr;
        break;
    case PT_ANY:
        *(BYTE*)srcObj->addr = 0xe9; //jmp
        *(DWORD*)((BYTE*)srcObj->addr + 1) = (BYTE*)destAddr - ((BYTE*)srcObj->addr + 5);
        break;
    }
    return true;
}

bool InitializeHookSrcObject(HookSrcObject* obj, void* addr, bool forceAny/* =false */)
{
    PatchType pt = PT_ANY;
    BYTE* p = (BYTE*)addr;

    if (!forceAny)
    {
        if (*p == 0xe8)
            pt = PT_CALL;
        else if (*(WORD*)p == 0xff8b && *(DWORD*)(p - 5) == 0x90909090 && p[-1] == 0x90)
            pt = PT_WIN32API;
    }

    obj->type = pt;
    obj->addr = addr;

    switch (obj->type)
    {
    case PT_CALL:
        obj->instCount = 0;
        memcpy(obj->_pat, addr, 5);
        InitializePattern(&obj->pattern, obj->_pat, 0, 5);
        break;
    case PT_WIN32API:
        obj->instCount = 0;
        memcpy(obj->_pat, addr, 2);
        InitializePattern(&obj->pattern, obj->_pat, 0, 2);
        break;
    case PT_ANY:
    {
        int curOff = 0;
        int opLen;
        void* destAddr;
        HookSrcObject::Instruction* inst = obj->insts;
        while (curOff < 5)
        {
            destAddr = 0;
            if (!GetOpInfo(p + curOff, &opLen, &destAddr))
            {
                SetLastError(ERROR_INVALID_DATA);
                return false;
            }
            inst->offset = (BYTE)curOff;
            inst->length = (BYTE)opLen;
            inst->jmpType = destAddr ? *(WORD*)&p[curOff] : 0;
            inst->destAddr = destAddr;

            inst++;
            curOff += opLen;
            if (curOff > MAX_PATCH_LENGTH)
            {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return false;
            }
        }
        obj->instCount = inst - obj->insts;
        memcpy(obj->_pat, p, curOff);
        InitializePattern(&obj->pattern, obj->_pat, 0, curOff);
    }
    break;
    }
    return true;
}

bool InitializePattern(CodePattern* pattern, BYTE* code, BYTE* mask, DWORD len)
{
    pattern->pattern = code;
    pattern->mask = mask;
    pattern->length = len;
    return true;
}

bool InitializeStubObject(HookStubObject* obj, void* addr, int length, int retval, DWORD options)
{
    obj->addr = addr;
    obj->length = length;
    obj->options = (StubOptions)options;
    obj->retnVal = retval;
    return true;
}

bool Hook32(HookSrcObject* srcObj, CodePattern* pre, HookStubObject* stubObj, void* newFunc, char* funcArgs)
{
    if (pre && !IsPatternMatch(srcObj->addr, pre))
        return false;

    if (!GenerateStub(srcObj, stubObj, newFunc, funcArgs))
        return false;

    DWORD oldProt;
    if (!VirtualProtect(srcObj->addr, 5, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;

    if (!PatchHookSrc(srcObj, stubObj->addr))
        return false;

    return true;
}

bool UnHook32(HookSrcObject* srcObj)
{
    memcpy(srcObj->addr, srcObj->pattern.pattern, srcObj->pattern.length);
    return true;
}