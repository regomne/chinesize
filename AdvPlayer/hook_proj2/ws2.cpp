#include "ws2.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#include "crc32.h"
#include "FileReader.h"
#include "FuncHelper.h"
#include "TxtSupport.h"

using namespace std;

bool g_Ws2HasChanged;

struct TextInfo {
	vector<string>* lines;
	vector<uint32_t>* idxs;
};

map<uint32_t, TextInfo> g_TextInfo;
map<wstring, wstring> g_MyFiles;

#define READ_INST_RETURN_ADDR 0x4f073e
#define OPEN_FILE_ARG1_OFFSET 0x910
#define NAME_LINE_START "%LC"
#define LINE_END_WITH_WAIT "%K%P"
//v1.5
//#define SCRIPT_OBJ_SCRIPT_BEGIN_OFFSET 8
//#define SCRIPT_OBJ_SCRIPT_END_OFFSET 9
//#define SCRIPT_OBJ_CUR_OFFSET 4

//v1.7
#define SCRIPT_OBJ_SCRIPT_BEGIN_OFFSET 5
#define SCRIPT_OBJ_SCRIPT_END_OFFSET 6
#define SCRIPT_OBJ_CUR_OFFSET 4

constexpr wchar_t* ChArcName = L"Ch.arc";

void HOOKFUNC MyOpenFile1(wchar_t* arcName, wchar_t* entName) {
	wstring s = entName;
	transform(s.begin(), s.end(), s.begin(), towlower);
	auto itr = g_MyFiles.find(s);
	if (itr != g_MyFiles.end()) {
		if (wcslen(arcName) >= 6) {
			wcscpy(arcName, itr->second.c_str());
			//             Log(L"Pic replaced: %s", entName);
		}
		else {
			Log(L"Replace arc name error:%s\n", arcName);
		}
	}
}

void HOOKFUNC MyOpenFile(Registers* regs) {
	auto fnamePtr = *(wchar_t**)(regs->esp + OPEN_FILE_ARG1_OFFSET);
	if (!fnamePtr) {
		return;
	}
	auto fname = wstring(fnamePtr);
	transform(fname.begin(), fname.end(), fname.begin(), towlower);
	if (fname.length() > 3 && fname.substr(fname.length() - 3) == L"ws2") {
		g_Ws2HasChanged = true;
	}
}

template<class T>
int bin_search(const T* start, uint32_t cnt, const T& val) {
	auto init_start = start;
	auto end = start + cnt;

	while (start <= end)
	{
		auto middle = start + (end - start) / 2;
		if (*middle > val) {
			end = middle - 1;
		}
		else if (*middle < val) {
			start = middle + 1;
		}
		else {
			return middle - init_start;
		}
	}
	return -1;
}

uint32_t CallProcFunc6(uint32_t old_proc_func, int a1, int a2, int a3, int a4, int a5, int a6) {
	uint32_t retVal;
	__asm {
		push a6;
		push a5;
		push a4;
		push a3;
		mov edx, a2;
		mov ecx, a1;
		call old_proc_func;
		mov retVal, eax;
	}
	return retVal;
}
uint32_t CallProcFunc7(uint32_t old_proc_func, int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
	uint32_t retVal;
	__asm {
		push a7
		push a6;
		push a5;
		push a4;
		push a3;
		mov edx, a2;
		mov ecx, a1;
		call old_proc_func;
		mov retVal, eax;
	}
	return retVal;
}

void get_script_info(uint32_t* script_obj, uint8_t** sc_start, uint8_t** sc_end, uint32_t* cur_off) {
	*sc_start = (uint8_t*)script_obj[SCRIPT_OBJ_SCRIPT_BEGIN_OFFSET];
	*sc_end = (uint8_t*)script_obj[SCRIPT_OBJ_SCRIPT_END_OFFSET];
	*cur_off = script_obj[SCRIPT_OBJ_CUR_OFFSET];
}

void get_script_info_v19(uint32_t* script_obj, uint32_t* script_obj2, uint8_t** sc_start, uint8_t** sc_end, uint32_t* cur_off) {
    *sc_start = (uint8_t*)script_obj[SCRIPT_OBJ_SCRIPT_BEGIN_OFFSET];
	*sc_end = (uint8_t*)script_obj[SCRIPT_OBJ_SCRIPT_END_OFFSET];
	*cur_off = *script_obj2;
}

