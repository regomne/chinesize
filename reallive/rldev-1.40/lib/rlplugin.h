/*
  rlplugin.h: interface for RealLive plugin DLLs.
  Thanks to roxfan for figuring this out.

  RealLives known to support DLLs are 1.2.3, 1.2.6, and 1.2.9.  Of those, 1.2.3 
  is not binary-compatible with the others; you must define OLD_INTERFACE when 
  compiling for it.

  Remember to export the functions undecorated.  Passing rlplugin.def to the
  linker should have the required effect for Microsoft's compiler; for gcc,
  use the option '-Wl,--add-stdcall-alias'.
*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define RLC_PLUGIN_FUNC(f) WINAPI reallive_dll_func_ ## f
#else
typedef void *HWND;
#define RLC_PLUGIN_FUNC(f) plugin_func_ ## f
#endif

#ifndef OLD_INTERFACE
#define OnLoad RLC_PLUGIN_FUNC(load)
#define OnInit RLC_PLUGIN_FUNC(init)
#else
#define OnLoad RLC_PLUGIN_FUNC(init)
#endif
#define OnFree RLC_PLUGIN_FUNC(free)
#define OnCall RLC_PLUGIN_FUNC(call)

typedef struct {
  long cbSize;      /* Size of the structure */
  HWND hMainWindow; /* Handle of the RealLive window  */
  long *intA;
  long *intB;
  long *intC;
  long *intD;
  long *intE;
  long *intF;
  long *intG;
  long *intZ;
  char **strS;
  char **strM;
  struct {          /* DC array: */
    void **pData;     /* Pixel data */
    long *pWidth;     /* DC width */
    long *pHeight;    /* DC height */
  } BankInfo[16];
  char *szGamePath; /* Game root directory (path of GAMEEXE.INI) */
  char *szSavePath; /* Save directory */
  char *szBgmPath;  /* BGM directory */
  char *szKoePath;  /* KOE directory */
  char *szMovPath;  /* MOV directory */
  char *szDataPath; /* Base directory for other resources (wav, g00, etc.) */
} RealLiveState;

#ifdef __cplusplus
extern "C" {
#endif

/* Called when the library is loaded, either with LoadDLL or a #DLL directive. */
__declspec(dllexport)
long OnLoad (RealLiveState*, long);

#ifndef OLD_INTERFACE
/* Called when the system is reset, e.g. when a saved game is loaded. */
__declspec(dllexport) 
void OnInit ();
#endif

/* Called when the library is unloaded. */
__declspec(dllexport) 
long OnFree ();

/* The main function accessed by CallDLL().  Unsupplied parameters default to 0. */
__declspec(dllexport) 
long OnCall (int, int, int, int, int);

#ifdef __cplusplus
}
#endif
