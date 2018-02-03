#pragma once
#include <windows.h>
#include "ilhook.h"

typedef BOOL(WINAPI *GetVolumeInformationRoutine)(
    LPCTSTR lpRootPathName,
    LPTSTR  lpVolumeNameBuffer,
    DWORD   nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPTSTR  lpFileSystemNameBuffer,
    DWORD   nFileSystemNameSize
    );
BOOL HOOKFUNC MyGetVolumeInformation(
    GetVolumeInformationRoutine oldfunc,
    LPCTSTR lpRootPathName,
    LPTSTR  lpVolumeNameBuffer,
    DWORD   nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPTSTR  lpFileSystemNameBuffer,
    DWORD   nFileSystemNameSize
);