string replace_string(uint8_t* scriptStart, uint32_t scriptSize, uint32_t curOff, const char* oldStringPtr)
{
	static TextInfo* txtInfo = nullptr;
	if (g_Ws2HasChanged) {
		g_Ws2HasChanged = false;
		auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
		auto itr = g_TextInfo.find(crc);
		if (itr != g_TextInfo.end()) {
			txtInfo = &itr->second;
		}
		else {
			txtInfo = nullptr;
		}
	}
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			string newString = "^8";
			auto oldString = string(oldStringPtr);
			if (oldString.find(NAME_LINE_START) == 0) {
				newString += NAME_LINE_START + lines[idx];
			}
			else if (oldString.length() >= 4 && oldString.substr(oldString.length() - 4) == LINE_END_WITH_WAIT) {
				newString += lines[idx] + LINE_END_WITH_WAIT;
			}
			else {
				newString += lines[idx];
			}
			return newString;
		}
	}
	return "";
}

uint32_t HOOKFUNC MyReadInst_v15(Registers* regs, uint32_t old_proc_func) {
	auto a6 = *(int*)(regs->esp + 0x10);
	auto a5 = *(int*)(regs->esp + 0xc);
	auto a4 = *(int*)(regs->esp + 0x8);
	auto a3 = *(int*)(regs->esp + 0x4);
	auto a2 = (int)regs->edx;
	auto a1 = (int)regs->ecx;
	if (*(uint32_t*)regs->esp != READ_INST_RETURN_ADDR) {
		return CallProcFunc6(old_proc_func, a1, a2, a3, a4, a5, a6);
	}

	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t curOff;
	get_script_info((uint32_t*)regs->esi, &scriptStart, &scriptEnd, &curOff);
	auto scriptSize = scriptEnd - scriptStart;

	static TextInfo* txtInfo = nullptr;
	if (g_Ws2HasChanged) {
		g_Ws2HasChanged = false;
		auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
		auto itr = g_TextInfo.find(crc);
		if (itr != g_TextInfo.end()) {
			txtInfo = &itr->second;
		}
		else {
			txtInfo = nullptr;
		}
	}
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			string newString = "^8";
			auto oldString = string((char*)a2);
			if (oldString.find(NAME_LINE_START) == 0) {
				newString += NAME_LINE_START + lines[idx];
			}
			else if (oldString.length() >= 4 && oldString.substr(oldString.length() - 4) == LINE_END_WITH_WAIT) {
				newString += lines[idx] + LINE_END_WITH_WAIT;
			}
			else {
				newString += lines[idx];
			}
			CallProcFunc6(old_proc_func, a1, (int)newString.c_str(), a3, a4, a5, a6);
			return oldString.length();
		}
	}
	return CallProcFunc6(old_proc_func, a1, a2, a3, a4, a5, a6);
}

uint32_t HOOKFUNC MyReadInst_v17(Registers* regs, uint32_t old_proc_func) {
	auto a7 = *(int*)(regs->esp + 0x14);
	auto a6 = *(int*)(regs->esp + 0x10);
	auto a5 = *(int*)(regs->esp + 0xc);
	auto a4 = *(int*)(regs->esp + 0x8);
	auto a3 = *(int*)(regs->esp + 0x4);
	auto a2 = (int)regs->edx;
	auto a1 = (int)regs->ecx;
	if (*(uint32_t*)regs->esp != READ_INST_RETURN_ADDR) {
		return CallProcFunc7(old_proc_func, a1, a2, a3, a4, a5, a6, a7);
	}

	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t curOff;
	get_script_info(*(uint32_t**)(regs->ebp - 0x24), &scriptStart, &scriptEnd, &curOff);
	auto scriptSize = scriptEnd - scriptStart;

	static TextInfo* txtInfo = nullptr;
	if (g_Ws2HasChanged) {
		g_Ws2HasChanged = false;
		auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
		auto itr = g_TextInfo.find(crc);
		if (itr != g_TextInfo.end()) {
			txtInfo = &itr->second;
		}
		else {
			txtInfo = nullptr;
		}
	}
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			string newString = "^8";
			auto oldString = string((char*)a4);
			if (oldString.find(NAME_LINE_START) == 0) {
				newString += NAME_LINE_START + lines[idx];
			}
			else if (oldString.length() >= 4 && oldString.substr(oldString.length() - 4) == LINE_END_WITH_WAIT) {
				newString += lines[idx] + LINE_END_WITH_WAIT;
			}
			else {
				newString += lines[idx];
			}
			CallProcFunc7(old_proc_func, a1, a2, a3, (int)newString.c_str(), a5, a6, a7);
			return oldString.length();
		}
	}
	return CallProcFunc7(old_proc_func, a1, a2, a3, a4, a5, a6, a7);
}

