#include "malie.h"
#include <sstream>
#include <map>

#include "FileReader.h"
#include "TxtSupport.h"
#include "FuncHelper.h"

using namespace std;

bool MalieString::init() {
    State state = State::Init;
    int line_start = -1;
    for (uint32_t idx = 0;idx < str_.size();++idx) {
        bool state_changed = true;
        int lastIdx = idx;
        switch (str_[idx]) {
        case 0:
            if (state == State::Ruby) {
                state_changed = false;
            }
            else {
                state = State::Init;
            }
            break;
        case 1:
            state = State::Init;
            idx += 4;
            break;
        case 2:
            state = State::Init;
            ++idx;
            break;
        case 3:
            state = State::Init;
            idx += 2;
            break;
        case 4:
            state = State::Init;
            ++idx;
            break;
        case 5:
            state = State::Init;
            idx += 2;
            break;
        case 6:
            state = State::Init;
            idx += 2;
            break;
        case 0xA:
            if (state == State::Ruby) {
                state_changed = false;
            }
            else {
                state = State::Init;
            }
            break;
        case 7:
            switch (str_[++idx]) {
            case 1://递归调用文字读取，然后继续处理（包含注释的文字）
                state = State::Ruby;
                state_changed = false;
                break;
            case 4://下一句自动出来
                state = State::Init;
                break;
            case 6://代表本句结束
                state = State::Init;
                break;
            case 7://递归调用文字读取然后wcslen，跳过不处理。应该是用于注释
                state = State::Init;
                ++idx;
                idx += wcslen(&str_[idx]);
                break;
            case 8://LoadVoice 后面是Voice名
                state = State::Voice;
                break;
            case 9://LoadVoice结束
                state = State::Init;
                break;
            default:
                state = State::Init;
                break;
            }
            break;
        default:
            if (state != State::Text && state != State::Ruby) {
                state = State::Text;
                line_start = idx;
            }
            else {
                state_changed = false;
            }
        }
        if (state_changed && line_start != -1 && line_start != idx) {
            str_pos_.push_back(StrPos(line_start, lastIdx));
            line_start = -1;
        }
    }
    return true;
}

vector<wstring> MalieString::export_str() {
    vector<wstring> strs;
    for (auto& pos : str_pos_) {
        auto line = str_.substr(pos.first, pos.second - pos.first);
        for (uint32_t i = 0;i < line.length();i++) {
            if (line[i] == 7 && line[i + 1] == 1) {
                line[i] = L'&';
                line[i + 1] = L'{';
                i++;
            }
            else if (line[i] == 0xa) {
                line[i] = L'|';
            }
            else if (line[i] == 0) {
                line[i] = L'}';
            }
        }
        strs.push_back(move(line));
    }
    return move(strs);
}

wstring MalieString::import_str(std::function<std::wstring(void)> get_str) {
    if (str_pos_.size() == 0) {
        return str_;
    }
    wstringstream ss;
    auto last_idx = 0;
    for (auto& pos : str_pos_) {
        ss << str_.substr(last_idx, pos.first - last_idx);
        auto s = get_str();
        if (s.length() == 0) {
            return L"";
        }
        if (s[0] == L'v' && s[1] == L'_') {
            ss << str_.substr(pos.first, pos.second - pos.first);
        }
        else {
            for (size_t i = 0;i < s.length();i++) {
                if (s[i] == L'&' && s[i + 1] == L'{') {
                    s[i] = 7;
                    s[i + 1] = 1;
                    i++;
                }
                else if (s[i] == L'|') {
                    s[i] = 0xa;
                }
                else if (s[i] == L'}') {
                    s[i] = 0;
                }
            }
            ss << s;
        }
        last_idx = pos.second;
    }
    ss << str_.substr(str_pos_[str_pos_.size() - 1].second);
    return ss.str();
}

void AddSplitTxtW(NakedMemory& txt, vector<wstring>& ls)
{
    auto buff = (char16_t*)txt.Get();
    auto buff_len = txt.GetSize() / 2;
    if (buff[0] == 0xfeff) {
        buff++;
        buff_len--;
    }
    uint32_t i = 0;
    uint32_t last = 0;
    while (true)
    {
        while (i < buff_len && buff[i] != u'\r') i++;
        ls.push_back(wstring((wchar_t*)buff + last, i - last));
        if (i + 2 > buff_len) break;
        i += 2;
        last = i;
    }
}

