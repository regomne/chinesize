#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <functional>

class MalieString
{
public:
    MalieString(const std::wstring& str) :str_(str) {}
    bool init();
    std::vector<std::wstring> export_str();
    std::wstring import_str(std::function<std::wstring(void)> get_str);

private:
    enum class State {
        Init,
        Voice,
        Ruby,
        Text,
    };

    typedef std::pair<int, int> StrPos;

private:
    std::wstring str_;
    std::vector<StrPos> str_pos_;
};