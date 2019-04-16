#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <regex>

#define __DX_MAKE
#include <DxArchive_.h>
#include <DxChar.h>

#include <gflags/gflags.h>

DEFINE_bool(e, false, "extract file(s) from a DxArchive");
DEFINE_bool(l, false, "list files in a DxArchive");
DEFINE_string(key, "", "key of the archive");
DEFINE_string(dxarc, "", "specify the file name of the DxArchive");
DEFINE_string(o, "", "output list file name or output dir");
DEFINE_string(include, "", "extract filter with regexp");
DEFINE_string(exclude, "", "extract filter with regexp, after include filter");

using namespace DxLib;

std::wstring to_wstr(const char* s)
{
    static std::vector<wchar_t> ws;
    auto s_len = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
    if (s_len < 0)
    {
        throw std::runtime_error("unknown coding error");
    }
    if (ws.size() < (size_t)s_len)
    {
        ws.resize(s_len);
    }
    s_len = MultiByteToWideChar(CP_ACP, 0, s, -1, ws.data(), s_len);
    return { ws.data(), (size_t)s_len - 1 };
}

std::wstring arc_name_to_wstr(const char* s, int cp)
{
    static std::vector<wchar_t> ws;
    auto len = GetStringCharNum(s, cp);
    if (ws.size() < (size_t)len + 1)
    {
        ws.resize(len + 1);
    }
    if (ConvString(s, -1, cp, (char*)ws.data(), (ws.size() + 1) * sizeof(wchar_t), DX_CHARCODEFORMAT_UTF16LE) < 0)
    {
        throw std::runtime_error("cp2 encoding fail");
    }
    return { ws.data(), (size_t)len };
}

using ReadFileStreamRoutine = void(uint8_t* buffer, size_t buffer_size);
using FileCallback = void(const std::wstring& root_dir, const std::wstring& file_name, size_t file_size,
    std::function<ReadFileStreamRoutine>&& read_stream);
std::function<FileCallback> g_file_callback;

void walk_arc(DXARC* arc, const std::string& dir)
{
    //printf("extracting folder %s\n", dir.c_str());
    FILEINFOW info;
    auto handle = DXA_FindFirst(arc, (const uint8_t*)(dir + "/*").c_str(), &info);
    if (handle != -1)
    {
        auto dir_w = arc_name_to_wstr(dir.c_str(), arc->CharCodeFormat);
        do
        {
            char encoded_name[260 * 2];
            auto encoded_name_len = ConvString((const char*)info.Name, -1, DX_CHARCODEFORMAT_UTF16LE,
                encoded_name, sizeof(encoded_name), arc->CharCodeFormat);
            if (encoded_name_len < 0)
            {
                throw std::runtime_error("cp encoding fail");
            }
            if (strcmp(encoded_name, ".") == 0 ||
                strcmp(encoded_name, "..") == 0)
            {
                continue;
            }

            auto rel_path = dir + "/" + encoded_name;
            if (info.DirFlag)
            {
                g_file_callback(dir_w + L"/" + info.Name, L"", 0, 
                    [](uint8_t*, size_t) {});
                walk_arc(arc, rel_path.c_str());
            }
            else
            {
                int pos, file_size;
                if (DXA_GetFileInfo(arc, arc->CharCodeFormat, rel_path.c_str(), &pos, &file_size) < 0)
                {
                    throw std::runtime_error("get file info err");
                }
                g_file_callback(dir_w, info.Name, file_size, [arc, rel_path, file_size](uint8_t* buffer, size_t size) {
                    if (size < (size_t)file_size)
                    {
                        throw std::logic_error("logic error");
                    }
                    DXARC_STREAM strm;
                    if (DXA_STREAM_Initialize(&strm, arc, (const uint8_t*)rel_path.c_str(), 0) < 0)
                    {
                        throw std::runtime_error("get file stream err");
                    }
                    if (DXA_STREAM_Read(&strm, buffer, size) < 0)
                    {
                        DXA_STREAM_Terminate(&strm);
                        throw std::runtime_error("read stream err");
                    }
                    DXA_STREAM_Terminate(&strm);
                });
               
            }
        } while (DXA_FindNext(handle, &info) == 0);
        DXA_FindClose(handle);
    }
}

