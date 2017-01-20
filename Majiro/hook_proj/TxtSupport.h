#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "NakedMemory.h"

std::vector<std::string> SplitTxtA(NakedMemory& txt);

NakedMemory CvtToAnsi(NakedMemory& str);
