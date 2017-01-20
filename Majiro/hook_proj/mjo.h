#pragma once
#include <stdint.h>
#include <vector>
#include <string>

#include "NakedMemory.h"
#include "ilhook.h"


class Mjo
{
private:
    struct MjoIAT
    {
        uint32_t hash;
        uint32_t offset;
    };

    struct JmpTbl
    {
        uint8_t* offset;
    };

    struct MultiJmpTbl
    {
        uint32_t tbl_cnt;
        uint8_t* offset;
    };

    struct StreamPtr
    {
        uint8_t* ptr;
        uint32_t ori_size;
    };

    struct MjoInfo
    {
        char magic[16];
        uint32_t entry_point;
        uint32_t unk;
        uint32_t iat_cnt;
        uint32_t code_size;
    };
    
    static const int OpTable[80];
    
    enum MajiroOps
    {
        OpPushStr = 0x801,
        OpLineNumber = 0x83a,
        OpCatString = 0x840,
        OpProcessString = 0x841,
        OpCtrlString = 0x842,
        OpJumpTable = 0x850,
    };

public:
    Mjo() :info_({ 0 }) {}
    
    bool read(const NakedMemory& mem);

    bool pack(const std::vector<std::string>& lines);

    NakedMemory write();

    std::vector<std::string>& get_lines()
    {
        return lines_;
    }

private:
    void xor_dec(void* block, uint32_t size);
    bool read_to_mem(const NakedMemory& mem);
    bool parse_code();

    bool pack_line(const std::string& line, uint32_t idx);
    NakedMemory gen_new_inst(const std::string& line);
    void insert_block(uint8_t* cur, uint32_t cur_size, uint8_t* newp, uint32_t new_size, uint8_t* cur_end);
    void fix_offsets(uint32_t cur_idx, uint8_t* start, int32_t dist);

private:
    MjoInfo info_;
    std::vector<MjoIAT> iat_;
    NakedMemory code_;

    std::vector<JmpTbl> jmp_tbl_;
    std::vector<MultiJmpTbl> mjmp_tbl_;

    std::vector<std::string> lines_;
    std::vector<StreamPtr> ptrs_;

    NakedMemory temp_;
};

void __stdcall TestParse(const wchar_t* mjo_name, const wchar_t* txt_name);
void __stdcall TestPack(const wchar_t* mjo_name, const wchar_t* txt_name, const wchar_t* new_mjo_name);

typedef FILE* (__cdecl * FopenRoutine)(const char* fname, const char* flag);
FILE* HOOKFUNC MyFopen(FopenRoutine old_func, const char* fname, const char* flag);
typedef int(__cdecl * FcloseRoutine)(FILE* fp);
int HOOKFUNC MyFclose(FcloseRoutine old_func, FILE* fp);
typedef int(__cdecl * FreadRoutine)(void* bf, size_t es, size_t ec, FILE* fp);
int HOOKFUNC MyFread(FreadRoutine old_func, void* bf, size_t es, size_t ec, FILE* fp);
