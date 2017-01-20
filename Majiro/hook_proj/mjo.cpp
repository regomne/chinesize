#include "mjo.h"
#include <assert.h>
#include <algorithm>
#include "FuncHelper.h"
#include "FileReader.h"
#include "TxtSupport.h"
#include "PicSupport.h"

using namespace std;

#include "crc32key.cpp"

const int Mjo::OpTable[80] = {
    4,	-1,	8,	4,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	10,
    10,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
    -3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-1,	-3,	0,	-4,	-4,	-4,	0,
    -4,	-4,	-4,	-4,	6,	6,	-1,	8,	-4,	-4,	2,	-4,	-4,	-4,	0,	0,
    -2,	0,	-1,	-4,	0,	-4,	0,	-4,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
};

void Mjo::xor_dec(void* block, uint32_t size)
{
    auto bf = (uint8_t*)block;
    for (uint32_t i = 0;i < size;i++)
    {
        bf[i] ^= g_crc32_table[i & 0x3ff];
    }
}

bool Mjo::read(const NakedMemory& mem)
{
    if (!read_to_mem(mem))
    {
        LOGERROR("header error.");
        return false;
    }
    if (!parse_code())
    {
        LOGERROR("code parse error.");
        return false;
    }
    return true;
}

bool Mjo::read_to_mem(const NakedMemory& mem)
{
    if (mem.GetSize() < sizeof(info_))
    {
        return false;
    }
    auto cur = (uint8_t*)mem.Get();
    memcpy(info_.magic, cur, 16);
    auto get_u32 = [&cur]() {
        cur += 4;
        return *(uint32_t*)(cur - 4);
    };
    cur += 16;
    info_.entry_point = get_u32();
    info_.unk = get_u32();
    info_.iat_cnt = get_u32();
    
    if (sizeof(info_) + info_.iat_cnt * 8 > mem.GetSize())
    {
        return false;
    }
    for (size_t i = 0;i < info_.iat_cnt;i++)
    {
        iat_.push_back(*(MjoIAT*)cur);
        cur += 8;
    }
    info_.code_size = get_u32();
    if (sizeof(info_) + info_.iat_cnt * 8 + info_.code_size > mem.GetSize())
    {
        return false;
    }

    code_.SetSize((info_.code_size + 300) * 2);
    memset(code_.Get(), 0, code_.GetSize());
    memcpy(code_.Get(), cur, info_.code_size);
    
    xor_dec(code_.Get(), info_.code_size);
    return true;
}

bool Mjo::parse_code()
{
    auto cur = (uint8_t*)code_.Get();
    auto get_u16 = [&cur]() {
        cur += 2;
        return *(uint16_t*)(cur - 2);
    };
    auto get_i32 = [&cur]() {
        cur += 4;
        return *(int32_t*)(cur - 4);
    };
    auto get_str = [&]() {
        auto len = get_u16();
        auto real_len = cur[len - 1] == 0 ? len - 1 : len;
        auto s = string((char*)cur, real_len);
        cur += len;
        return s;
    };
    auto get_multi_str = [&]() {
        auto s = get_str();
        //if (*(uint16_t*)cur == OpLineNumber && *(uint16_t*)(cur + 4) == OpCatString)
        //{
        //    cur += 6;
        //    s += '@' + get_str();
        //}
        while (*(cur + 6) == 'n' &&
            *(uint16_t*)cur == OpProcessString &&
            *(uint16_t*)(cur + 2) == OpCtrlString &&
            *(uint16_t*)(cur + 12) == OpCatString)
        {
            cur += 14;
            s += "\\n" + get_str();
        }
        return s;
    };
    auto set_err = [&](uint32_t op) {
        LOGERROR("error, pos:0x%x, op:0x%x", cur - (uint8_t*)code_.Get(), op);
    };

    auto end_ptr = (uint8_t*)code_.Get() + info_.code_size;
    auto last_string_ptr = cur;

    while (cur < end_ptr)
    {
        auto op = get_u16();
        if (op <= 0x1a9)
        {
            continue;
        }
        else if (op <= 0x320)
        {
            cur += 8;
        }
        else if (op == OpPushStr)
        {
            auto old = cur;
            auto s = get_str();
            if ((uint8_t)s[0] >= 0x81)
            {
                last_string_ptr = old;
                lines_.push_back(s);
                ptrs_.push_back({ old, (uint32_t)(cur - old) });
            }
        }
        else if (op >= 0x800 && op <= 0x847)
        {
            auto flag = OpTable[op - 0x800];
            if (flag >= 0)
            {
                cur += flag;
                continue;
            }
            int32_t dist = 0;
            uint8_t* old = nullptr;
            switch (flag)
            {
            case -1:
                cur += get_u16();
                break;
            case -2:
                last_string_ptr = cur;
                lines_.push_back(get_multi_str());
                ptrs_.push_back({ last_string_ptr, (uint32_t)(cur - last_string_ptr) });
                break;
            case -3:
                set_err(op);
                return false;
            case -4:
                dist = get_i32();
                if (dist > 0 || cur + dist < last_string_ptr)
                {
                    jmp_tbl_.push_back({ cur - 4 });
                }
                break;
            default:
                assert(false);
                break;
            }
        }
        else if (op == OpJumpTable)
        {
            auto cnt = get_u16();
            mjmp_tbl_.push_back({ cnt, cur });
            cur += cnt * 4;
        }
        else
        {
            set_err(op);
            return false;
        }
    }
    return true;
}

