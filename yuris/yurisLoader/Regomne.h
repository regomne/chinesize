#pragma once

#include <windows.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	__field_bcount_part(MaximumLength, Length) PWCH   Buffer;
} UNICODE_STRING;

typedef void (*PPEBLOCKROUTINE)(PVOID PebLock);

typedef struct _PEB_LDR_DATA
{
	ULONG Length; // +0x00
	BOOLEAN Initialized; // +0x04
	PVOID SsHandle; // +0x08
	LIST_ENTRY InLoadOrderModuleList; // +0x0c
	LIST_ENTRY InMemoryOrderModuleList; // +0x14
	LIST_ENTRY InInitializationOrderModuleList;// +0x1c
} PEB_LDR_DATA,*PPEB_LDR_DATA; // +0x24

typedef struct _LDR_MODULE
{
	LIST_ENTRY          InLoadOrderModuleList;   //+0x00
	LIST_ENTRY          InMemoryOrderModuleList; //+0x08  
	LIST_ENTRY          InInitializationOrderModuleList; //+0x10
	void*               BaseAddress;  //+0x18
	void*               EntryPoint;   //+0x1c
	ULONG               SizeOfImage;
	UNICODE_STRING      FullDllName;
	UNICODE_STRING      BaseDllName;
	ULONG               Flags;
	SHORT               LoadCount;
	SHORT               TlsIndex;
	HANDLE              SectionHandle;
	ULONG               CheckSum;
	ULONG               TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	UNICODE_STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
	UNICODE_STRING CurrentDirectoryPath;
	HANDLE CurrentDirectoryHandle;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingPositionLeft;
	ULONG StartingPositionTop;
	ULONG Width;
	ULONG Height;
	ULONG CharWidth;
	ULONG CharHeight;
	ULONG ConsoleTextAttributes;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopName;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_FREE_BLOCK {
	struct _PEB_FREE_BLOCK *Next;
	ULONG Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

typedef struct _PEB { // Size: 0x1D8
	/*000*/ UCHAR InheritedAddressSpace;
	/*001*/ UCHAR ReadImageFileExecOptions;
	/*002*/ UCHAR BeingDebugged;
	/*003*/ UCHAR SpareBool; // Allocation size
	/*004*/ HANDLE Mutant;
	/*008*/ HINSTANCE ImageBaseAddress; // Instance
	/*00C*/ PPEB_LDR_DATA LoaderData; 
	/*010*/ PRTL_DRIVE_LETTER_CURDIR *ProcessParameters;
	/*014*/ ULONG SubSystemData;
	/*018*/ HANDLE DefaultHeap;
	/*01C*/ KSPIN_LOCK FastPebLock;
	/*020*/ PPEBLOCKROUTINE FastPebLockRoutine;
	/*024*/ PPEBLOCKROUTINE FastPebUnlockRoutine;
	/*028*/ ULONG EnvironmentUpdateCount;
	/*02C*/ ULONG KernelCallbackTable;
	/*030*/ PVOID EventLogSection;
	/*034*/ PVOID EventLog;
	/*038*/ PPEB_FREE_BLOCK FreeList;
	/*03C*/ ULONG TlsExpansionCounter;
	/*040*/ ULONG TlsBitmap;
	/*044*/ LARGE_INTEGER TlsBitmapBits;
	/*04C*/ ULONG ReadOnlySharedMemoryBase;
	/*050*/ ULONG ReadOnlySharedMemoryHeap;
	/*054*/ ULONG ReadOnlyStaticServerData;
	/*058*/ ULONG AnsiCodePageData;
	/*05C*/ ULONG OemCodePageData;
	/*060*/ ULONG UnicodeCaseTableData;
	/*064*/ ULONG NumberOfProcessors;
	/*068*/ LARGE_INTEGER NtGlobalFlag; // Address of a local copy
	/*070*/ LARGE_INTEGER CriticalSectionTimeout;
	/*078*/ ULONG HeapSegmentReserve;
	/*07C*/ ULONG HeapSegmentCommit;
	/*080*/ ULONG HeapDeCommitTotalFreeThreshold;
	/*084*/ ULONG HeapDeCommitFreeBlockThreshold;
	/*088*/ ULONG NumberOfHeaps;
	/*08C*/ ULONG MaximumNumberOfHeaps;
	/*090*/ ULONG ProcessHeaps;
	/*094*/ ULONG GdiSharedHandleTable;
	/*098*/ ULONG ProcessStarterHelper;
	/*09C*/ ULONG GdiDCAttributeList;
	/*0A0*/ KSPIN_LOCK LoaderLock;
	/*0A4*/ ULONG OSMajorVersion;
	/*0A8*/ ULONG OSMinorVersion;
	/*0AC*/ USHORT OSBuildNumber;
	/*0AE*/ USHORT OSCSDVersion;
	/*0B0*/ ULONG OSPlatformId;
	/*0B4*/ ULONG ImageSubsystem;
	/*0B8*/ ULONG ImageSubsystemMajorVersion;
	/*0BC*/ ULONG ImageSubsystemMinorVersion;
	/*0C0*/ ULONG ImageProcessAffinityMask;
	/*0C4*/ ULONG GdiHandleBuffer[0x22];
	/*14C*/ ULONG PostProcessInitRoutine;
	/*150*/ ULONG TlsExpansionBitmap;
	/*154*/ UCHAR TlsExpansionBitmapBits[0x80];
	/*1D4*/ ULONG SessionId;
} PEB, *PPEB;
/*
typedef struct _FLOATING_SAVE_AREA {
	ULONG   ControlWord;
	ULONG   StatusWord;
	ULONG   TagWord;
	ULONG   ErrorOffset;
	ULONG   ErrorSelector;
	ULONG   DataOffset;
	ULONG   DataSelector;
	UCHAR   RegisterArea[80];
	ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;




//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

	//
	// The flags values within this flag control the contents of
	// a CONTEXT record.
	//
	// If the context record is used as an input parameter, then
	// for each portion of the context record controlled by a flag
	// whose value is set, it is assumed that that portion of the
	// context record contains valid context. If the context record
	// is being used to modify a threads context, then only that
	// portion of the threads context will be modified.
	//
	// If the context record is used as an IN OUT parameter to capture
	// the context of a thread, then only those portions of the thread's
	// context corresponding to set flags will be returned.
	//
	// The context record is never used as an OUT only parameter.
	//

	ULONG ContextFlags;

	//
	// This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
	// set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
	// included in CONTEXT_FULL.
	//

	ULONG   Dr0;
	ULONG   Dr1;
	ULONG   Dr2;
	ULONG   Dr3;
	ULONG   Dr6;
	ULONG   Dr7;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
	//

	FLOATING_SAVE_AREA FloatSave;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_SEGMENTS.
	//

	ULONG   SegGs;
	ULONG   SegFs;
	ULONG   SegEs;
	ULONG   SegDs;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_INTEGER.
	//

	ULONG   Edi;
	ULONG   Esi;
	ULONG   Ebx;
	ULONG   Edx;
	ULONG   Ecx;
	ULONG   Eax;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_CONTROL.
	//

	ULONG   Ebp;
	ULONG   Eip;
	ULONG   SegCs;              // MUST BE SANITIZED
	ULONG   EFlags;             // MUST BE SANITIZED
	ULONG   Esp;
	ULONG   SegSs;

	//
	// This section is specified/returned if the ContextFlags word
	// contains the flag CONTEXT_EXTENDED_REGISTERS.
	// The format and contexts are processor specific
	//

	UCHAR   ExtendedRegisters[512];

} CONTEXT;

typedef CONTEXT *PCONTEXT;
*/

BOOL WINAPI HookApi(WCHAR* lpszDllName,CHAR* lpszFuncName,void* lpNewFunc);
BOOL WINAPI PatchFunc(BYTE* Dest, void* lpNewFunc);
int WINAPI GetOpCodeSize32(PVOID Start);