void extract_arc(DXARC* arc, const std::wstring& output_dir,
    const std::wstring& include_filter, const std::wstring& exclude_filter)
{
    std::shared_ptr<std::wregex> inc, exc;
    if (include_filter != L"")
    {
        inc.reset(new std::wregex(include_filter, std::regex_constants::syntax_option_type::icase));
    }
    if (exclude_filter != L"")
    {
        exc.reset(new std::wregex(exclude_filter, std::regex_constants::syntax_option_type::icase));
    }

    g_file_callback = [output_dir, inc, exc](const std::wstring& root_dir, const std::wstring& file_name, size_t file_size,
        std::function<ReadFileStreamRoutine>&& read_stream)
    {
        static std::vector<uint8_t> buff;

        auto file_path = root_dir + L"/" + file_name;
        auto full_path = output_dir + L"/" + file_path;

        if ((inc && !std::regex_search(file_path, *inc)) ||
            (exc && std::regex_search(file_path, *exc)))
        {
            return;
        }

        if (file_name == L"")
        {
            // is a dir
            CreateDirectory(full_path.c_str(), nullptr);
        }
        else
        {
            if (buff.size() < file_size)
            {
                buff.resize(file_size);
            }
            read_stream(buff.data(), file_size);

            auto hf = CreateFile(full_path.c_str(), GENERIC_WRITE,
                FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hf == INVALID_HANDLE_VALUE)
            {
                throw std::runtime_error("create file error");
            }
            DWORD bytes_wrote = 0;
            WriteFile(hf, buff.data(), file_size, &bytes_wrote, nullptr);
            CloseHandle(hf);
            if (bytes_wrote != file_size)
            {
                throw std::runtime_error("writing file error");
            }
        }
    };

    CreateDirectory(output_dir.c_str(), nullptr);
    try
    {
        walk_arc(arc, "");
    }
    catch (std::exception& e)
    {
        printf("extracting failed: %s\n", e.what());
    }
}

void list_arc(DXARC* arc, const std::wstring& output_file)
{
    std::shared_ptr<std::function<void(const std::wstring&)>> log_f;
    HANDLE of = INVALID_HANDLE_VALUE;
    if (output_file == L"")
    {
        of = GetStdHandle(STD_OUTPUT_HANDLE);
        log_f.reset(new std::function<void(const std::wstring&)>([of](const auto& s) {
            DWORD bytes_wrote = 0;
            WriteConsole(of, s.c_str(), s.length(), &bytes_wrote, nullptr);
        }));
    }
    else
    {
        of = CreateFile(output_file.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (of == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("can't open file to write");
        }
        DWORD bytes_wrote = 0;
        WriteFile(of, "\xff\xfe", 2, &bytes_wrote, nullptr);
        log_f.reset(new std::function<void(const std::wstring&)>([of](const auto& s) {
            DWORD bytes_wrote = 0;
            WriteFile(of, s.c_str(), s.length() * sizeof(wchar_t), &bytes_wrote, nullptr);
        }));
    }

    g_file_callback = [log_f](const std::wstring& root_dir, const std::wstring& file_name, size_t file_size,
        std::function<ReadFileStreamRoutine>&& read_stream)
    {
        std::wstring str;
        if (file_name == L"")
        {
            str = L"D " + root_dir + L"\r\n";
        }
        else
        {
            str = L"F " + root_dir + L"/" + file_name + L"\r\n";
        }
        (*log_f)(str);
    };

    try
    {
        walk_arc(arc, "");
    }
    catch (const std::exception& e)
    {
        printf("listing failed: %s\n", e.what());
    }

    CloseHandle(of);
}

int main(int argc, char* argv[])
{
    gflags::SetUsageMessage(R"(
    To list    files: exDxBin -l -dxarc <dxarc> {-key <key>} {-o <output_file>
    To extract files: exDxBin -e -dxarc <dxarc> {-key <key>} -o <output_dir> {-include <regexp>} {-exclude <regexp>}
)");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    //constexpr auto kBinKey = "_ppiixxeell_";
    if ((!FLAGS_e && !FLAGS_l) || (FLAGS_e && FLAGS_l) ||
        (FLAGS_e && (FLAGS_o == "" || FLAGS_dxarc == "")))
    {
        gflags::ShowUsageWithFlags(argv[0]);
        return 0;
    }
    auto arc_key = FLAGS_key == "" ? nullptr : FLAGS_key.c_str();

    DXARC arc;
    auto ret = DXA_Initialize(&arc);
    ret = DXA_OpenArchiveFromFile(&arc, to_wstr(FLAGS_dxarc.c_str()).c_str(), arc_key);
    if (ret != 0)
    {
        printf("can't open archive\n");
        return 0;
    }

    if (FLAGS_e)
    {
        try
        {
            extract_arc(&arc, to_wstr(FLAGS_o.c_str()), to_wstr(FLAGS_include.c_str()), to_wstr(FLAGS_exclude.c_str()));
        }
        catch (const std::regex_error& e)
        {
            printf("invalid regex expression:%s", e.what());
        }
    }
    else if (FLAGS_l)
    {
        list_arc(&arc, to_wstr(FLAGS_o.c_str()));
    }

    return 0;
}