bool Mjo::pack(const std::vector<std::string>& lines)
{
    if (lines.size() != lines_.size())
    {
        LOGERROR("lines cnt error, ori:%d, new:%d", lines_.size(), lines.size());
        return false;
    }
    temp_.SetSize(code_.GetSize());
    for (size_t i = 0;i < lines.size();i++)
    {
        if (!pack_line(lines[i], i))
        {
            return false;
        }
    }
    return true;
}

bool Mjo::pack_line(const std::string& line, uint32_t idx)
{
    auto cur = ptrs_[idx].ptr;
    auto ori_size = ptrs_[idx].ori_size;
    if (*(uint16_t*)(cur - 2) == OpPushStr && line.find("\\n") != string::npos)
    {
        LOGERROR("inst PushStr don't need \\n, line: %d", idx + 1);
        return false;
    }
    auto new_insts = gen_new_inst(line);
    insert_block(cur, ori_size, (uint8_t*)new_insts.Get(), new_insts.GetSize(), (uint8_t*)code_.Get() + info_.code_size);
    ptrs_[idx].ori_size = new_insts.GetSize();
    fix_offsets(idx, cur, new_insts.GetSize() - ori_size);
    info_.code_size += (new_insts.GetSize() - ori_size);
    return true;
}

NakedMemory Mjo::gen_new_inst(const std::string& line)
{
    auto estimate_line_cnt = std::count(line.begin(), line.end(), '\\');
    NakedMemory mem(line.length() + 3 + estimate_line_cnt * 30);
    auto cur = (uint8_t*)mem.Get();
    auto wu16 = [&cur](uint16_t val) {
        *(uint16_t*)cur = val;
        cur += 2;
    };
    auto wcstr = [&](const char* s, uint32_t n) {
        memcpy(cur, s, n);
        cur[n] = 0;
        cur += n + 1;
    };
    auto wpstr = [&](const char* s, uint32_t n) {
        wu16(n + 1);
        wcstr(s, n);
    };

    auto pos = line.find("\\n");
    if (pos == string::npos)
    {
        pos = line.length();
    }
    wpstr(line.c_str(), pos);
    
    pos += 2;
    while (pos < line.length())
    {
        auto next = line.find("\\n", pos);
        if (next == string::npos)
        {
            next = line.length();
        }
        wu16(OpProcessString);
        wu16(OpCtrlString);
        wpstr("n", 1);
        wu16(OpLineNumber);
        wu16(1);
        wu16(OpCatString);
        wpstr(line.c_str() + pos, next - pos);
        pos = next + 2;
    }
    mem.SetSize(cur - (uint8_t*)mem.Get());
    return std::move(mem);
}