uint32_t HOOKFUNC MyReadInst_v19(Registers* regs, old_read_inst_func old_proc_func, uint32_t a0, uint32_t a1, const char* a2, uint32_t a3) {
	if (*(uint32_t*)regs->esp != READ_INST_RETURN_ADDR) {
		return old_proc_func(a0, a1, a2, a3);
	}
	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t curOff;
	get_script_info_v19(*(uint32_t**)(regs->ebp - 0x10),*(uint32_t**)(regs->ebp + 0xC), &scriptStart, &scriptEnd, &curOff);
	auto str = replace_string(scriptStart, scriptEnd - scriptStart, curOff, a2);
	if (str == "")
	{
		return old_proc_func(a0, a1, a2, a3);
	}
	old_proc_func(a0, a1, str.c_str(), a3);
	return strlen(a2);
}

void HOOKFUNC MyMbtowc(Registers* regs) {
	auto cp = (int*)(regs->esp);
	auto flags = (int*)(regs->esp + 4);
	auto strPtr = (char**)(regs->esp + 8);
	auto strLen = (uint32_t*)(regs->esp + 0xc);

	auto str = *strPtr;
	if (*strLen == -1 || *strLen > 2) {
		if (str[0] == '^' && str[1] == '8') {
			*cp = CP_UTF8;
			*flags = 0;
			*strPtr += 2;
			if (*strLen != -1 && *strLen > 2) {
				*strLen -= 2;
			}
		}
		else {
			*cp = 932;
		}
	}
}

void HOOKFUNC MySelString_v15(Registers* regs) {
	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t _nouse_curOff;
	get_script_info((uint32_t*)regs->ebp, &scriptStart, &scriptEnd, &_nouse_curOff);
	auto curOff = *(uint32_t*)regs->ebx;
	auto scriptSize = scriptEnd - scriptStart;

	TextInfo* txtInfo = nullptr;
	auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
	auto itr = g_TextInfo.find(crc);
	if (itr != g_TextInfo.end()) {
		txtInfo = &itr->second;
	}
	else {
		txtInfo = nullptr;
	}
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			auto& line = lines[idx];
			static char newString[200];
			strcpy_s(newString, "^8");
			if (line.find("Sel:") == 0) {
				strcat_s(newString, &line[4]);
			}
			else {
				strcat_s(newString, line.c_str());
			}
			*(char**)(regs->esp) = newString;
		}
	}
}

void HOOKFUNC MySelString_v17(Registers* regs) {
	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t curOff;
	get_script_info((uint32_t*)regs->edi, &scriptStart, &scriptEnd, &curOff);
	auto scriptSize = scriptEnd - scriptStart;

	TextInfo* txtInfo = nullptr;
	auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
	auto itr = g_TextInfo.find(crc);
	if (itr != g_TextInfo.end()) {
		txtInfo = &itr->second;
	}
	else {
		txtInfo = nullptr;
	}
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			auto& line = lines[idx];
			static char newString[200];
			strcpy_s(newString, "^8");
			if (line.find("Sel:") == 0) {
				strcat_s(newString, &line[4]);
			}
			else {
				strcat_s(newString, line.c_str());
			}
			*(char**)(regs->ecx) = newString;
		}
	}
}

void HOOKFUNC MyMbToWc2(int* cp, int* flags, char** str_ptr, uint32_t* str_len) {
	auto str = *str_ptr;
	if (*cp == 932 && (*str_len == -1 || *str_len > 2)) {
		if (str[0] == '^' && str[1] == '8') {
			*cp = CP_UTF8;
			*flags = 0;
			*str_ptr += 2;
            if (*str_len != -1 && *str_len > 2) {
                *str_len -= 2;
			}
		}
	}
}

void HOOKFUNC MySelString_v19(Registers* regs) {
	uint8_t* scriptStart;
	uint8_t* scriptEnd;
	uint32_t curOff;
    get_script_info_v19(*(uint32_t**)(regs->ebp - 0x2C), *(uint32_t**)(regs->ebp - 0x34),&scriptStart, &scriptEnd, &curOff);
	auto scriptSize = scriptEnd - scriptStart;

	TextInfo* txtInfo = nullptr;
	auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
	auto itr = g_TextInfo.find(crc);
	if (itr != g_TextInfo.end()) {
		txtInfo = &itr->second;
	}
	else {
		txtInfo = nullptr;
	}
	static char newString[200];
	if (txtInfo) {
		auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
		auto& lines = *(txtInfo->lines);
		if (idx != -1) {
			auto& line = lines[idx];
			static char newString[200];
			strcpy_s(newString, "^8");
			if (line.find("Sel:") == 0) {
				strcat_s(newString, &line[4]);
			}
			else {
				strcat_s(newString, line.c_str());
			}
            *(char**)(regs->esp + 8) = newString;
		}
	}
}

void HOOKFUNC MyChangeFont_v15(Registers* regs) {
	static const wchar_t* font_name = L"MS UI Gothic";
	regs->ecx = (DWORD)font_name;
}

void HOOKFUNC MyChangeFont_v17(const wchar_t** font_name) {
	static const wchar_t* static_font_name = L"MS UI Gothic";
    *font_name = static_font_name;
}

