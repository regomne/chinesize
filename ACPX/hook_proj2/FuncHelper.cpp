#include "FuncHelper.h"
#include <windows.h>
#include <stdio.h>
#include "ilhook.h"

__declspec(naked) uint32_t ThiscallFunction0(void* addr, void* this_p)
{
    __asm
    {
        mov ecx, [esp + 8];
        mov eax, [esp + 4];
        call eax;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction1(void* addr, void* thisp, uint32_t arg1)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push [ebp + 0x10];
        mov ecx, [ebp + 0xc];
        call [ebp + 0x8];
        pop ebp;
        ret;
    }
}
__declspec(naked) uint32_t ThiscallFunction2(void* addr, void* thisp, uint32_t arg1, uint32_t arg2)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push [ebp + 0x14];
		push [ebp + 0x10];
		mov ecx, [ebp + 0xc];
		call [ebp + 0x8];
		pop ebp;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction3(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push [ebp + 0x18];
        push [ebp + 0x14];
		push [ebp + 0x10];
		mov ecx, [ebp + 0xc];
		call [ebp + 0x8];
        pop ebp;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction4(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push [ebp + 0x1c];
        push [ebp + 0x18];
        push [ebp + 0x14];
		push [ebp + 0x10];
		mov ecx, [ebp + 0xc];
		call [ebp + 0x8];
        pop ebp;
        ret;
    }
}

void Log(const wchar_t* format, ...)
{
    wchar_t buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vswprintf_s(buffer, format, ap);
    FILE* fp = nullptr;
    fopen_s(&fp, "log.log", "ab+");
    fwrite(buffer, 1, char_cnt * 2, fp);
    fwrite(L"\r\n", 1, 4, fp);
    fclose(fp);
    va_end(ap);
}

void Log(const char* format, ...)
{
    char buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vsprintf_s(buffer, format, ap);
    FILE* fp = nullptr;
    fopen_s(&fp, "log.log", "ab+");
    fwrite(buffer, 1, char_cnt, fp);
    fwrite("\r\n", 1, 2, fp);
    fclose(fp);
    va_end(ap);
}

bool PatchMemory(PatchStruct* psts, uint32_t cnt)
{
    for (size_t i = 0;i < cnt;i++)
    {
        PatchStruct& st = psts[i];
        auto mod = GetModuleHandleA(st.mod_name);
        if (!mod)
        {
            mod = LoadLibraryA(st.mod_name);
            if (!mod)
            {
                LOGERROR("Patch: Can't find module: %s", st.mod_name);
                return false;
            }
        }
        auto patch_addr = (uint8_t*)mod + st.offset;

        if (st.pattern != nullptr)
        {
            if (memcmp(patch_addr, st.pattern, st.len) != 0)
            {
                return false;
            }
        }
        DWORD oldProt;
        if (!VirtualProtect(patch_addr, st.len, PAGE_EXECUTE_READWRITE, &oldProt))
        {
            return false;
        }
        memcpy(patch_addr, st.hex, st.len);
    }
    return true;
}


bool HookFunctions(const HookPointStruct* hooks, uint32_t cnt)
{
    auto buff = (uint8_t*)VirtualAlloc(0, cnt * 100, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!buff)
    {
        LOGERROR("Hook: no memory!");
        return false;
    }

    HookSrcObject src;
    HookStubObject stub;

    for (size_t i = 0;i < cnt;i++)
    {
        auto hook = &hooks[i];
        auto mod = GetModuleHandleA(hook->module_name);
        if (!mod)
        {
            mod = LoadLibraryA(hook->module_name);
            if (!mod)
            {
                LOGERROR("Hook: Can't find module: %s", hook->module_name);
                return false;
            }
        }
        auto addr = (uint8_t*)mod + hook->offset;
        auto opt_data = hook->options&STUB_JMP_ADDR_AFTER_RETURN ?
            hook->dest_rva + (ptrdiff_t)mod :
            hook->ret_value;
        if (!InitializeHookSrcObject(&src, addr, true) ||
            !InitializeStubObject(&stub, buff + i * 100, 100, opt_data, hook->options) ||
            !Hook32(&src, 0, &stub, hook->hook_routine, hook->reg_tags))
        {
            LOGERROR("Hook: Can't hook module: %s, offset: 0x%x", hook->module_name, hook->offset);
            return false;
        }
    }
    return true;
}

bool HookFunctions(const HookPointStructWithName* hooks, uint32_t cnt)
{
    auto buff = (uint8_t*)VirtualAlloc(0, cnt * 100, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!buff)
    {
        LOGERROR("Hook: no memory!");
        return false;
    }

    HookSrcObject src;
    HookStubObject stub;

    for (size_t i = 0;i < cnt;i++)
    {
        auto hook = &hooks[i];
        auto mod = GetModuleHandleA(hook->module_name);
        if (!mod)
        {
            mod = LoadLibraryA(hook->module_name);
            if (!mod)
            {
                LOGERROR("Hook: Can't find module: %s", hook->module_name);
                return false;
            }
        }
        auto addr = GetProcAddress(mod, hook->proc_name);
        if (!addr)
        {
            LOGERROR("Hook: Can't find %s in module: %s", hook->proc_name, hook->module_name);
            return false;
        }
        auto opt_data = hook->options&STUB_JMP_ADDR_AFTER_RETURN ?
            hook->dest_rva + (ptrdiff_t)mod :
            hook->ret_value;
        if (!InitializeHookSrcObject(&src, (void*)addr, true) ||
            !InitializeStubObject(&stub, buff + i * 100, 100, opt_data, hook->options) ||
            !Hook32(&src, 0, &stub, hook->hook_routine, hook->reg_tags))
        {
            LOGERROR("Hook: Can't hook module: %s, name: %s", hook->module_name, hook->proc_name);
            return false;
        }
    }
    return true;
}

std::wstring decode_string(const char* s, int cp)
{
    auto len = MultiByteToWideChar(cp, 0, s, -1, 0, 0);
    auto buff = new wchar_t[len];
    len = MultiByteToWideChar(cp, 0, s, -1, buff, len);
    std::wstring str(buff);
    delete[] buff;
    return str;
}

std::wstring decode_string(const char* s, uint32_t slen, int cp)
{
    auto len = MultiByteToWideChar(cp, 0, s, slen, 0, 0);
    auto buff = new wchar_t[len];
    len = MultiByteToWideChar(cp, 0, s, slen, buff, len);
    std::wstring str(buff, len);
    delete[] buff;
    return str;
}

uint32_t get_ep_rva_from_module_address(void* module_start) {
    auto dos_head = (IMAGE_DOS_HEADER*)module_start;
    if (dos_head->e_magic != 'ZM') {
        return 0;
    }
    auto head = (IMAGE_NT_HEADERS*)((uint8_t*)dos_head + dos_head->e_lfanew);
    auto ep = head->OptionalHeader.AddressOfEntryPoint;
    return ep;
}

std::wstring get_module_path(HMODULE module) {
    wchar_t path[2048];
    auto name_len = GetModuleFileName(module, path, ARRAYSIZE(path));
    if (name_len == 0 || name_len == ARRAYSIZE(path)) {
        return L"";
    }
    auto p = wcsrchr(path, L'\\');
    if (!p) {
        return L"";
    }
    return std::wstring(path, p - path + 1);
}