void Mjo::insert_block(uint8_t* cur, uint32_t cur_size, uint8_t* newp, uint32_t new_size, uint8_t* cur_end)
{
    if (cur_size >= new_size + 16)
    {
        memcpy(cur, newp, new_size);
        memcpy(cur + new_size, cur + cur_size, cur_end - (cur + cur_size));
    }
    else if (new_size == cur_size)
    {
        memcpy(cur, newp, new_size);
    }
    else
    {
        memcpy(temp_.Get(), cur + cur_size, cur_end - (cur + cur_size));
        memcpy(cur, newp, new_size);
        memcpy(cur + new_size, temp_.Get(), cur_end - (cur + cur_size));
    }
}

void Mjo::fix_offsets(uint32_t cur_idx, uint8_t* start, int32_t dist)
{
    if (dist == 0)
    {
        return;
    }

    for (auto i = cur_idx + 1;i < ptrs_.size();i++)
    {
        ptrs_[i].ptr += dist;
    }
    
    auto cur_off = (uint32_t)(start - (uint8_t*)code_.Get());
    for (auto& ia : iat_)
    {
        if (ia.offset > cur_off)
        {
            ia.offset += dist;
        }
    }

    if (info_.entry_point > cur_off)
    {
        info_.entry_point += dist;
    }

    for (auto& jmp : jmp_tbl_)
    {
        if (jmp.offset > start)
        {
            jmp.offset += dist;
        }
        auto ori_dist = *(int32_t*)jmp.offset;
        auto dest = jmp.offset + 4 + ori_dist;
        if (dest <= start && jmp.offset > start)
        {
            *(int32_t*)jmp.offset -= dist;
        }
        else if (dest > start && jmp.offset < start)
        {
            *(int32_t*)jmp.offset += dist;
        }
    }

    for (auto& mjmp : mjmp_tbl_)
    {
        if (mjmp.offset > start)
        {
            mjmp.offset += dist;
        }
        auto next_inst_ptr = mjmp.offset + mjmp.tbl_cnt * 4;
        auto offsets = (int32_t*)mjmp.offset;
        for (size_t i = 0;i < mjmp.tbl_cnt;i++)
        {
            auto dest = next_inst_ptr + offsets[i];
            if (dest <= start && mjmp.offset > start)
            {
                offsets[i] -= dist;
            }
            else if (dest > start && mjmp.offset < start)
            {
                offsets[i] += dist;
            }
        }
    }
}

NakedMemory Mjo::write()
{
    NakedMemory mem(0x20 + info_.iat_cnt * 8 + info_.code_size);
    auto p = (uint8_t*)mem.Get();
    memcpy(p, info_.magic, 16);
    p += 16;
    auto wu32 = [&p](uint32_t val) {
        *(uint32_t*)p = val;
        p += 4;
    };
    wu32(info_.entry_point);
    wu32(info_.unk);
    wu32(info_.iat_cnt);
    for (auto& ia : iat_)
    {
        wu32(ia.hash);
        wu32(ia.offset);
    }
    wu32(info_.code_size);
    memcpy(p, code_.Get(), info_.code_size);
    xor_dec(p, info_.code_size);
    return std::move(mem);
}

void __stdcall TestParse(const wchar_t* mjo_name, const wchar_t* txt_name)
{
    MyFileReader reader;
    auto mem = reader.ReadToMem(mjo_name);

    Mjo mjo;
    if (!mjo.read(mem))
    {
        LOGERROR("read faild");
        return;
    }
    auto& ls = mjo.get_lines();

    FILE* fp = nullptr;
    _wfopen_s(&fp, txt_name, L"wb");
    if (!fp)
    {
        return;
    }
    for (auto& s : ls)
    {
        fwrite(s.c_str(), 1, s.length(), fp);
        fwrite("\r\n", 1, 2, fp);
    }
    fclose(fp);
}

void __stdcall TestPack(const wchar_t* mjo_name, const wchar_t* txt_name, const wchar_t* new_mjo_name)
{
    MyFileReader reader;
    auto mem = reader.ReadToMem(mjo_name);

    Mjo mjo;
    if (!mjo.read(mem))
    {
        LOGERROR("read faild");
        return;
    }

    auto txtmem = reader.ReadToMem(txt_name);
    auto ls = SplitTxtA(txtmem);
    if (ls[ls.size() - 1] == "")
    {
        ls.pop_back();
    }

    if (!mjo.pack(ls))
    {
        LOGERROR(L"%s pack fail, txt: %s", mjo_name, txt_name);
        return;
    }
    
    auto nm = mjo.write();

    FILE* fp;
    _wfopen_s(&fp, new_mjo_name, L"wb");
    if (!fp)
    {
        return;
    }
    fwrite(nm.Get(), 1, nm.GetSize(), fp);
    fclose(fp);
}