struct ArcHeader {
	uint32_t cnt;
	uint32_t idxSize;
};

struct EntryInfo {
	uint32_t size;
	uint32_t off;
};

bool ReadArcFileList(const wchar_t* arcName, map<wstring, wstring>& fileMap) {
	FILE* fp = nullptr;
	_wfopen_s(&fp, arcName, L"rb");
	if (!fp) {
		return false;
	}

	ArcHeader hdr;
	fread(&hdr, 1, sizeof(hdr), fp);
	for (uint32_t i = 0; i < hdr.cnt; i++) {
		EntryInfo info;
		fread(&info, 1, sizeof(info), fp);

		wstringstream s;
		wchar_t c;
		fread(&c, 1, sizeof(c), fp);
		while (c != 0)
		{
			s << c;
			if (fread(&c, 1, sizeof(c), fp) != sizeof(c)) break;
		}
		auto str = s.str();
		transform(str.begin(), str.end(), str.begin(), towlower);
		fileMap[str] = arcName;
	}
	fclose(fp);
	return true;
}

#pragma pack(1)
struct FileEntry {
	char fname[51];
	uint32_t offset;
	uint32_t size;
};
#pragma pack()

vector<wstring> GetFileList(wchar_t* path) {
	WIN32_FIND_DATA wfd;
	vector<wstring> files;
	auto hf = FindFirstFile(path, &wfd);
	if (hf != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(wfd.cFileName);
		} while (FindNextFile(hf, &wfd));
		FindClose(hf);
	}
	else {
		Log("findFile fail");
	}
	return std::move(files);
}

#include "key.h"

void XorDec(void* dst, uint32_t len) {
	auto p = (uint8_t*)dst;
	for (uint32_t i = 0; i < len; i++) {
		p[i] ^= g_dat_key[i & 1023];
	}
}

map<wstring, FileEntry> g_fileInfos;

vector<wstring> GetFileList(NakedMemory& mem) {
	auto bf = (uint8_t*)mem.Get();
	auto file_cnt = *(uint32_t*)bf;
	auto entries = (FileEntry*)(bf + 4);
	vector<wstring> files;
	for (int i = 0; i < file_cnt; i++) {
		files.push_back(decode_string(entries[i].fname, CP_UTF8));
		g_fileInfos[files[i]] = entries[i];
	}
	return std::move(files);
}

vector<string>* ReadTxt(NakedMemory& pack, const wstring& txtName) {
	auto& entry = g_fileInfos[txtName];
	NakedMemory mem((uint8_t*)pack.Get() + entry.offset, entry.size);
	auto ansi = CvtToAnsi(mem, CP_UTF8);
	auto lines = SplitTxtA(ansi);
	while (lines[lines.size() - 1] == "")
	{
		lines.pop_back();
	}
	return new vector<string>(std::move(lines));
}

vector<uint32_t>* ReadIdx(NakedMemory& pack, const wstring& idxName, uint32_t* crc) {
	auto& entry = g_fileInfos[idxName];
	NakedMemory mem((uint8_t*)pack.Get() + entry.offset, entry.size);
	auto buff = (uint32_t*)mem.Get();
	*crc = *buff;
	auto vec = new vector<uint32_t>((mem.GetSize() - 4) / 4);
	memcpy(&(*vec)[0], buff + 1, mem.GetSize() - 4);
	return vec;
}

void InitWs2() {
	MyFileReader reader;
	auto mem = reader.ReadToMem("cnpack");
	if (!mem.Get()) {
		Log(L"Can't read pack.");
		return;
	}
	XorDec(mem.Get(), mem.GetSize());
	auto txtList = GetFileList(mem);
	for (auto& txtName : txtList) {
		if (txtName.substr(txtName.length() - 3) != L"txt") {
			continue;
		}
		auto idxFullName = L"idx\\" + txtName.substr(4, txtName.length() - 8) + L".idx";
		auto txtFullName = txtName;
		auto lines = ReadTxt(mem, txtFullName);
		if (!lines) {
			Log(L"Reading %s error.", txtFullName.c_str());
			continue;
		}
		uint32_t crc;
		auto idxs = ReadIdx(mem, idxFullName, &crc);
		if (!idxs) {
			Log(L"Reading %s eror.", idxFullName.c_str());
			delete lines;
			continue;
		}
		if (idxs->size() != lines->size()) {
			Log(L"%s line error, need:%d, real:%d", txtFullName.c_str(), idxs->size(), lines->size());
			delete lines;
			delete idxs;
			continue;
		}
		TextInfo info = { lines,idxs };
		g_TextInfo[crc] = info;
	}

	if (!ReadArcFileList(ChArcName, g_MyFiles)) {
		Log(L"Can't find arc:%s", ChArcName);
	}
}