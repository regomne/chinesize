#pragma once

#include <stdint.h>
#include <windows.h>
#include <ilhook.h>

struct ExtFileHandleInfo
{
    uint32_t isSubFile;
    HANDLE file;
    uint32_t filePos;
    uint32_t size;
};

struct ExtFileInfo
{
    uint32_t isMem;
    void* buff;
    uint32_t unk;
    uint32_t size;
    ExtFileHandleInfo* handleInfo;
};

typedef ExtFileInfo* (__cdecl* OpenExtFileRoutine)(const char* filename);
ExtFileInfo* HOOKFUNC MyOpenFile(OpenExtFileRoutine oldFunc, const char* fileName);

int HOOKFUNC MyStrcmpi(const char* s1, const char* s2);

typedef BOOL(*TextOutARoutine)(
    HDC hdc,           // デバイスコンテキストのハンドル
    int nXStart,       // 開始位置（基準点）の x 座標
    int nYStart,       // 開始位置（基準点）の y 座標
    LPCSTR lpString,  // 文字列
    int cbString       // 文字数
    );
BOOL HOOKFUNC MyTextout(TextOutARoutine old_proc,
    HDC hdc,
    int nXStart,
    int nYStart,
    LPCSTR lpString,
    int cbString
);