// typedef uint32_t(*__cdecl ReadMjoRoutine)(const char* mjo_name);
// uint32_t HOOKFUNC MyReadMjo(ReadMjoRoutine ss, const char* mjo_name)
// {
//     auto fname = L"txt\\" + decode_string(mjo_name, 932) + L".txt";
//     auto attr = GetFileAttributes(fname.c_str());
//     if (attr != INVALID_FILE_ATTRIBUTES && !(attr&FILE_ATTRIBUTE_DIRECTORY))
//     {
// 
//     }
// }

FILE* g_cur_mjo_fp = nullptr;
uint32_t g_cur_mjo_pos = 0;
NakedMemory* g_cur_new_mjo = nullptr;

FILE* HOOKFUNC MyFopen(FopenRoutine old_func, const char* fname, const char* flag)
{
    auto pos = strrchr(fname, '\\');
    if (pos == nullptr)
    {
        return old_func(fname, flag);
    }
    auto path = decode_string(fname, pos + 1 - fname, CP_ACP);
    auto mjo_name = decode_string(pos + 1, 932);
    auto mjo_full_name = path + mjo_name;
    //FILE* fp = nullptr;
    //_wfopen_s(&fp, mjo_full_name.c_str(), decode_string(flag, CP_ACP).c_str());
    //if (!fp)
    //{
    //    return old_func(fname, flag);
    //}
    FILE* fp = old_func(fname, flag);
    if (!fp)
    {
        return fp;
    }

    transform(mjo_name.begin(), mjo_name.end(), mjo_name.begin(), tolower);
    if (mjo_name.find(L".mjo") == mjo_name.length() - 4)
    {
        wchar_t cur_path[1000];
        GetCurrentDirectory(ARRAYSIZE(cur_path), cur_path);
        auto txt_name = wstring(cur_path) + L"\\txt\\" + mjo_name.substr(0, mjo_name.length() - 4) + L".txt";
        MyFileReader reader;
        auto txt_mem = reader.ReadToMem(txt_name.c_str());
        if (!txt_mem.Get())
        {
            return fp;
        }
        auto ansi_txt = CvtToAnsi(txt_mem);
        auto ls = SplitTxtA(ansi_txt);
        if (ls[ls.size() - 1] == "")
        {
            ls.pop_back();
        }

        auto mjo_mem = reader.ReadToMem(fname);
        if (!mjo_mem.Get())
        {
            return fp;
        }
        Mjo mjo_file;
        if (!mjo_file.read(mjo_mem) ||
            !mjo_file.pack(ls))
        {
            LOGERROR(L"read or pack mjo fail: %s", mjo_name.c_str());
            return fp;
        }
        delete g_cur_new_mjo;
        g_cur_new_mjo = new NakedMemory(std::move(mjo_file.write()));
        g_cur_mjo_pos = 0;
        g_cur_mjo_fp = fp;
    }
    return fp;
}

int HOOKFUNC MyFclose(FcloseRoutine old_func, FILE* fp)
{
    if (fp == g_cur_mjo_fp)
    {
        g_cur_mjo_fp = nullptr;
        delete g_cur_new_mjo;
        g_cur_new_mjo = nullptr;
    }
    return old_func(fp);
}

int HOOKFUNC MyFread(FreadRoutine old_func, void* bf, size_t es, size_t ec, FILE* fp)
{
    if (g_cur_mjo_fp == fp && g_cur_new_mjo)
    {
        auto read_size = es * ec;
        if (read_size > g_cur_new_mjo->GetSize() - g_cur_mjo_pos)
        {
            read_size = g_cur_new_mjo->GetSize() - g_cur_mjo_pos;
        }
        memcpy(bf, (uint8_t*)g_cur_new_mjo->Get() + g_cur_mjo_pos, read_size);
        g_cur_mjo_pos += read_size;
        return ec;
    }
    else
    {
        return old_func(bf, es, ec, fp);
    }
}