vector<wstring> read_all_txt() {
    MyFileReader reader;
    vector<wstring> ls;
    for (int i = 0;i < 20;i++) {
        wchar_t fname[100];
        swprintf_s(fname, L"txt\\%02d.txt", i);
        auto mem = reader.ReadToMem(fname);
        if (mem.GetSize() == 0) {
            break;
        }
        AddSplitTxtW(mem, ls);
        while (ls[ls.size() - 1].length() == 0) {
            ls.pop_back();
        }
    }
    return move(ls);
}

map<int, wstring> g_data_tbl;
void read_data_table(map<int, wstring>& data_tbl) {
    MyFileReader reader;
    auto idx_mem = reader.ReadToMem(L"txt\\str.idx");
    if (idx_mem.GetSize() == 0) {
        LOGERROR("Can't find str.idx!");
        return;
    }
    auto txt = reader.ReadToMem(L"txt\\str.txt");
    if (txt.GetSize() == 0) {
        LOGERROR("Can't find str.txt!");
        return;
    }
    auto ls = SplitTxtW(txt);
    while (ls[ls.size() - 1].length() == 0) {
        ls.pop_back();
    }
    if (ls.size() != idx_mem.GetSize() / 4) {
        LOGERROR("str.txt and str.idx not fit!");
        return;
    }

    auto off_tbl = (int*)idx_mem.Get();
    for (size_t i = 0;i < ls.size();i++) {
        data_tbl[off_tbl[i]] = ls[i];
    }
}

map<int, pair<int, int>> g_str_idx;
vector<wstring>* g_strs;

void malie_init() {
    {
        auto ls = read_all_txt();
        g_strs = new vector<wstring>(move(ls));
    }

    if (g_strs->size() == 0) {
        LOGERROR("Error open txt file.");
        return;
    }

    wistringstream ss;
    auto last_line = -1;
    int last_idx = -1;
    for (size_t i = 0;i < g_strs->size();i++) {
        auto& l = (*g_strs)[i];
        if (l[0] == L'#') {
            ss.clear();
            ss.str(l);
            if (last_idx != -1) {
                g_str_idx[last_idx] = pair<int, int>(last_line, i);
            }
            ss.seekg(1, 0);
            ss >> last_idx;
            last_line = i + 1;
        }
    }

    read_data_table(g_data_tbl);
}

CRITICAL_SECTION g_init_lock;

DWORD WINAPI malie_init_thread(LPVOID param) {
    EnterCriticalSection(&g_init_lock);
    malie_init();
    LeaveCriticalSection(&g_init_lock);
    return 0;
}

auto make_str_func(int idx) {
    auto start = -1, end = -1;
    static int cur_idx;
    auto itr = g_str_idx.find(idx);
    if (itr != g_str_idx.end()) {
        start = itr->second.first;
        end = itr->second.second;
    }
    cur_idx = start;
    
    return [start, end]() {
        if (start != -1 && cur_idx < end) {
            return (*g_strs)[cur_idx++];
        }
        else {
            return wstring(L"");
        }
    };
}

void HOOKFUNC MyReadStrTbl(Registers* regs) {
    EnterCriticalSection(&g_init_lock);
    auto str_idx = *(int*)(regs->esp + 0x20);
    auto ori_str = (wchar_t*)regs->esi;
    auto ori_len = regs->ecx / 2;

    MalieString str(wstring(ori_str, ori_len));
    str.init();
    auto new_str = str.import_str(make_str_func(str_idx));
    if (new_str.length() != 0) {
        static wchar_t static_str[1024];
        memcpy(static_str, new_str.c_str(), new_str.length() * 2);

        regs->esi = (DWORD)static_str;
        regs->ecx = new_str.length() * 2;
    }
    LeaveCriticalSection(&g_init_lock);
}

uint8_t* g_vm_data;

void HOOKFUNC MyReadVmData(Registers* regs) {
    g_vm_data = (uint8_t*)regs->eax;
}

void HOOKFUNC MyReadDataString(const wchar_t** s_ptr) {
    auto off = (uint8_t*)*s_ptr - g_vm_data;
    auto itr = g_data_tbl.find(off);
    if (itr != g_data_tbl.end()) {
        *s_ptr = itr->second.c_str();
    }
}