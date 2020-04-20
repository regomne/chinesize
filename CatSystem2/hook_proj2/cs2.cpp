#include "cs2.h"
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
) {
    auto ret = oldfunc(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber,
        lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
    if (!ret) {
        return ret;
    }
    if (lpVolumeSerialNumber) {
        *lpVolumeSerialNumber = 0xb2de6fd3;
    }
    return ret;
}