#include "rct_key.cpp"

struct RctHeader {
    uint8_t magic[8];		/* "鞠間TC00" and "鞠間TC01" and "鞠間TS00" and "鞠間TS01" */
    uint32_t width;
    uint32_t height;
    uint32_t data_length;
};

using namespace std;

void xor_dec(void* buff, uint32_t size)
{
    for (size_t i = 0;i < size;i++)
    {
        ((uint8_t*)buff)[i] ^= g_rct_key[i & 0x3ff];
    }
}

static DWORD rct_decompress(BYTE *uncompr, DWORD uncomprLen,
    BYTE *compr, DWORD comprLen, DWORD width)
{
    DWORD act_uncomprLen = 0;
    DWORD curByte = 0;
    DWORD pos[32];

    pos[0] = -3;
    pos[1] = -6;
    pos[2] = -9;
    pos[3] = -12;
    pos[4] = -15;
    pos[5] = -18;
    pos[6] = (3 - width) * 3;
    pos[7] = (2 - width) * 3;
    pos[8] = (1 - width) * 3;
    pos[9] = (0 - width) * 3;
    pos[10] = (-1 - width) * 3;
    pos[11] = (-2 - width) * 3;
    pos[12] = (-3 - width) * 3;
    pos[13] = 9 - ((width * 3) << 1);
    pos[14] = 6 - ((width * 3) << 1);
    pos[15] = 3 - ((width * 3) << 1);
    pos[16] = 0 - ((width * 3) << 1);
    pos[17] = -3 - ((width * 3) << 1);
    pos[18] = -6 - ((width * 3) << 1);
    pos[19] = -9 - ((width * 3) << 1);
    pos[20] = 9 - width * 9;
    pos[21] = 6 - width * 9;
    pos[22] = 3 - width * 9;
    pos[23] = 0 - width * 9;
    pos[24] = -3 - width * 9;
    pos[25] = -6 - width * 9;
    pos[26] = -9 - width * 9;
    pos[27] = 6 - ((width * 3) << 2);
    pos[28] = 3 - ((width * 3) << 2);
    pos[29] = 0 - ((width * 3) << 2);
    pos[30] = -3 - ((width * 3) << 2);
    pos[31] = -6 - ((width * 3) << 2);

    uncompr[act_uncomprLen++] = compr[curByte++];
    uncompr[act_uncomprLen++] = compr[curByte++];
    uncompr[act_uncomprLen++] = compr[curByte++];

    while (1) {
        BYTE flag;
        DWORD copy_bytes, copy_pos;

        if (curByte >= comprLen)
            break;

        flag = compr[curByte++];

        if (!(flag & 0x80)) {
            if (flag != 0x7f)
                copy_bytes = flag * 3 + 3;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0x80;
                copy_bytes *= 3;
            }

            if (curByte + copy_bytes - 1 >= comprLen)
                break;
            if (act_uncomprLen + copy_bytes - 1 >= uncomprLen)
                break;

            memcpy(&uncompr[act_uncomprLen], &compr[curByte], copy_bytes);
            act_uncomprLen += copy_bytes;
            curByte += copy_bytes;
        }
        else {
            copy_bytes = flag & 3;
            copy_pos = (flag >> 2) & 0x1f;

            if (copy_bytes != 3) {
                copy_bytes = copy_bytes * 3 + 3;
            }
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 4;
                copy_bytes *= 3;
            }

            for (unsigned int i = 0; i < copy_bytes; i++) {
                if (act_uncomprLen >= uncomprLen)
                    goto out;
                uncompr[act_uncomprLen] = uncompr[act_uncomprLen + pos[copy_pos]];
                act_uncomprLen++;
            }
        }
    }
out:
    //	if (curByte != comprLen)
    //		fprintf(stderr, "compr miss-match %d VS %d\n", curByte, comprLen);

    return act_uncomprLen;
}

