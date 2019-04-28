#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "NakedMemory.h"

std::vector<std::string> SplitTxtA(NakedMemory& txt);
std::vector<std::wstring> SplitTxtW(NakedMemory& txt);

NakedMemory CvtToAnsi(NakedMemory& str, int cp = CP_ACP);
char* ReadOneLineFromTxt();