static DWORD rc8_decompress(BYTE *uncompr, DWORD uncomprLen,
    BYTE *compr, DWORD comprLen, DWORD width)
{
    DWORD act_uncomprLen = 0;
    DWORD curByte = 0;
    DWORD pos[16];

    pos[0] = -1;
    pos[1] = -2;
    pos[2] = -3;
    pos[3] = -4;
    pos[4] = 3 - width;
    pos[5] = 2 - width;
    pos[6] = 1 - width;
    pos[7] = 0 - width;
    pos[8] = -1 - width;
    pos[9] = -2 - width;
    pos[10] = -3 - width;
    pos[11] = 2 - (width * 2);
    pos[12] = 1 - (width * 2);
    pos[13] = (0 - width) << 1;
    pos[14] = -1 - (width * 2);
    pos[15] = (-1 - width) * 2;

    uncompr[act_uncomprLen++] = compr[curByte++];

    while (1) {
        BYTE flag;
        DWORD copy_bytes, copy_pos;

        if (curByte >= comprLen)
            break;

        flag = compr[curByte++];

        if (!(flag & 0x80)) {
            if (flag != 0x7f)
                copy_bytes = flag + 1;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0x80;
            }

            if (curByte + copy_bytes - 1 >= comprLen)
                break;
            if (act_uncomprLen + copy_bytes - 1 >= uncomprLen)
                break;

            memcpy(&uncompr[act_uncomprLen], &compr[curByte], copy_bytes);
            act_uncomprLen += copy_bytes;
            curByte += copy_bytes;
        }
        else {
            copy_bytes = flag & 7;
            copy_pos = (flag >> 3) & 0xf;

            if (copy_bytes != 7)
                copy_bytes += 3;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0xa;
            }

            for (unsigned int i = 0; i < copy_bytes; i++) {
                if (act_uncomprLen >= uncomprLen)
                    break;
                uncompr[act_uncomprLen] = uncompr[act_uncomprLen + pos[copy_pos]];
                act_uncomprLen++;
            }
        }
    }

    //	if (curByte != comprLen)
    //		fprintf(stderr, "compr miss-match %d VS %d\n", curByte, comprLen);

    return act_uncomprLen;
}

bool un_ts00(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);

    if (hdr->data_length + sizeof(*hdr) != mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    xor_dec(cur, hdr->data_length);
    auto unc_len = hdr->width * hdr->height * 3;
    NakedMemory newbuff(unc_len);
    auto act_len = rct_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 24, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 24, NakedMemory(), ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;
}

bool un_ts01(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);
    cur += *(uint16_t*)cur + 2;

    if (hdr->data_length + cur != (uint8_t*)mem.Get() + mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    xor_dec(cur, hdr->data_length);
    auto unc_len = hdr->width * hdr->height * 3;
    NakedMemory newbuff(unc_len);
    auto act_len = rct_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 24, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 24, NakedMemory(), ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;
}

bool un_rc8(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);

    if (hdr->data_length + sizeof(*hdr) + 0x300 != mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    auto pal_ptr = cur;
    cur += 0x300;
    //xor_dec(cur, hdr->data_length);
    auto unc_len = hdr->width * hdr->height;
    NakedMemory newbuff(unc_len);
    auto act_len = rc8_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }

    NakedMemory pallete(0x400);
    auto p = (uint8_t*)pallete.Get();
    for (int i = 0;i < 0x100;i++)
    {
        p[0] = pal_ptr[0];
        p[1] = pal_ptr[1];
        p[2] = pal_ptr[2];
        p[3] = 0;
        p += 4;
        pal_ptr += 3;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 8, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 8, pallete, ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;

}

void un_pic(const wchar_t* rctname, const wchar_t* bmpname)
{
    MyFileReader reader;
    auto mem = reader.ReadToMem(rctname);
    auto hdr = (RctHeader*)mem.Get();
    bool ret;
    if (memcmp(hdr->magic, "\x98\x5a\x92\x9a\x38_00", 8) == 0)
    {
        ret = un_rc8(hdr, mem, bmpname);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTS00", 8) == 0)
    {
        ret = un_ts00(hdr, mem, bmpname);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTS01", 8) == 0)
    {
        ret = un_ts01(hdr, mem, bmpname);
    }

    if (!ret)
    {
        wprintf(L"Error, fname: %s\n", rctname);
    }
}
