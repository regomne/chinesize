//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用入力情報プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_INPUT

// インクルード----------------------------------------------------------------
#include "DxInputWin.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGuid.h"
#include "../DxLog.h"
#include "../DxInput.h"
#include "../DxSystem.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

#define DEADZONE_D							(0.35)
#define DEADZONE							(DWORD)( DEADZONE_D * 65536 )
#define DEADZONE_DIRECTINPUT( ZONE )		(DWORD)( 10000 * (ZONE) / 65536)
#define DEADZONE_WINMM( ZONE )				(DWORD)(0x8000 * (ZONE) / 65536)
#define DEADZONE_XINPUT( ZONE )				(SHORT)( 32767 * (ZONE) / 65536)
#define DEADZONE_XINPUT_TRIGGER( ZONE )		(SHORT)(   255 * (ZONE) / 65536)
#define VALIDRANGE_WINMM( ZONE )			(0x8000 - DEADZONE_WINMM(ZONE))
#define VALIDRANGE_XINPUT( ZONE )			( 32767 - DEADZONE_XINPUT(ZONE))
#define VALIDRANGE_XINPUT_TRIGGER( ZONE )	(   255 - DEADZONE_XINPUT_TRIGGER(ZONE))

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS			(0xBB)
#endif

#ifndef VK_OEM_COMMA
#define VK_OEM_COMMA		(0xBC)
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS		(0xBD)
#endif

#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD		(0xBE)
#endif

#ifndef VK_OEM_1
#define VK_OEM_1			(0xBA)
#endif

#ifndef VK_OEM_2
#define VK_OEM_2			(0xBF)
#endif

#ifndef VK_OEM_3
#define VK_OEM_3			(0xC0)
#endif

#ifndef VK_OEM_4
#define VK_OEM_4			(0xDB)
#endif

#ifndef VK_OEM_5
#define VK_OEM_5			(0xDC)
#endif

#ifndef VK_OEM_6
#define VK_OEM_6			(0xDD)
#endif

#ifndef VK_OEM_7
#define VK_OEM_7			(0xDE)
#endif

#ifndef VK_OEM_102
#define VK_OEM_102			(0xE2)
#endif

#ifndef VK_OEM_COPY
#define VK_OEM_COPY			(0xF2)
#endif

#ifndef VK_OEM_AUTO
#define VK_OEM_AUTO			(0xF3)
#endif

#ifndef VK_OEM_ENLW
#define VK_OEM_ENLW			(0xF4)
#endif

#ifndef VK_XBUTTON1
#define VK_XBUTTON1			(0x05)
#endif

#ifndef VK_XBUTTON2
#define VK_XBUTTON2			(0x06)
#endif

#ifndef WHEEL_DELTA
#define WHEEL_DELTA			(120)
#endif

// 型定義----------------------------------------------------------------------

// 定数定義 ----------------------------------------------------------------------

//#include "DxInputDef.h"
// キーボードデバイスのデータフォーマット定義 =============

// ＧＵＩＤ
extern GUID GUIDDIKEYBOARD ;

// デバイスオブジェクトデータ
extern D_DIOBJECTDATAFORMAT C_ODFDIKEYBOARD[256] ;

// デバイスデータフォーマット
extern D_DIDATAFORMAT C_DFDIKEYBOARD ;


// ジョイスティックデバイスのデータフォーマット定義 =======

// ＧＵＩＤ
extern GUID GUIDDIJOYSTICK[8] ;

// デバイスオブジェクトデータ
extern D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK[44] ;

// デバイスデータフォーマット
extern D_DIDATAFORMAT C_DFDIJOYSTICK ;
extern D_DIDATAFORMAT C_DFDIJOYSTICK2 ;
extern D_DIDATAFORMAT C_DFDIMOUSE2 ;
extern D_DIDATAFORMAT C_DFDIMOUSE ;

// データ宣言------------------------------------------------------------------

// キーボードデバイスのデータフォーマット定義 -----------------------

// ＧＵＩＤ
GUID GUIDDIKEYBOARD =
{
//	Data1		Data2	Data3	Data4
	0x55728220,	0xd33c,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00", 
} ;

// デバイスオブジェクトデータ
D_DIOBJECTDATAFORMAT C_ODFDIKEYBOARD[256] =
{
//		pguid				dwOfs	dwType 		dwFlags
	{ 	&GUIDDIKEYBOARD, 	0x00,	0x8000000c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x01,	0x8000010c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x02,	0x8000020c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x03,	0x8000030c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x04,	0x8000040c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x05,	0x8000050c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x06,	0x8000060c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x07,	0x8000070c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x08,	0x8000080c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x09,	0x8000090c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0a,	0x80000a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0b,	0x80000b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0c,	0x80000c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0d,	0x80000d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0e,	0x80000e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0f,	0x80000f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x10,	0x8000100c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x11,	0x8000110c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x12,	0x8000120c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x13,	0x8000130c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x14,	0x8000140c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x15,	0x8000150c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x16,	0x8000160c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x17,	0x8000170c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x18,	0x8000180c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x19,	0x8000190c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1a,	0x80001a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1b,	0x80001b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1c,	0x80001c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1d,	0x80001d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1e,	0x80001e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1f,	0x80001f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x20,	0x8000200c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x21,	0x8000210c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x22,	0x8000220c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x23,	0x8000230c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x24,	0x8000240c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x25,	0x8000250c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x26,	0x8000260c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x27,	0x8000270c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x28,	0x8000280c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x29,	0x8000290c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2a,	0x80002a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2b,	0x80002b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2c,	0x80002c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2d,	0x80002d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2e,	0x80002e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2f,	0x80002f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x30,	0x8000300c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x31,	0x8000310c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x32,	0x8000320c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x33,	0x8000330c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x34,	0x8000340c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x35,	0x8000350c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x36,	0x8000360c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x37,	0x8000370c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x38,	0x8000380c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x39,	0x8000390c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3a,	0x80003a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3b,	0x80003b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3c,	0x80003c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3d,	0x80003d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3e,	0x80003e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3f,	0x80003f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x40,	0x8000400c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x41,	0x8000410c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x42,	0x8000420c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x43,	0x8000430c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x44,	0x8000440c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x45,	0x8000450c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x46,	0x8000460c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x47,	0x8000470c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x48,	0x8000480c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x49,	0x8000490c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4a,	0x80004a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4b,	0x80004b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4c,	0x80004c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4d,	0x80004d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4e,	0x80004e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4f,	0x80004f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x50,	0x8000500c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x51,	0x8000510c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x52,	0x8000520c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x53,	0x8000530c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x54,	0x8000540c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x55,	0x8000550c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x56,	0x8000560c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x57,	0x8000570c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x58,	0x8000580c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x59,	0x8000590c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5a,	0x80005a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5b,	0x80005b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5c,	0x80005c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5d,	0x80005d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5e,	0x80005e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5f,	0x80005f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x60,	0x8000600c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x61,	0x8000610c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x62,	0x8000620c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x63,	0x8000630c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x64,	0x8000640c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x65,	0x8000650c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x66,	0x8000660c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x67,	0x8000670c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x68,	0x8000680c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x69,	0x8000690c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6a,	0x80006a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6b,	0x80006b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6c,	0x80006c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6d,	0x80006d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6e,	0x80006e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6f,	0x80006f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x70,	0x8000700c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x71,	0x8000710c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x72,	0x8000720c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x73,	0x8000730c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x74,	0x8000740c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x75,	0x8000750c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x76,	0x8000760c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x77,	0x8000770c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x78,	0x8000780c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x79,	0x8000790c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7a,	0x80007a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7b,	0x80007b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7c,	0x80007c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7d,	0x80007d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7e,	0x80007e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7f,	0x80007f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x80,	0x8000800c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x81,	0x8000810c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x82,	0x8000820c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x83,	0x8000830c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x84,	0x8000840c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x85,	0x8000850c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x86,	0x8000860c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x87,	0x8000870c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x88,	0x8000880c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x89,	0x8000890c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8a,	0x80008a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8b,	0x80008b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8c,	0x80008c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8d,	0x80008d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8e,	0x80008e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8f,	0x80008f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x90,	0x8000900c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x91,	0x8000910c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x92,	0x8000920c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x93,	0x8000930c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x94,	0x8000940c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x95,	0x8000950c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x96,	0x8000960c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x97,	0x8000970c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x98,	0x8000980c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x99,	0x8000990c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9a,	0x80009a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9b,	0x80009b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9c,	0x80009c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9d,	0x80009d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9e,	0x80009e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9f,	0x80009f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xa0,	0x8000a00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa1,	0x8000a10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa2,	0x8000a20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa3,	0x8000a30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa4,	0x8000a40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa5,	0x8000a50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa6,	0x8000a60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa7,	0x8000a70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa8,	0x8000a80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa9,	0x8000a90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xaa,	0x8000aa0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xab,	0x8000ab0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xac,	0x8000ac0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xad,	0x8000ad0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xae,	0x8000ae0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xaf,	0x8000af0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xb0,	0x8000b00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb1,	0x8000b10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb2,	0x8000b20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb3,	0x8000b30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb4,	0x8000b40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb5,	0x8000b50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb6,	0x8000b60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb7,	0x8000b70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb8,	0x8000b80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb9,	0x8000b90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xba,	0x8000ba0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbb,	0x8000bb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbc,	0x8000bc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbd,	0x8000bd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbe,	0x8000be0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbf,	0x8000bf0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xc0,	0x8000c00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc1,	0x8000c10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc2,	0x8000c20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc3,	0x8000c30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc4,	0x8000c40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc5,	0x8000c50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc6,	0x8000c60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc7,	0x8000c70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc8,	0x8000c80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc9,	0x8000c90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xca,	0x8000ca0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcb,	0x8000cb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcc,	0x8000cc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcd,	0x8000cd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xce,	0x8000ce0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcf,	0x8000cf0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xd0,	0x8000d00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd1,	0x8000d10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd2,	0x8000d20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd3,	0x8000d30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd4,	0x8000d40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd5,	0x8000d50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd6,	0x8000d60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd7,	0x8000d70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd8,	0x8000d80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd9,	0x8000d90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xda,	0x8000da0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdb,	0x8000db0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdc,	0x8000dc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdd,	0x8000dd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xde,	0x8000de0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdf,	0x8000df0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xe0,	0x8000e00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe1,	0x8000e10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe2,	0x8000e20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe3,	0x8000e30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe4,	0x8000e40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe5,	0x8000e50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe6,	0x8000e60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe7,	0x8000e70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe8,	0x8000e80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe9,	0x8000e90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xea,	0x8000ea0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xeb,	0x8000eb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xec,	0x8000ec0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xed,	0x8000ed0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xee,	0x8000ee0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xef,	0x8000ef0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xf0,	0x8000f00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf1,	0x8000f10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf2,	0x8000f20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf3,	0x8000f30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf4,	0x8000f40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf5,	0x8000f50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf6,	0x8000f60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf7,	0x8000f70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf8,	0x8000f80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf9,	0x8000f90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfa,	0x8000fa0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfb,	0x8000fb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfc,	0x8000fc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfd,	0x8000fd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfe,	0x8000fe0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xff,	0x8000ff0c,	0x0 },
} ;

// デバイスデータフォーマット
D_DIDATAFORMAT C_DFDIKEYBOARD =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x02,	// dwFlags
	256,	// dwDataSize
	256,	// dwNumObjs
	C_ODFDIKEYBOARD, // rgodf
} ;


// ジョイスティックデバイスのデータフォーマット定義 -----------------

// ＧＵＩＤ
GUID GUID_X_AXIS	= { 0xa36d02e0, 0xc9f3, 0x11cf, "\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_Y_AXIS	= { 0xa36d02e1,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_Z_AXIS	= { 0xa36d02e2,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RX_AXIS	= { 0xa36d02f4,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RY_AXIS	= { 0xa36d02f5,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RZ_AXIS	= { 0xa36d02e3,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_SLIDER	= { 0xa36d02e4,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_POV		= { 0xa36d02f2,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };

// ジョイスティック１デバイスオブジェクトデータ
D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK[44] =
{
//	pguid				dwOfs	dwType		dwFlags
	&GUID_X_AXIS,		0x00,	0x80ffff03,	0x100,
	&GUID_Y_AXIS,		0x04,	0x80ffff03,	0x100,
	&GUID_Z_AXIS,		0x08,	0x80ffff03,	0x100,
	&GUID_RX_AXIS,		0x0c,	0x80ffff03,	0x100,
	&GUID_RY_AXIS,		0x10,	0x80ffff03,	0x100,
	&GUID_RZ_AXIS,		0x14,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x100,
	&GUID_POV,			0x20,	0x80ffff10,	0x000,
	&GUID_POV,			0x24,	0x80ffff10,	0x000,
	&GUID_POV,			0x28,	0x80ffff10,	0x000,
	&GUID_POV,			0x2c,	0x80ffff10,	0x000,
	NULL,				0x30,	0x80ffff0c,	0x000,
	NULL,				0x31,	0x80ffff0c,	0x000,
	NULL,				0x32,	0x80ffff0c,	0x000,
	NULL,				0x33,	0x80ffff0c,	0x000,
	NULL,				0x34,	0x80ffff0c,	0x000,
	NULL,				0x35,	0x80ffff0c,	0x000,
	NULL,				0x36,	0x80ffff0c,	0x000,
	NULL,				0x37,	0x80ffff0c,	0x000,
	NULL,				0x38,	0x80ffff0c,	0x000,
	NULL,				0x39,	0x80ffff0c,	0x000,
	NULL,				0x3a,	0x80ffff0c,	0x000,
	NULL,				0x3b,	0x80ffff0c,	0x000,
	NULL,				0x3c,	0x80ffff0c,	0x000,
	NULL,				0x3d,	0x80ffff0c,	0x000,
	NULL,				0x3e,	0x80ffff0c,	0x000,
	NULL,				0x3f,	0x80ffff0c,	0x000,
	NULL,				0x40,	0x80ffff0c,	0x000,
	NULL,				0x41,	0x80ffff0c,	0x000,
	NULL,				0x42,	0x80ffff0c,	0x000,
	NULL,				0x43,	0x80ffff0c,	0x000,
	NULL,				0x44,	0x80ffff0c,	0x000,
	NULL,				0x45,	0x80ffff0c,	0x000,
	NULL,				0x46,	0x80ffff0c,	0x000,
	NULL,				0x47,	0x80ffff0c,	0x000,
	NULL,				0x48,	0x80ffff0c,	0x000,
	NULL,				0x49,	0x80ffff0c,	0x000,
	NULL,				0x4a,	0x80ffff0c,	0x000,
	NULL,				0x4b,	0x80ffff0c,	0x000,
	NULL,				0x4c,	0x80ffff0c,	0x000,
	NULL,				0x4d,	0x80ffff0c,	0x000,
	NULL,				0x4e,	0x80ffff0c,	0x000,
	NULL,				0x4f,	0x80ffff0c,	0x000,
} ;

// ジョイスティック１デバイスデータフォーマット
D_DIDATAFORMAT C_DFDIJOYSTICK =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x01,	// dwFlags
	80,		// dwDataSize
	44,		// dwNumObjs
	C_ODFDIJOYSTICK, // rgodf
} ;

// ジョイスティック２デバイスオブジェクトデータ
D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK2[164] =
{
//	pguid				dwOfs	dwType		dwFlags
	&GUID_X_AXIS,		0x00,	0x80ffff03,	0x100,
	&GUID_Y_AXIS,		0x04,	0x80ffff03,	0x100,
	&GUID_Z_AXIS,		0x08,	0x80ffff03,	0x100,
	&GUID_RX_AXIS,		0x0c,	0x80ffff03,	0x100,
	&GUID_RY_AXIS,		0x10,	0x80ffff03,	0x100,
	&GUID_RZ_AXIS,		0x14,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x100,
	&GUID_POV,			0x20,	0x80ffff10,	0x000,
	&GUID_POV,			0x24,	0x80ffff10,	0x000,
	&GUID_POV,			0x28,	0x80ffff10,	0x000,
	&GUID_POV,			0x2c,	0x80ffff10,	0x000,
	NULL,				0x30,	0x80ffff0c,	0x000,
	NULL,				0x31,	0x80ffff0c,	0x000,
	NULL,				0x32,	0x80ffff0c,	0x000,
	NULL,				0x33,	0x80ffff0c,	0x000,
	NULL,				0x34,	0x80ffff0c,	0x000,
	NULL,				0x35,	0x80ffff0c,	0x000,
	NULL,				0x36,	0x80ffff0c,	0x000,
	NULL,				0x37,	0x80ffff0c,	0x000,
	NULL,				0x38,	0x80ffff0c,	0x000,
	NULL,				0x39,	0x80ffff0c,	0x000,
	NULL,				0x3a,	0x80ffff0c,	0x000,
	NULL,				0x3b,	0x80ffff0c,	0x000,
	NULL,				0x3c,	0x80ffff0c,	0x000,
	NULL,				0x3d,	0x80ffff0c,	0x000,
	NULL,				0x3e,	0x80ffff0c,	0x000,
	NULL,				0x3f,	0x80ffff0c,	0x000,
	NULL,				0x40,	0x80ffff0c,	0x000,
	NULL,				0x41,	0x80ffff0c,	0x000,
	NULL,				0x42,	0x80ffff0c,	0x000,
	NULL,				0x43,	0x80ffff0c,	0x000,
	NULL,				0x44,	0x80ffff0c,	0x000,
	NULL,				0x45,	0x80ffff0c,	0x000,
	NULL,				0x46,	0x80ffff0c,	0x000,
	NULL,				0x47,	0x80ffff0c,	0x000,
	NULL,				0x48,	0x80ffff0c,	0x000,
	NULL,				0x49,	0x80ffff0c,	0x000,
	NULL,				0x4a,	0x80ffff0c,	0x000,
	NULL,				0x4b,	0x80ffff0c,	0x000,
	NULL,				0x4c,	0x80ffff0c,	0x000,
	NULL,				0x4d,	0x80ffff0c,	0x000,
	NULL,				0x4e,	0x80ffff0c,	0x000,
	NULL,				0x4f,	0x80ffff0c,	0x000,
	NULL,				0x50,	0x80ffff0c,	0x000,
	NULL,				0x51,	0x80ffff0c,	0x000,
	NULL,				0x52,	0x80ffff0c,	0x000,
	NULL,				0x53,	0x80ffff0c,	0x000,
	NULL,				0x54,	0x80ffff0c,	0x000,
	NULL,				0x55,	0x80ffff0c,	0x000,
	NULL,				0x56,	0x80ffff0c,	0x000,
	NULL,				0x57,	0x80ffff0c,	0x000,
	NULL,				0x58,	0x80ffff0c,	0x000,
	NULL,				0x59,	0x80ffff0c,	0x000,
	NULL,				0x5a,	0x80ffff0c,	0x000,
	NULL,				0x5b,	0x80ffff0c,	0x000,
	NULL,				0x5c,	0x80ffff0c,	0x000,
	NULL,				0x5d,	0x80ffff0c,	0x000,
	NULL,				0x5e,	0x80ffff0c,	0x000,
	NULL,				0x5f,	0x80ffff0c,	0x000,
	NULL,				0x60,	0x80ffff0c,	0x000,
	NULL,				0x61,	0x80ffff0c,	0x000,
	NULL,				0x62,	0x80ffff0c,	0x000,
	NULL,				0x63,	0x80ffff0c,	0x000,
	NULL,				0x64,	0x80ffff0c,	0x000,
	NULL,				0x65,	0x80ffff0c,	0x000,
	NULL,				0x66,	0x80ffff0c,	0x000,
	NULL,				0x67,	0x80ffff0c,	0x000,
	NULL,				0x68,	0x80ffff0c,	0x000,
	NULL,				0x69,	0x80ffff0c,	0x000,
	NULL,				0x6a,	0x80ffff0c,	0x000,
	NULL,				0x6b,	0x80ffff0c,	0x000,
	NULL,				0x6c,	0x80ffff0c,	0x000,
	NULL,				0x6d,	0x80ffff0c,	0x000,
	NULL,				0x6e,	0x80ffff0c,	0x000,
	NULL,				0x6f,	0x80ffff0c,	0x000,
	NULL,				0x70,	0x80ffff0c,	0x000,
	NULL,				0x71,	0x80ffff0c,	0x000,
	NULL,				0x72,	0x80ffff0c,	0x000,
	NULL,				0x73,	0x80ffff0c,	0x000,
	NULL,				0x74,	0x80ffff0c,	0x000,
	NULL,				0x75,	0x80ffff0c,	0x000,
	NULL,				0x76,	0x80ffff0c,	0x000,
	NULL,				0x77,	0x80ffff0c,	0x000,
	NULL,				0x78,	0x80ffff0c,	0x000,
	NULL,				0x79,	0x80ffff0c,	0x000,
	NULL,				0x7a,	0x80ffff0c,	0x000,
	NULL,				0x7b,	0x80ffff0c,	0x000,
	NULL,				0x7c,	0x80ffff0c,	0x000,
	NULL,				0x7d,	0x80ffff0c,	0x000,
	NULL,				0x7e,	0x80ffff0c,	0x000,
	NULL,				0x7f,	0x80ffff0c,	0x000,
	NULL,				0x80,	0x80ffff0c,	0x000,
	NULL,				0x81,	0x80ffff0c,	0x000,
	NULL,				0x82,	0x80ffff0c,	0x000,
	NULL,				0x83,	0x80ffff0c,	0x000,
	NULL,				0x84,	0x80ffff0c,	0x000,
	NULL,				0x85,	0x80ffff0c,	0x000,
	NULL,				0x86,	0x80ffff0c,	0x000,
	NULL,				0x87,	0x80ffff0c,	0x000,
	NULL,				0x88,	0x80ffff0c,	0x000,
	NULL,				0x89,	0x80ffff0c,	0x000,
	NULL,				0x8a,	0x80ffff0c,	0x000,
	NULL,				0x8b,	0x80ffff0c,	0x000,
	NULL,				0x8c,	0x80ffff0c,	0x000,
	NULL,				0x8d,	0x80ffff0c,	0x000,
	NULL,				0x8e,	0x80ffff0c,	0x000,
	NULL,				0x8f,	0x80ffff0c,	0x000,
	NULL,				0x90,	0x80ffff0c,	0x000,
	NULL,				0x91,	0x80ffff0c,	0x000,
	NULL,				0x92,	0x80ffff0c,	0x000,
	NULL,				0x93,	0x80ffff0c,	0x000,
	NULL,				0x94,	0x80ffff0c,	0x000,
	NULL,				0x95,	0x80ffff0c,	0x000,
	NULL,				0x96,	0x80ffff0c,	0x000,
	NULL,				0x97,	0x80ffff0c,	0x000,
	NULL,				0x98,	0x80ffff0c,	0x000,
	NULL,				0x99,	0x80ffff0c,	0x000,
	NULL,				0x9a,	0x80ffff0c,	0x000,
	NULL,				0x9b,	0x80ffff0c,	0x000,
	NULL,				0x9c,	0x80ffff0c,	0x000,
	NULL,				0x9d,	0x80ffff0c,	0x000,
	NULL,				0x9e,	0x80ffff0c,	0x000,
	NULL,				0x9f,	0x80ffff0c,	0x000,
	NULL,				0xa0,	0x80ffff0c,	0x000,
	NULL,				0xa1,	0x80ffff0c,	0x000,
	NULL,				0xa2,	0x80ffff0c,	0x000,
	NULL,				0xa3,	0x80ffff0c,	0x000,
	NULL,				0xa4,	0x80ffff0c,	0x000,
	NULL,				0xa5,	0x80ffff0c,	0x000,
	NULL,				0xa6,	0x80ffff0c,	0x000,
	NULL,				0xa7,	0x80ffff0c,	0x000,
	NULL,				0xa8,	0x80ffff0c,	0x000,
	NULL,				0xa9,	0x80ffff0c,	0x000,
	NULL,				0xaa,	0x80ffff0c,	0x000,
	NULL,				0xab,	0x80ffff0c,	0x000,
	NULL,				0xac,	0x80ffff0c,	0x000,
	NULL,				0xad,	0x80ffff0c,	0x000,
	NULL,				0xae,	0x80ffff0c,	0x000,
	NULL,				0xaf,	0x80ffff0c,	0x000,
	&GUID_X_AXIS,		0xb0,	0x80ffff03,	0x200,
	&GUID_Y_AXIS,		0xb4,	0x80ffff03,	0x200,
	&GUID_Z_AXIS,		0xb8,	0x80ffff03,	0x200,
	&GUID_RX_AXIS,		0xbc,	0x80ffff03,	0x200,
	&GUID_RY_AXIS,		0xc0,	0x80ffff03,	0x200,
	&GUID_RZ_AXIS,		0xc4,	0x80ffff03,	0x200,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x200,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x200,
	&GUID_X_AXIS,		0xd0,	0x80ffff03,	0x300,
	&GUID_Y_AXIS,		0xd4,	0x80ffff03,	0x300,
	&GUID_Z_AXIS,		0xd8,	0x80ffff03,	0x300,
	&GUID_RX_AXIS,		0xdc,	0x80ffff03,	0x300,
	&GUID_RY_AXIS,		0xe0,	0x80ffff03,	0x300,
	&GUID_RZ_AXIS,		0xe4,	0x80ffff03,	0x300,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x300,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x300,
	&GUID_X_AXIS,		0xf0,	0x80ffff03,	0x400,
	&GUID_Y_AXIS,		0xf4,	0x80ffff03,	0x400,
	&GUID_Z_AXIS,		0xf8,	0x80ffff03,	0x400,
	&GUID_RX_AXIS,		0xfc,	0x80ffff03,	0x400,
	&GUID_RY_AXIS,		0x00,	0x80ffff03,	0x400,
	&GUID_RZ_AXIS,		0x04,	0x80ffff03,	0x400,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x400,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x400,
} ;

// ジョイスティック２デバイスデータフォーマット
D_DIDATAFORMAT C_DFDIJOYSTICK2 =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x01,	// dwFlags
	272,	// dwDataSize
	164,	// dwNumObjs
	C_ODFDIJOYSTICK2, // rgodf
} ;


GUID GUIDDIMOUSE2[3] =
{
	0xa36d02e0, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e1, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e2, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
} ;

D_DIOBJECTDATAFORMAT C_ODFDIMOUSE2[11] =
{
	&GUIDDIMOUSE2[0],	0x00, 0x00ffff03, 0x000,
	&GUIDDIMOUSE2[1],	0x04, 0x00ffff03, 0x000,
	&GUIDDIMOUSE2[2],	0x08, 0x80ffff03, 0x000,
	NULL,				0x0c, 0x00ffff0c, 0x000,
	NULL,				0x0d, 0x00ffff0c, 0x000,
	NULL,				0x0e, 0x80ffff0c, 0x000,
	NULL, 				0x0f, 0x80ffff0c, 0x000,
	NULL, 				0x10, 0x80ffff0c, 0x000,
	NULL, 				0x11, 0x80ffff0c, 0x000,
	NULL, 				0x12, 0x80ffff0c, 0x000,
	NULL, 				0x13, 0x80ffff0c, 0x000,
} ;

D_DIDATAFORMAT C_DFDIMOUSE2 =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	2,		// dwFlags
	20,		// dwDataSize
	11,		// dwNumObjs
	C_ODFDIMOUSE2,
} ;


GUID GUIDDIMOUSE[3] =
{
	0xa36d02e0, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e1, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e2, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
} ;

D_DIOBJECTDATAFORMAT C_ODFDIMOUSE[7] =
{
	&GUIDDIMOUSE[0],	0x00, 0x00ffff03, 0x000,
	&GUIDDIMOUSE[1],	0x04, 0x00ffff03, 0x000,
	&GUIDDIMOUSE[2],	0x08, 0x80ffff03, 0x000,
	NULL,				0x0c, 0x00ffff0c, 0x000,
	NULL,				0x0d, 0x00ffff0c, 0x000,
	NULL,				0x0e, 0x80ffff0c, 0x000,
	NULL, 				0x0f, 0x80ffff0c, 0x000,
} ;

D_DIDATAFORMAT C_DFDIMOUSE =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	2,		// dwFlags
	16,		// dwDataSize
	7,		// dwNumObjs
	C_ODFDIMOUSE,
} ;

const static unsigned short __KeyMap[][3 /* 0:Windows仮想キーコード  1:DirectInputキーコード  2:DirectInput使用時に使用するか否か */ ] =
{
	'A',			D_DIK_A,			0,	// Ａキー
	'B',			D_DIK_B,			0,	// Ｂキー
	'C',			D_DIK_C,			0,	// Ｃキー
	'D',			D_DIK_D,			0,	// Ｄキー
	'E',			D_DIK_E,			0,	// Ｅキー
	'F',			D_DIK_F,			0,	// Ｆキー
	'G',			D_DIK_G,			0,	// Ｇキー
	'H',			D_DIK_H,			0,	// Ｈキー
	'I',			D_DIK_I,			0,	// Ｉキー
	'J',			D_DIK_J,			0,	// Ｊキー
	'K',			D_DIK_K,			0,	// Ｋキー
	'L',			D_DIK_L,			0,	// Ｌキー
	'M',			D_DIK_M,			0,	// Ｍキー
	'N',			D_DIK_N,			0,	// Ｎキー
	'O',			D_DIK_O,			0,	// Ｏキー
	'P',			D_DIK_P,			0,	// Ｐキー
	'Q',			D_DIK_Q,			0,	// Ｑキー
	'R',			D_DIK_R,			0,	// Ｒキー
	'S',			D_DIK_S,			0,	// Ｓキー
	'T',			D_DIK_T,			0,	// Ｔキー
	'U',			D_DIK_U,			0,	// Ｕキー
	'V',			D_DIK_V,			0,	// Ｖキー
	'W',			D_DIK_W,			0,	// Ｗキー
	'X',			D_DIK_X,			0,	// Ｘキー
	'Y',			D_DIK_Y,			0,	// Ｙキー
	'Z',			D_DIK_Z,			0,	// Ｚキー
	'0',			D_DIK_0,			0,	// ０キー
	'1',			D_DIK_1,			0,	// １キー
	'2',			D_DIK_2,			0,	// ２キー
	'3',			D_DIK_3,			0,	// ３キー
	'4',			D_DIK_4,			0,	// ４キー
	'5',			D_DIK_5,			0,	// ５キー
	'6',			D_DIK_6,			0,	// ６キー
	'7',			D_DIK_7,			0,	// ７キー
	'8',			D_DIK_8,			0,	// ８キー
	'9',			D_DIK_9,			0,	// ９キー

	VK_LEFT,		D_DIK_LEFT,			1,	// 左キー
	VK_UP,			D_DIK_UP,			1,	// 上キー
	VK_RIGHT,		D_DIK_RIGHT,		1,	// 右キー
	VK_DOWN,		D_DIK_DOWN,			1,	// 下キー

	VK_OEM_PLUS,	D_DIK_SEMICOLON,	0,	// ；キー
	VK_OEM_1,		D_DIK_COLON,		0,	// ：キー
	VK_OEM_4,		D_DIK_LBRACKET,		0,	// ［キー
	VK_OEM_6,		D_DIK_RBRACKET,		0,	// ］キー
	VK_OEM_3,		D_DIK_AT,			0,	// ＠キー
	VK_OEM_102,		D_DIK_BACKSLASH,	0,	// ＼キー
	VK_OEM_COMMA,	D_DIK_COMMA,		0,	// ，キー
	VK_OEM_7,		D_DIK_PREVTRACK,	0,	// ＾きー

	VK_OEM_MINUS,	D_DIK_MINUS,		0,	// −キー
	VK_OEM_5,		D_DIK_YEN,			0,	// ￥キー
	VK_OEM_PERIOD,	D_DIK_PERIOD,		0,	// ．キー
	VK_OEM_2,		D_DIK_SLASH,		0,	// ／キー

	VK_DELETE,		D_DIK_DELETE,		0,	// デリートキー

	VK_ESCAPE,		D_DIK_ESCAPE,		0,	// エスケープキー
	VK_SPACE,		D_DIK_SPACE,		0,	// スペースキー

	VK_BACK, 		D_DIK_BACK,			0,	// バックスペースキー
	VK_TAB, 		D_DIK_TAB,			0,	// タブキー
	VK_RETURN,		D_DIK_RETURN,		0,	// エンターキー

	VK_NUMLOCK,		D_DIK_NUMLOCK,		1,	// テンキーNumLockキー
	VK_NUMPAD0,		D_DIK_NUMPAD0,		1,	// テンキー０
	VK_NUMPAD1,		D_DIK_NUMPAD1,		1,	// テンキー１
	VK_NUMPAD2,		D_DIK_NUMPAD2,		1,	// テンキー２
	VK_NUMPAD3,		D_DIK_NUMPAD3,		1,	// テンキー３
	VK_NUMPAD4,		D_DIK_NUMPAD4,		1,	// テンキー４
	VK_NUMPAD5,		D_DIK_NUMPAD5,		1,	// テンキー５
	VK_NUMPAD6,		D_DIK_NUMPAD6,		1,	// テンキー６
	VK_NUMPAD7,		D_DIK_NUMPAD7,		1,	// テンキー７
	VK_NUMPAD8,		D_DIK_NUMPAD8,		1,	// テンキー８
	VK_NUMPAD9,		D_DIK_NUMPAD9,		1,	// テンキー９
	VK_MULTIPLY,	D_DIK_MULTIPLY,		1,	// テンキー＊キー
	VK_ADD,			D_DIK_ADD,			1,	// テンキー＋キー
	VK_SUBTRACT,	D_DIK_SUBTRACT,		1,	// テンキー−キー
	VK_DECIMAL,		D_DIK_DECIMAL,		1,	// テンキー．キー
	VK_DIVIDE,		D_DIK_DIVIDE,		1,	// テンキー／キー

	VK_RETURN,		D_DIK_NUMPADENTER,	1,	// テンキーのエンターキー
//	VK_NUMPADENTER,	D_DIK_NUMPADENTER,	1,	// テンキーのエンターキー

	VK_LSHIFT,		D_DIK_LSHIFT,		1,	// 左シフトキー
	VK_RSHIFT,		D_DIK_RSHIFT,		1,	// 右シフトキー
	VK_LCONTROL,	D_DIK_LCONTROL,		1,	// 左コントロールキー
	VK_RCONTROL,	D_DIK_RCONTROL,		1,	// 右コントロールキー
	VK_PRIOR,		D_DIK_PGUP,			1,	// ＰａｇｅＵＰキー
	VK_NEXT,		D_DIK_PGDN,			1,	// ＰａｇｅＤｏｗｎキー
	VK_END,			D_DIK_END,			1,	// エンドキー
	VK_HOME,		D_DIK_HOME,			1,	// ホームキー
	VK_INSERT,		D_DIK_INSERT,		1,	// インサートキー

	VK_LMENU,		D_DIK_LALT,			1,	// 左ＡＬＴキー
	VK_RMENU,		D_DIK_RALT,			1,	// 右ＡＬＴキー
	VK_SCROLL,		D_DIK_SCROLL,		1,	// ScrollLockキー
	VK_APPS,		D_DIK_APPS,			1,	// アプリケーションメニューキー
	VK_CAPITAL,		D_DIK_CAPSLOCK,		1,	// CaspLockキー
	VK_SNAPSHOT,	D_DIK_SYSRQ,		1,	// PrintScreenキー
	VK_PAUSE,		D_DIK_PAUSE,		1,	// PauseBreakキー
	VK_LWIN,		D_DIK_LWIN,			1,	// 左Ｗｉｎキー
	VK_RWIN,		D_DIK_RWIN,			1,	// 右Ｗｉｎキー

	VK_OEM_ENLW,	D_DIK_KANJI,		1,	// 半角／全角キー
	VK_CONVERT,		D_DIK_CONVERT,		1,	// 変換キー
	VK_NONCONVERT,	D_DIK_NOCONVERT,	1,	// 無変換キー
	VK_OEM_COPY,	D_DIK_KANA,			0,	// カナキー

	VK_F1,			D_DIK_F1,			1,	// Ｆ１キー
	VK_F2,			D_DIK_F2,			1,	// Ｆ２キー
	VK_F3,			D_DIK_F3,			1,	// Ｆ３キー
	VK_F4,			D_DIK_F4,			1,	// Ｆ４キー
	VK_F5,			D_DIK_F5,			1,	// Ｆ５キー
	VK_F6,			D_DIK_F6,			1,	// Ｆ６キー
	VK_F7,			D_DIK_F7,			1,	// Ｆ７キー
	VK_F8,			D_DIK_F8,			1,	// Ｆ８キー
	VK_F9,			D_DIK_F9,			1,	// Ｆ９キー
	VK_F10,			D_DIK_F10,			1,	// Ｆ１０キー
	VK_F11,			D_DIK_F11,			1,	// Ｆ１１キー
	VK_F12,			D_DIK_F12,			1,	// Ｆ１２キー

	0xffff,			0xffff,
} ;

#define XINPUT_TO_DIRECTINPUT_BUTTONNUM		(10)
WORD XInputButtonToDirectInputButtonNo[ XINPUT_TO_DIRECTINPUT_BUTTONNUM ] =
{
	D_XINPUT_GAMEPAD_A,
	D_XINPUT_GAMEPAD_B,
	D_XINPUT_GAMEPAD_X,
	D_XINPUT_GAMEPAD_Y,
	D_XINPUT_GAMEPAD_LEFT_SHOULDER,
	D_XINPUT_GAMEPAD_RIGHT_SHOULDER,
	D_XINPUT_GAMEPAD_BACK,
	D_XINPUT_GAMEPAD_START,
	D_XINPUT_GAMEPAD_LEFT_THUMB,
	D_XINPUT_GAMEPAD_RIGHT_THUMB
} ;

// 関数プロトタイプ宣言 -------------------------------------------------------

// ジョイパッド列挙用コールバック関数
		BOOL FAR PASCAL EnumJoypadProc( const D_DIDEVICEINSTANCEW *pdinst , LPVOID pvRef ) ;

// プログラム------------------------------------------------------------------

BOOL CALLBACK EffectEnumCallBack( const D_DIEFFECTINFOA * /*Info*/, void * /*Data*/ )
{
	return D_DIENUM_CONTINUE ;
}

// ジョイパッド列挙用コールバック関数
BOOL FAR PASCAL EnumJoypadProc( const D_DIDEVICEINSTANCEW *pdinst , LPVOID /*pvRef*/ )
{
	HRESULT hr ;
	D_IDirectInputDevice7 *Joystick ;
	D_DIPROPRANGE diprg ;
	D_DIPROPDWORD dipdw ;
	HANDLE Event ;
	INPUTPADDATA *pad ;
	int i ;

	SETUP_WIN_API

	pad = &InputSysData.Pad[ InputSysData.PadNum ] ;

	// ジョイスティックデバイスの作成
	pad->PF.XInputDeviceNo = -1 ;
	if( InputSysData.PF.UseDirectInput8Flag == TRUE )
	{
		hr = InputSysData.PF.DirectInputObject->CreateDevice( pdinst->guidInstance, ( D_IDirectInputDevice ** )&Joystick , NULL ) ;
	}
	else
	{
		hr = InputSysData.PF.DirectInputObject->CreateDeviceEx( pdinst->guidInstance, IID_IDIRECTINPUTDEVICE7, ( void ** )&Joystick , NULL ) ;
	}
	if( hr != D_DI_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x65\x51\x9b\x52\xc5\x88\x6e\x7f\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"入力装置の取得に失敗しました\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// 情報表示
	{
		D_DIDEVICEINSTANCEW State ;
		WCHAR tszInstanceNameUPR[ MAX_PATH ] ;

		_MEMSET( &State, 0, sizeof( State ) ) ;
		State.dwSize = sizeof( State ) ;
		Joystick->GetDeviceInfo( &State ) ;

		// 小文字を大文字にしたコントローラ名を作成
		_WCSCPY( tszInstanceNameUPR, State.tszInstanceName ) ;
		_WCSUPR( tszInstanceNameUPR ) ;

		// Xbox360 コントローラ又は Xbox One コントローラの場合で、XInputで検出されていたら弾く
		if( _WCSSTR( tszInstanceNameUPR, L"XBOX 360 FOR WINDOWS" ) != NULL ||
			_WCSSTR( tszInstanceNameUPR, L"XBOX ONE FOR WINDOWS" ) != NULL )
		{
			for( i = 0 ; i < InputSysData.PadNum ; i ++ )
			{
				if( InputSysData.Pad[ i ].PF.XInputDeviceNo >= 0 )
					break ;
			}
			if( i != InputSysData.PadNum )
			{
				Joystick->Release() ;
				Joystick = NULL ;
				return D_DIENUM_CONTINUE ;
			}
		}

		DXST_LOGFILE_ADDUTF16LE( "\x65\x51\x9b\x52\xc5\x88\x6e\x7f\x92\x30\x8b\x89\x64\x30\x51\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"入力装置を見つけました\n" @*/ ) ;

		_WCSCPY_S( pad->PF.InstanceName, sizeof( pad->PF.InstanceName ), State.tszInstanceName );
		_WCSCPY_S( pad->PF.ProductName,  sizeof( pad->PF.ProductName  ), State.tszProductName  );

		DXST_LOGFILE_TABADD ;
		DXST_LOGFILEFMT_ADDW(( L"Device Instance Name : %s", State.tszInstanceName )) ;

		DXST_LOGFILEFMT_ADDW(( L"Device Product Name  : %s", State.tszProductName )) ;
		DXST_LOGFILE_TABSUB ;
	}
	
	// ジョイスパッドのデータ形式を設定
//	hr = Joystick->SetDataFormat( &c_dfDIJoystick ) ;
	hr = Joystick->SetDataFormat( &C_DFDIJOYSTICK ) ;
	if( hr != D_DI_OK )
	{
		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x01\x30\xc8\x53\x6f\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスではありませんでした、又は設定に失敗しました\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// ジョイパッドの協調レベルを設定する
	if( InputSysData.NoUseVibrationFlag )
	{
		hr = Joystick->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
	}
	else
	{
		hr = Joystick->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_FOREGROUND | D_DISCL_EXCLUSIVE ) ;
		if( hr != D_DI_OK )
		{
			hr = Joystick->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
		}
	}
	if( hr != D_DI_OK )
	{
//		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスの協調レベルの設定に失敗しました\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// Ｘの範囲を設定
	diprg.diph.dwSize		= sizeof( diprg ) ;
	diprg.diph.dwHeaderSize	= sizeof( diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_X ;
	diprg.diph.dwHow		= D_DIPH_BYOFFSET ;
	diprg.lMin				= -DIRINPUT_MAX ;
	diprg.lMax				= +DIRINPUT_MAX ;
	hr = Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x38\xff\xf8\x8e\xa2\x95\xc2\x4f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスのＸ軸関係の設定に失敗しました\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// Ｙの範囲を設定
	diprg.diph.dwObj		= D_DIJOFS_Y ;
	hr = Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x39\xff\xf8\x8e\xa2\x95\xc2\x4f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスのＹ軸関係の設定に失敗しました\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// Ｚの範囲を設定
	diprg.diph.dwObj		= D_DIJOFS_Z ;
	Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;

	// ＲＸＲＹＲＺの範囲を設定
	diprg.diph.dwObj		= D_DIJOFS_RX ;
	Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_RY ;
	Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_RZ ;
	Joystick->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;

	// 無効ゾーンのセット
	pad->DeadZone = DEADZONE ;
	pad->DeadZoneD = DEADZONE_D ;

	// Ｘの無効ゾーンを設定
	dipdw.diph.dwSize		= sizeof( dipdw ) ;
	dipdw.diph.dwHeaderSize	= sizeof( dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_X ;
	dipdw.diph.dwHow		= D_DIPH_BYOFFSET ;
	dipdw.dwData			= DEADZONE_DIRECTINPUT( pad->DeadZone ) ;
	hr = Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x38\xff\xf8\x8e\x6e\x30\x21\x71\xb9\x52\xbe\x30\xfc\x30\xf3\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスのＸ軸の無効ゾーンの設定に失敗しました\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// Ｙの無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_Y ;
	hr = Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joystick->Release() ;
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x39\xff\xf8\x8e\x6e\x30\x21\x71\xb9\x52\xbe\x30\xfc\x30\xf3\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドデバイスのＹ軸の無効ゾーンの設定に失敗しました\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// Ｚの無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_Z ;
	Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// ＲＸＲＹＲＺの無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_RX ;
	Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_RY ;
	Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_RZ ;
	Joystick->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// フォースフェードバックオブジェクトの作成(PSパッド想定)
	{
		DWORD dwAxes[2] = { D_DIJOFS_X, D_DIJOFS_Y } ;
		LONG lDirection[2] = { 0, 0 } ;

//		Joystick->EnumEffects( EffectEnumCallBack, NULL, D_DIEFT_ALL ) ;

		// 右側モーター用(手元のパッドでは何故か GUID_SINE エフェクトは右モーターになってた)
		{
			D_DIPERIODIC diPeriodic ;	// タイプ固有パラメータ
			D_DIENVELOPE diEnvelope ;	// エンベロープ
			D_DIEFFECT diEffect ;		// 汎用パラメータ

			_MEMSET( &diEffect, 0, sizeof( diEffect ) ) ;
			_MEMSET( &diEnvelope, 0, sizeof( diEnvelope ) ) ;
			_MEMSET( &diPeriodic, 0, sizeof( diPeriodic ) ) ;

			// 次にタイプ固有パラメータを初期化する。次の例のような値を設定すると、1/20 秒周期の全力周期的エフェクトを生成することになる。

			diPeriodic.dwMagnitude	= D_DI_FFNOMINALMAX ;
			diPeriodic.lOffset		= 0;
			diPeriodic.dwPhase		= 0;
			diPeriodic.dwPeriod		= (DWORD) (1.5 * D_DI_SECONDS);
			// チェーンソーのモーターが始動しようとして、短い間咳のような騒音を出し、次第にそれが止んでいくというエフェクトを得るために、エンベロープに半秒間のアタック時間と 1 秒間のフェード時間を設定する。こうして、短期間の維持期間値が得られる。

			diEnvelope.dwSize			= sizeof(D_DIENVELOPE);
			diEnvelope.dwAttackLevel	= 0;
			diEnvelope.dwAttackTime		= (DWORD) (0.5 * D_DI_SECONDS);
			diEnvelope.dwFadeLevel		= 0;
			diEnvelope.dwFadeTime		= (DWORD) (1.0 * D_DI_SECONDS);
			// 次に、基本的エフェクトパラメータを設定する。これらのパラメータには、方向とデバイス オブジェクト (ボタンと軸) の識別方法を決定するフラグ、エフェクトのサンプル周期とゲイン、およびさきほど準備した他のデータへのポインタが含まれる。さらに、ジョイスティックの発射ボタンにエフェクトを割り付けて、ボタンを押せばエフェクトが自動的に再生するようにする。

			diEffect.dwSize						= sizeof(D_DIEFFECT);
			diEffect.dwFlags					= D_DIEFF_POLAR | D_DIEFF_OBJECTOFFSETS;
			diEffect.dwDuration					= INFINITE ;
 
			diEffect.dwSamplePeriod				= 0;               // デフォルト値
			diEffect.dwGain						= D_DI_FFNOMINALMAX;         // スケーリングなし
			diEffect.dwTriggerButton			= D_DIEB_NOTRIGGER ; // D_DIJOFS_BUTTON( 0 );
			diEffect.dwTriggerRepeatInterval	= 0;
			diEffect.cAxes						= 2;
			diEffect.rgdwAxes					= dwAxes;
			diEffect.rglDirection				= &lDirection[0];
			diEffect.lpEnvelope					= &diEnvelope;
			diEffect.cbTypeSpecificParams		= sizeof(diPeriodic);
			diEffect.lpvTypeSpecificParams		= &diPeriodic;
			// セットアップはここまで。やっとエフェクトを生成できる。

			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PF.DIEffect = NULL ;
			if( InputSysData.NoUseVibrationFlag == FALSE )
			{
				Joystick->CreateEffect(
								 GUID_SINE,     // 列挙からの GUID
								 &diEffect,      // データの場所
								 &pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PF.DIEffect,  // インターフェイス ポインタを置く場所
								 NULL ) ;          // 集合化なし
			}
		}

		// 左側モーター設定(手元のパッドでは何故か GUID_CONSTANTFORCE エフェクトは右モーターになってた)
		{
			DWORD    rgdwAxes[2] = { D_DIJOFS_X, D_DIJOFS_Y };
			LONG     rglDirection[2] = { 0, 0 };
			D_DICONSTANTFORCE cf = { D_DI_FFNOMINALMAX };

			D_DIEFFECT eff;
			_MEMSET( &eff, 0, sizeof(eff) );
			eff.dwSize                  = sizeof(D_DIEFFECT);
			eff.dwFlags                 = D_DIEFF_CARTESIAN | D_DIEFF_OBJECTOFFSETS;
			eff.dwDuration              = INFINITE;
			eff.dwSamplePeriod          = 0;
			eff.dwGain                  = D_DI_FFNOMINALMAX;
			eff.dwTriggerButton         = D_DIEB_NOTRIGGER;
			eff.dwTriggerRepeatInterval = 0;
			eff.cAxes                   = 2;
			eff.rgdwAxes                = rgdwAxes;
			eff.rglDirection            = rglDirection;
			eff.lpEnvelope              = 0;
			eff.cbTypeSpecificParams    = sizeof(D_DICONSTANTFORCE);
			eff.lpvTypeSpecificParams   = &cf;
			eff.dwStartDelay            = 0;

			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PF.DIEffect = NULL ;
			if( InputSysData.NoUseVibrationFlag == FALSE )
			{
				hr = Joystick->CreateEffect(
								 GUID_CONSTANTFORCE,	// 列挙からの GUID
								 &eff,					// データの場所
								 &pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PF.DIEffect,  // インターフェイス ポインタを置く場所
								 NULL					// 集合化なし
				) ;
				if( hr != D_DI_OK )
				{
					DXST_LOGFILE_ADDUTF16LE(( "\x68\x54\x1f\x67\x84\x76\xa8\x30\xd5\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x02\x30\x0a\x00\x00"/*@ L"周期的エフェクトの作成に失敗しました。\n" @*/ )) ;
				}
			}
		}

		// 振動関係の設定の初期化を行う
		for( i = 0 ; i < DINPUTPAD_MOTOR_NUM ; i ++ )
		{
			pad->Effect[ i ].PlayFlag  = FALSE ;
			pad->Effect[ i ].Power     = D_DI_FFNOMINALMAX ;
			pad->Effect[ i ].PlayState = FALSE ;
			pad->Effect[ i ].SetPower  = 0 ;
		}
//		pad->EffectNotCommonFlag    = FALSE ;
//		pad->EffectCommon.PlayFlag	= FALSE ;
//		pad->EffectCommon.Power		= D_DI_FFNOMINALMAX ;
//		pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PlayFlag	= FALSE ;
//		pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power		= D_DI_FFNOMINALMAX ;
//		pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PlayFlag	= FALSE ;
//		pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power		= D_DI_FFNOMINALMAX ;
//		pad->EffectPlayStateLeft	= FALSE ;
//		pad->EffectPlayStateRight	= FALSE ;
	}

	// 変化通知用のイベントを作成・アタッチする
	Event = WinAPIData.Win32Func.CreateEventAFunc( NULL, TRUE, FALSE, NULL ) ;
	hr = Joystick->SetEventNotification( Event ) ;
	if( hr != D_DI_POLLEDDEVICE && hr != D_DI_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xa4\x30\xd9\x30\xf3\x30\xc8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドイベントのセットアップが失敗しました\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// デバイスのアクセス権を取得する
	if( Joystick->Acquire() != D_DI_OK )
	{
		// デバイスの取得に失敗したら協調レベルを弱くして再度試す
		Joystick->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
		if( Joystick->Acquire() != D_DI_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x29\x6a\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドのアクセス権の取得に失敗しました\n" @*/ ) ;
			return D_DIENUM_CONTINUE ;
		}
	}

	// イベントハンドルを保存
	pad->PF.Event = Event ;

	// ジョイパッドデバイスのアドレスを保存
	pad->PF.Device = Joystick ;

	// ステータスを初期化
	_MEMSET( &pad->State, 0, sizeof( D_DIJOYSTATE ) ) ;

	// ジョイパッドの数を増やす
	InputSysData.PadNum ++ ;

	DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\xfd\x8f\xa0\x52\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドの追加は正常に終了しました\n" @*/ ) ;

	// 終了
	return ( InputSysData.PadNum != MAX_JOYPAD_NUM ) ? D_DIENUM_CONTINUE : D_DIENUM_STOP ;
}


// 入力システムを初期化する処理の環境依存処理
extern int InitializeInputSystem_PF_Timing0( void )
{
	HRESULT hr = 0 ;
	int i, j ;
	int KeyToJoypadInput[ MAX_JOYPAD_NUM ][ 32 ][ 4 ] ;
	static int NowInitialize = FALSE ;

	// 初期化判定
	if( InputSysData.PF.DirectInputObject != NULL )
	{
		return 0 ;
	}

	// 既に初期化処理が開始されている場合も何もせずに終了
	if( NowInitialize )
	{
		return 0 ;
	}

	// 初期化中フラグを立てる
	NowInitialize = TRUE ;

	// ゼロ初期化
	int	InitializeFlag									= InputSysData.InitializeFlag ;
	int NoUseXInputFlag									= InputSysData.PF.NoUseXInputFlag ;
	int NoUseDirectInputFlag							= InputSysData.PF.NoUseDirectInputFlag ;
	int UseDirectInputFlag								= InputSysData.PF.UseDirectInputFlag ;
	int KeyboardNotUseDirectInputFlag					= InputSysData.PF.KeyboardNotUseDirectInputFlag ;
	int KeyExclusiveCooperativeLevelFlag				= InputSysData.PF.KeyExclusiveCooperativeLevelFlag ;
	int KeyToJoypadInputInitializeFlag					= InputSysData.KeyToJoypadInputInitializeFlag ;
	int NoUseVibrationFlag								= InputSysData.NoUseVibrationFlag ;
	_MEMCPY( KeyToJoypadInput, InputSysData.KeyToJoypadInput, sizeof( InputSysData.KeyToJoypadInput ) ) ;
	_MEMSET( &InputSysData, 0, sizeof( InputSysData ) ) ; 
	InputSysData.PF.NoUseXInputFlag						= NoUseXInputFlag ;
	InputSysData.PF.NoUseDirectInputFlag				= NoUseDirectInputFlag ;
	InputSysData.PF.UseDirectInputFlag					= UseDirectInputFlag ;
	InputSysData.PF.KeyboardNotUseDirectInputFlag		= KeyboardNotUseDirectInputFlag ;
	InputSysData.PF.KeyExclusiveCooperativeLevelFlag	= KeyExclusiveCooperativeLevelFlag ;
	InputSysData.KeyToJoypadInputInitializeFlag			= KeyToJoypadInputInitializeFlag ;
	InputSysData.NoUseVibrationFlag						= NoUseVibrationFlag ;
	InputSysData.InitializeFlag							= InitializeFlag ;
	_MEMCPY( InputSysData.KeyToJoypadInput, KeyToJoypadInput, sizeof( InputSysData.KeyToJoypadInput ) ) ;

	// 強制的に DirectInput8 を使用するようにする
	InputSysData.PF.UseDirectInput8Flag = TRUE ;

	// ＤｉｒｅｃｔＩｎｐｕｔ を使用するかどうかで処理を分岐
START:
	if( InputSysData.PF.NoUseDirectInputFlag == FALSE )
	{
		const wchar_t *XInputDllFileName[] = 
		{
			L"xinput1_4.dll",
			L"xinput1_3.dll",
			L"xinput9_1_0.dll",
			NULL
		} ;

		// DirectInput を使用する場合

		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\xa2\x95\xc2\x4f\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x0a\x00\x00"/*@ L"DirectInput関係初期化処理\n" @*/ ) ;
		DXST_LOGFILE_TABADD ;

		// ＸＩｎｐｕｔを使用しないフラグが立っていなかったらＸＩｎｐｕｔのＤＬＬを読み込む
		if( InputSysData.PF.NoUseXInputFlag == FALSE )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"XInput DLL の読み込み中... " @*/ ) ;
			for( i = 0 ; XInputDllFileName[ i ] != NULL ; i ++ )
			{
				InputSysData.PF.XInputDLL = LoadLibraryW( XInputDllFileName[ i ] ) ;
				if( InputSysData.PF.XInputDLL != NULL )
					break ;
			}

			if( InputSysData.PF.XInputDLL == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x01\x30\x58\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x6f\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"失敗、XInputは使用しません\n" @*/ ) ;
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

				InputSysData.PF.XInputGetStateFunc = ( DWORD ( WINAPI * )( DWORD, D_XINPUT_STATE*     ) )GetProcAddress( InputSysData.PF.XInputDLL, "XInputGetState" ) ;
				InputSysData.PF.XInputSetStateFunc = ( DWORD ( WINAPI * )( DWORD, D_XINPUT_VIBRATION* ) )GetProcAddress( InputSysData.PF.XInputDLL, "XInputSetState" ) ;
			}
		}

		// ＤｉｒｅｃｔＩｎｐｕｔオブジェクトを作成する
		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x37\x00\x20\x00\x6e\x30\xd6\x53\x97\x5f\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"DirectInput7 の取得中... " @*/ ) ;

		if( InputSysData.PF.UseDirectInput8Flag == FALSE )
		{
			hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTINPUT , NULL, CLSCTX_INPROC_SERVER, IID_IDIRECTINPUT7, ( LPVOID * )&InputSysData.PF.DirectInputObject ) ;
			if( !FAILED( hr ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
				DXST_LOGFILE_TABADD ;
				DXST_LOGFILE_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"引き続き初期化処理... " @*/ ) ;
				hr = InputSysData.PF.DirectInputObject->Initialize( WinAPIData.Win32Func.GetModuleHandleWFunc( NULL ), 0x700 ) ;
				if( FAILED( hr ) ) 
				{
					DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"初期化に失敗。" @*/ ) ;
					InputSysData.PF.DirectInputObject->Release() ;
					InputSysData.PF.DirectInputObject = NULL ;
					goto ER1 ;
				}
				DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"初期化成功\n" @*/ ) ;
				DXST_LOGFILE_TABSUB ;
			}
		}

		if( InputSysData.PF.DirectInputObject == NULL )
		{
			if( InputSysData.PF.UseDirectInput8Flag == FALSE )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x02\x30\x0a\x00\x00"/*@ L"取得に失敗。\n" @*/ ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x20\x00\x30\x00\x78\x00\x25\x00\x78\x00\x0a\x00\x00"/*@ L"エラーコード 0x%x\n" @*/, hr ));
			}
ER1:
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x38\x00\x20\x00\x6e\x30\xd6\x53\x97\x5f\x92\x30\x66\x8a\x7f\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x00"/*@ L"DirectInput8 の取得を試みます..." @*/ ) ;
			hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTINPUT8 , NULL, CLSCTX_INPROC_SERVER, IID_IDIRECTINPUT8, ( LPVOID * )&InputSysData.PF.DirectInputObject ) ;
			if( !FAILED( hr ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
				DXST_LOGFILE_TABADD ;
				DXST_LOGFILE_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"引き続き初期化処理... " @*/ ) ;
				hr = InputSysData.PF.DirectInputObject->Initialize( WinAPIData.Win32Func.GetModuleHandleWFunc( NULL ) , 0x800 ) ;
				if( FAILED( hr ) ) 
				{
					DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"初期化に失敗。" @*/ ) ;
					InputSysData.PF.DirectInputObject->Release() ;
					InputSysData.PF.DirectInputObject = NULL ;
					goto ER2;
				}
				DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"初期化成功\n" @*/ ) ;
				InputSysData.PF.UseDirectInput8Flag = TRUE;
				DXST_LOGFILE_TABSUB ;
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"取得に失敗。" @*/ ) ;
ER2:
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x02\x30\x0a\x00\x00"/*@ L"DirectInput を使用しません。\n" @*/ ) ;
				DXST_LOGFILE_TABSUB ;

				if( InputSysData.PF.DirectInputObject ) InputSysData.PF.DirectInputObject->Release() ;
				InputSysData.PF.DirectInputObject = NULL;
				InputSysData.PF.NoUseDirectInputFlag = TRUE;
//				return -1 ;
				goto START;
			}
		}

		// メインウインドウをアクティブにする
		// WinAPIData.Win32Func.BringWindowToTopFunc( NS_GetMainWindowHandle() ) ;

		// アクティブになるまで待つ
		NS_ProcessMessage() ;

		// ジョイパッドデバイスを初期化する
		SetupJoypad() ;

		// マウスデバイスを初期化する
		InputSysData.PF.MouseDeviceObject = NULL ;
		{
			// マウスデバイスを作成する
			DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xa6\x30\xb9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"マウスデバイスの初期化... " @*/ ) ; 
			if( InputSysData.PF.UseDirectInput8Flag == TRUE )
			{
				hr = InputSysData.PF.DirectInputObject->CreateDevice( GUID_SYSMOUSE, ( D_IDirectInputDevice ** )&InputSysData.PF.MouseDeviceObject , NULL ) ;
			}
			else
			{
				hr = InputSysData.PF.DirectInputObject->CreateDeviceEx( GUID_SYSMOUSE, IID_IDIRECTINPUTDEVICE7, ( void ** )&InputSysData.PF.MouseDeviceObject , NULL ) ;
			}
			if( hr != D_DI_OK )
			{
				InputSysData.PF.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;
//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"マウスデバイスの取得に失敗しました" ) ;
			}

			// マウスデバイスのデータ形式を設定する
	//		hr = InputSysData.PF.MouseDeviceObject->SetDataFormat( &c_dfDIMouse2 ) ;
			hr = InputSysData.PF.MouseDeviceObject->SetDataFormat( &C_DFDIMOUSE2 ) ;
			if( hr != D_DI_OK )
			{
				InputSysData.PF.MouseDeviceObject->Release() ;
				InputSysData.PF.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;

//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"マウスデバイスのデータ形式の設定に失敗しました" ) ;
			}

			// マウスの協調レベルを設定する
	//		hr = InputSysData.PF.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
			hr = InputSysData.PF.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
	//		hr = InputSysData.PF.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_FOREGROUND ) ;
 			if( hr != D_DI_OK ) 
			{
				InputSysData.PF.MouseDeviceObject->Release() ;
				InputSysData.PF.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;

//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"マウスデバイスの協調レベルの設定に失敗しました" ) ;
			}

			DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"初期化成功\n" @*/ ) ;

MOUSEDEVICEINITEND:
			if( InputSysData.PF.MouseDeviceObject == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xa6\x30\xb9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マウスデバイスの取得に失敗しました\n" @*/ ) ; 
			}

			InputSysData.KeyInputGetTime = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		}

		// キーボードデバイスを初期化する
		InputSysData.PF.KeyboardDeviceObject = NULL ;
		if( InputSysData.PF.KeyboardNotUseDirectInputFlag == FALSE )
		{
			HANDLE Event ;

			// キーボードデバイスを作成する
			DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"キーボードデバイスの初期化... " @*/ ) ; 
			if( InputSysData.PF.UseDirectInput8Flag == TRUE )
			{
				hr = InputSysData.PF.DirectInputObject->CreateDevice( GUID_SYSKEYBOARD, ( D_IDirectInputDevice ** )&InputSysData.PF.KeyboardDeviceObject , NULL ) ;
			}
			else
			{
				hr = InputSysData.PF.DirectInputObject->CreateDeviceEx( GUID_SYSKEYBOARD, IID_IDIRECTINPUTDEVICE7, ( void ** )&InputSysData.PF.KeyboardDeviceObject , NULL ) ;
			}
			if( hr != D_DI_OK )
			{
//				if( InputSysData.PF.MouseDeviceObject )
//				{
//					InputSysData.PF.MouseDeviceObject->Release() ;
//					InputSysData.PF.MouseDeviceObject = NULL ;
//				}
				goto KEYBOARDDEVICEINITEND ;

//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;
//
//				NowInitialize = FALSE ;
//				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"キーボードデバイスの取得に失敗しました" @*/ ) ;
			}

			// キーボードデバイスのデータ形式を設定する
	//		hr = InputSysData.PF.KeyboardDeviceObject->SetDataFormat( &c_dfDIKeyboard ) ;
			hr = InputSysData.PF.KeyboardDeviceObject->SetDataFormat( &C_DFDIKEYBOARD ) ;
			if( hr != D_DI_OK )
			{
//				if( InputSysData.PF.MouseDeviceObject )
//				{
//					InputSysData.PF.MouseDeviceObject->Release() ;
//					InputSysData.PF.MouseDeviceObject = NULL ;
//				}

				InputSysData.PF.KeyboardDeviceObject->Release() ;
				InputSysData.PF.KeyboardDeviceObject = NULL ;
				goto KEYBOARDDEVICEINITEND ;

//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;
//
//				NowInitialize = FALSE ;
//				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xc7\x30\xfc\x30\xbf\x30\x62\x5f\x0f\x5f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"キーボードデバイスのデータ形式の設定に失敗しました" @*/ ) ;
			}

			// キーボードの協調レベルを設定する
			if( InputSysData.PF.KeyExclusiveCooperativeLevelFlag )
			{
				hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
				if( hr != D_DI_OK )
				{
					hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
				}
			}
			else
			{
				hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
			}
	//		hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
	//		hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_FOREGROUND ) ;
 			if( hr != D_DI_OK ) 
			{
//				if( InputSysData.PF.MouseDeviceObject )
//				{
//					InputSysData.PF.MouseDeviceObject->Release() ;
//					InputSysData.PF.MouseDeviceObject = NULL ;
//				}

				InputSysData.PF.KeyboardDeviceObject->Release() ;
				InputSysData.PF.KeyboardDeviceObject = NULL ;
				goto KEYBOARDDEVICEINITEND ;

//				InputSysData.PF.DirectInputObject->Release() ;
//				InputSysData.PF.DirectInputObject = NULL ;
//
//				NowInitialize = FALSE ;
//				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"キーボードデバイスの協調レベルの設定に失敗しました" @*/ ) ;
			}

			// バッファを設定してみる
			{
				D_DIPROPDWORD DIProp ;

				DIProp.diph.dwSize = sizeof( DIProp ) ;
				DIProp.diph.dwHeaderSize = sizeof( DIProp.diph ) ;
				DIProp.diph.dwObj = 0 ;
				DIProp.diph.dwHow = D_DIPH_DEVICE ;
				DIProp.dwData = 100 ;
				if( InputSysData.PF.KeyboardDeviceObject->SetProperty( D_DIPROP_BUFFERSIZE, &DIProp.diph ) != D_DI_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\x6e\x30\xd7\x30\xed\x30\xd1\x30\xc6\x30\xa3\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キーボードのプロパティ設定に失敗しました\n" @*/ ) ;
				}
			}

			// 変化通知用のイベントを作成・アタッチする
			Event = WinAPIData.Win32Func.CreateEventAFunc( NULL, TRUE, FALSE, NULL ) ;
			hr = InputSysData.PF.KeyboardDeviceObject->SetEventNotification( Event ) ;
			if( hr != D_DI_POLLEDDEVICE && hr != D_DI_OK )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xa4\x30\xd9\x30\xf3\x30\xc8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キーボードイベントのセットアップが失敗しました\n" @*/ ) ;

				InputSysData.PF.KeyboardDeviceObject->Release() ;
				InputSysData.PF.KeyboardDeviceObject = NULL ;
				goto KEYBOARDDEVICEINITEND ;

//				NowInitialize = FALSE ;
//				return -1 ;
			}
			InputSysData.PF.KeyEvent = Event ;

			// キーボードのデバイスを取得する
			hr = InputSysData.PF.KeyboardDeviceObject->Acquire() ;
			if( hr != D_DI_OK )
			{
				hr = InputSysData.PF.KeyboardDeviceObject->Acquire() ;
				if( hr != D_DI_OK )
				{
//					if( InputSysData.PF.MouseDeviceObject )
//					{
//						InputSysData.PF.MouseDeviceObject->Release() ;
//						InputSysData.PF.MouseDeviceObject = NULL ;
//					}

					InputSysData.PF.KeyboardDeviceObject->Release() ;
					InputSysData.PF.KeyboardDeviceObject = NULL ;
					goto KEYBOARDDEVICEINITEND ;

//					InputSysData.PF.DirectInputObject->Release() ;
//					InputSysData.PF.DirectInputObject = NULL ;
//
//					NowInitialize = FALSE ;
//					return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"キーボードデバイスの取得に失敗しました" @*/ ) ;
				}
			}

			DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"初期化成功\n" @*/ ) ;

KEYBOARDDEVICEINITEND:
			if( InputSysData.PF.KeyboardDeviceObject == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キーボードデバイスの取得に失敗しました\n" @*/ ) ; 
			}

			InputSysData.KeyInputGetTime = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		}
		

		DXST_LOGFILE_TABSUB ;
		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\xa2\x95\x23\x90\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectInput 関連の初期化は正常に終了しました\n" @*/ ) ;
	}
	else
	{
		// DirectInput を使用しない場合
		JOYCAPSW joycaps ;
		DXST_LOGFILE_TABADD ;

		// パッドの数を調べる
		for( i = 0 ; i < MAX_JOYPAD_NUM ; i ++ )
		{
//			_MEMSET( &joyex, 0, sizeof( joyex ) ) ;
//			joyex.dwSize  = sizeof( joyex ) ;
//			joyex.dwFlags = JOY_RETURNALL ;
//			if( WinAPIData.Win32Func.joyGetPosExFunc( i, &joyex ) != JOYERR_NOERROR ) break ;
			_MEMSET( &joycaps, 0, sizeof( joycaps ) ) ;
			hr = ( HRESULT )WinAPIData.Win32Func.joyGetDevCapsFunc( ( UINT )i, &joycaps, sizeof( joycaps ) ) ;
			if( hr != JOYERR_NOERROR ) break ;
			InputSysData.Pad[ i ].PF.RightStickFlag = ( joycaps.wCaps & JOYCAPS_HASZ ) && ( joycaps.wCaps & JOYCAPS_HASR ) ;
			InputSysData.Pad[ i ].PF.MultimediaAPICaps = joycaps.wCaps ;
			InputSysData.Pad[ i ].DeadZone = DEADZONE ;
			InputSysData.Pad[ i ].DeadZoneD = DEADZONE_D ;
		}
		InputSysData.PadNum = i ;

		DXST_LOGFILEFMT_ADDUTF16LE(( "\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x70\x65\x6f\x30\x20\x00\x25\x00\x64\x00\x20\x00\x0b\x50\x67\x30\x59\x30\x0a\x00\x00"/*@ L"パッドの数は %d 個です\n" @*/, i ));

		DXST_LOGFILE_TABSUB ;
		InputSysData.PF.DirectInputObject = (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ;
		DXST_LOGFILE_ADDUTF16LE( "\x65\x51\x9b\x52\xa2\x95\x23\x90\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"入力関連の初期化をしました\n" @*/ ) ;
	}

	// キーボードとジョイパッドの入力のデフォルトの対応表を設定する
	if( InputSysData.KeyToJoypadInputInitializeFlag == FALSE )
	{
		InputSysData.KeyToJoypadInputInitializeFlag = TRUE ;

		for( i = 0 ; i < MAX_JOYPAD_NUM ; i ++ )
		{
			for ( j = 0 ; j < 32 ; j ++ )
			{
				InputSysData.KeyToJoypadInput[ i ][ j ][ 0 ] = -1 ;
				InputSysData.KeyToJoypadInput[ i ][ j ][ 1 ] = -1 ;
			}
		}
		InputSysData.KeyToJoypadInput[ 0 ][  0 ][ 0 ] = D_DIK_NUMPAD2;
		InputSysData.KeyToJoypadInput[ 0 ][  0 ][ 1 ] = D_DIK_DOWN;
		InputSysData.KeyToJoypadInput[ 0 ][  1 ][ 0 ] = D_DIK_NUMPAD4;
		InputSysData.KeyToJoypadInput[ 0 ][  1 ][ 1 ] = D_DIK_LEFT;
		InputSysData.KeyToJoypadInput[ 0 ][  2 ][ 0 ] = D_DIK_NUMPAD6;
		InputSysData.KeyToJoypadInput[ 0 ][  2 ][ 1 ] = D_DIK_RIGHT;
		InputSysData.KeyToJoypadInput[ 0 ][  3 ][ 0 ] = D_DIK_NUMPAD8;
		InputSysData.KeyToJoypadInput[ 0 ][  3 ][ 1 ] = D_DIK_UP;
		InputSysData.KeyToJoypadInput[ 0 ][  4 ][ 0 ] = D_DIK_Z;
		InputSysData.KeyToJoypadInput[ 0 ][  5 ][ 0 ] = D_DIK_X;
		InputSysData.KeyToJoypadInput[ 0 ][  6 ][ 0 ] = D_DIK_C;
		InputSysData.KeyToJoypadInput[ 0 ][  7 ][ 0 ] = D_DIK_A;
		InputSysData.KeyToJoypadInput[ 0 ][  8 ][ 0 ] = D_DIK_S;
		InputSysData.KeyToJoypadInput[ 0 ][  9 ][ 0 ] = D_DIK_D;
		InputSysData.KeyToJoypadInput[ 0 ][ 10 ][ 0 ] = D_DIK_Q;
		InputSysData.KeyToJoypadInput[ 0 ][ 11 ][ 0 ] = D_DIK_W;
		InputSysData.KeyToJoypadInput[ 0 ][ 12 ][ 0 ] = D_DIK_ESCAPE;
		InputSysData.KeyToJoypadInput[ 0 ][ 13 ][ 0 ] = D_DIK_SPACE;
	}

	// 初期化中フラグを倒す
	NowInitialize = FALSE ;

	// 正常終了
	return 0 ;
}

// 入力システムの後始末をする処理の環境依存処理
extern int TerminateInputSystem_PF_Timing0( void )
{
	// 初期化判定
	if( InputSysData.PF.DirectInputObject == NULL )
	{
		return 0 ;
	}

	SETUP_WIN_API

	// ＤｉｒｅｃｔＩｎｐｕｔ を使用していたかどうかで処理を分岐
	if( InputSysData.PF.DirectInputObject != (D_IDirectInput7 *)(DWORD_PTR)0xffffffff )
	{
		// DirectInput を使用した場合
		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x8c\x5b\x86\x4e\x0a\x00\x00"/*@ L"DirectInput 関連の終了処理... 完了\n" @*/ ) ;

		// ジョイパッドデバイスの解放
		TerminateJoypad() ;

		// キーボードデバイスの解放
		if( InputSysData.PF.KeyboardDeviceObject != NULL )
		{
			InputSysData.PF.KeyboardDeviceObject->SetEventNotification( NULL ) ;
			InputSysData.PF.KeyboardDeviceObject->Unacquire() ;
			InputSysData.PF.KeyboardDeviceObject->Release() ;
			InputSysData.PF.KeyboardDeviceObject = NULL ;
			WinAPIData.Win32Func.CloseHandleFunc( InputSysData.PF.KeyEvent ) ;
		}

		// マウスデバイスの解放
		if( InputSysData.PF.MouseDeviceObject != NULL )
		{
			InputSysData.PF.MouseDeviceObject->Unacquire() ;
			InputSysData.PF.MouseDeviceObject->Release() ;
			InputSysData.PF.MouseDeviceObject = NULL ;
		}

		// ＤｉｒｅｃｔＩｎｐｕｔオブジェクトの解放
		if( InputSysData.PF.DirectInputObject != NULL )
		{
			InputSysData.PF.DirectInputObject->Release() ;
			InputSysData.PF.DirectInputObject = NULL ;
		}

		// ＸＩｎｐｕｔの後始末処理
		if( InputSysData.PF.XInputDLL )
		{
			FreeLibrary( InputSysData.PF.XInputDLL ) ;
			InputSysData.PF.XInputDLL = NULL ;
			InputSysData.PF.XInputGetStateFunc = NULL ;
			InputSysData.PF.XInputSetStateFunc = NULL ;
		}
	}
	else
	{
		// DirectInput を使用しなかった場合
		DXST_LOGFILE_ADDUTF16LE( "\x65\x51\x9b\x52\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x8c\x5b\x86\x4e\x0a\x00\x00"/*@ L"入力関連の終了処理... 完了\n" @*/ ) ;

		InputSysData.PF.DirectInputObject = NULL ;
	}

	// 正常終了
	return 0 ;
}

// 自動初期化を行う環境依存処理
extern int AutoInitialize_PF( void )
{
	// ウインドウがクローズされる前なら初期化を行う
	if( NS_GetWindowCloseFlag() == FALSE )
	{
		return InitializeInputSystem() ;
	}

	// それ以外の場合は通常終了
	return 0 ;
}

// ジョイパッドのセットアップの環境依存処理
extern int SetupJoypad_PF( void )
{
	// 初期化判定
	if( InputSysData.PF.DirectInputObject == NULL || InputSysData.PF.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff )
	{
		return -1 ;
	}

	// 既に初期化されていた場合を考えて後始末を最初に行う
	TerminateJoypad() ;

	// ジョイパッドデバイスを初期化する
	DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x0a\x00\x00"/*@ L"ジョイパッドの初期化... \n" @*/ ) ;
	InputSysData.PadNum = 0 ;

	// XInput で取得できるパッドを先に検出
	if( InputSysData.PF.XInputDLL != NULL )
	{
		DWORD i ;
		DWORD j ;
		INPUTPADDATA *pad ;
		D_XINPUT_STATE state ;

		pad = &InputSysData.Pad[ InputSysData.PadNum ] ;
		for( i = 0 ; i < 4 ; i ++ )
		{
			if( InputSysData.PF.XInputGetStateFunc( i, &state ) != ERROR_SUCCESS )
				continue ;

			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xca\x30\xf3\x30\xd0\x30\xfc\x30\x20\x00\x25\x00\x64\x00\x20\x00\x6f\x30\x20\x00\x58\x00\x62\x00\x6f\x00\x78\x00\x33\x00\x36\x00\x30\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x74\x00\x72\x00\x6f\x00\x6c\x00\x6c\x00\x65\x00\x72\x00\x20\x00\x67\x30\x59\x30\x00"/*@ L"ジョイパッドナンバー %d は Xbox360 Controller です" @*/, InputSysData.PadNum )) ;

			// XInput でのデバイス番号を保存
			pad->PF.XInputDeviceNo = ( int )i ;

			// 無効ゾーンのセット
			pad->DeadZone = DEADZONE ;
			pad->DeadZoneD = DEADZONE_D ;

			// 振動関係の設定の初期化を行う
			for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
			{
				pad->Effect[ j ].PF.DIEffect   = NULL ;
				pad->Effect[ j ].PlayFlag	= FALSE ;
				pad->Effect[ j ].Power		= 0 ;
				pad->Effect[ j ].PlayState  = FALSE ;
				pad->Effect[ j ].SetPower   = 0 ;
			}
//			pad->EffectNotCommonFlag    = FALSE ;
//			pad->EffectCommon.PlayFlag	= FALSE ;
//			pad->EffectCommon.Power		= D_DI_FFNOMINALMAX ;
//			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PlayFlag	= FALSE ;
//			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power		= D_DI_FFNOMINALMAX ;
//			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PlayFlag	= FALSE ;
//			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power		= D_DI_FFNOMINALMAX ;
//			pad->EffectPlayStateLeft	= FALSE ;
//			pad->EffectPlayStateRight	= FALSE ;

			// ステータスを初期化
			_MEMSET( &pad->State, 0, sizeof( D_DIJOYSTATE ) ) ;
			_MEMSET( &pad->XInputState, 0, sizeof( pad->XInputState ) ) ;

			// 名前を設定
			_WCSCPY_S( pad->PF.InstanceName, sizeof( pad->PF.InstanceName ), L"Controller (XBOX 360 For Windows)" );
			_WCSCPY_S( pad->PF.ProductName,  sizeof( pad->PF.ProductName  ), L"Controller (XBOX 360 For Windows)" );

			pad ++ ;
			InputSysData.PadNum ++ ;
		}
	}

	InputSysData.PF.DirectInputObject->EnumDevices( D_DIDEVTYPE_JOYSTICK, EnumJoypadProc, NULL, D_DIEDFL_ATTACHEDONLY ) ;

	// 正常終了
	return 0 ;
}

// ジョイパッドの後始末を行う処理の環境依存処理
extern int TerminateJoypad_PF( void )
{
	int i ;
	int j ;
	INPUTPADDATA *pad ;

	// 初期化判定
	if( InputSysData.PF.DirectInputObject == NULL || InputSysData.PF.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff )
	{
		return -1 ;
	}

	// ジョイパッドデバイスの解放
	pad = InputSysData.Pad ;
	for( i = 0 ; i < InputSysData.PadNum ; i ++ , pad ++ )
	{
		if( pad->PF.XInputDeviceNo < 0 )
		{
			pad->PF.Device->Unacquire() ;
			pad->PF.Device->SetEventNotification( NULL ) ;

			for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
			{
				if( pad->Effect[ j ].PF.DIEffect != NULL )
				{
					pad->Effect[ j ].PF.DIEffect->Stop() ;
					pad->Effect[ j ].PF.DIEffect->Unload() ;
					pad->Effect[ j ].PF.DIEffect = NULL ;
				}
			}

			pad->PF.Device->Release() ;
			pad->PF.Device = NULL ;

			WinAPIData.Win32Func.CloseHandleFunc( pad->PF.Event ) ;
		}
		
		_MEMSET( pad, 0, sizeof( *pad ) ) ;
	}

	InputSysData.PadNum = 0 ;

	// 正常終了
	return 0 ;
}

// 入力状態の更新の環境依存処理
extern int UpdateKeyboardInputState_PF( int UseProcessMessage )
{
	unsigned char KeyBuffer[256] ;
	int Time, hr, i ;
	int Result = 0 ;

	SETUP_WIN_API

	// キーボード入力処理に DirectInput を使用するかどうかで処理を分岐
	if( InputSysData.PF.NoUseDirectInputFlag          == TRUE ||
		InputSysData.PF.KeyboardNotUseDirectInputFlag == TRUE ||
		InputSysData.PF.KeyboardDeviceObject          == NULL ||
		( InputSysData.PF.UseDirectInputFlag == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA ) )
	{
		// メインスレッド以外から呼ばれた場合は何もせずに終了
		if( WinAPIData.Win32Func.GetCurrentThreadIdFunc() != WinData.MainThreadID )
		{
			goto ENDFUNCTION ;
		}

		// まだ前回の取得から 1msec も経過していない場合は以前の状態を返す
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( InputSysData.KeyInputGetTime == Time ) goto ENDFUNCTION ;
		InputSysData.KeyInputGetTime = Time ;

		// キー入力のメッセージが来ている可能性があるので、メッセージ処理を行う
		if( UseProcessMessage != FALSE && NS_ProcessMessage() != 0 )
		{
			goto ENDFUNCTION ;
		}

		// 標準ＡＰＩで入力状態を取得する
		WinAPIData.Win32Func.GetKeyboardStateFunc( KeyBuffer ) ;
		_MEMSET( &InputSysData.KeyInputBuf, 0, sizeof( InputSysData.KeyInputBuf ) ) ;
		for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
			InputSysData.KeyInputBuf[__KeyMap[i][1]] = KeyBuffer[__KeyMap[i][0]] ;

		InputSysData.MouseInputBuf[ 0 ] = ( BYTE )( ( KeyBuffer[ VK_LBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		InputSysData.MouseInputBuf[ 1 ] = ( BYTE )( ( KeyBuffer[ VK_RBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		InputSysData.MouseInputBuf[ 2 ] = ( BYTE )( ( KeyBuffer[ VK_MBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		if( WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA )
		{
			InputSysData.MouseInputBuf[ 3 ] = ( BYTE )( ( KeyBuffer[ VK_XBUTTON1 ] & 0x80 ) ? 1 : 0 ) ;
			InputSysData.MouseInputBuf[ 4 ] = ( BYTE )( ( KeyBuffer[ VK_XBUTTON2 ] & 0x80 ) ? 1 : 0 ) ;
		}
		InputSysData.MouseInputBufValidFlag = TRUE ;
	}
	else
	{
		// まだ前回の取得から 1msec も経過していない場合は以前の状態を返す
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( InputSysData.KeyInputGetTime == Time && WinAPIData.Win32Func.WaitForSingleObjectFunc( InputSysData.PF.KeyEvent, 0 ) != WAIT_OBJECT_0 )
		{
			goto ENDFUNCTION ;
		}

		// キー入力のメッセージが来ている可能性があるので、メッセージ処理を行う
		if( UseProcessMessage != FALSE && NS_ProcessMessage() != 0 )
		{
			goto ENDFUNCTION ;
		}

		// 初期化判定
		if( InputSysData.PF.DirectInputObject == NULL )
		{
			goto ENDFUNCTION ;
		}

		// ステータス取得、ロストしていたら再取得
		hr = InputSysData.PF.KeyboardDeviceObject->GetDeviceState( sizeof( InputSysData.KeyInputBuf ) , ( LPVOID )InputSysData.KeyInputBuf ) ;
//		if( hr == DIERR_INPUTLOST )
		if( hr != D_DI_OK )
		{
			if( InputSysData.PF.KeyboardDeviceObject->Acquire() == D_DI_OK )
			{
				hr = InputSysData.PF.KeyboardDeviceObject->GetDeviceState( sizeof( InputSysData.KeyInputBuf ) , ( LPVOID )InputSysData.KeyInputBuf ) ;
				if( hr != D_DI_OK ) goto ENDFUNCTION ;
			}
			else
			{
				goto ENDFUNCTION ;
			}
		}

		InputSysData.KeyInputGetTime = Time ;
		
		// イベントをリセット
		if( WinAPIData.Win32Func.WaitForSingleObjectFunc( InputSysData.PF.KeyEvent, 0 ) == WAIT_OBJECT_0 ) WinAPIData.Win32Func.ResetEventFunc( InputSysData.PF.KeyEvent ) ;

		// 右シフトキーを標準ＡＰＩから取る
		WinAPIData.Win32Func.GetKeyboardStateFunc( KeyBuffer ) ;
		InputSysData.KeyInputBuf[ KEY_INPUT_RSHIFT ] = KeyBuffer[ VK_RSHIFT ] ;

		// ******* ＤｉｒｅｃｔＩｎｐｕｔが信用できないので、一部のキーを除き標準ＡＰＩでキー状態を取得 ******* //
/*		if( InputSysData.PF.KeyExclusiveCooperativeLevelFlag == FALSE )
		{
			//メインスレッド以外だったらこの処理を行わない
			if( WinData.MainThreadID == WinAPIData.Win32Func.GetCurrentThreadIdFunc() )
			{
				// 標準ＡＰＩで入力状態を取得する
				WinAPIData.Win32Func.GetKeyboardStateFunc( KeyBuffer ) ;
				for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
					if( __KeyMap[i][2] != 1 )
						InputSysData.KeyInputBuf[__KeyMap[i][1]] = KeyBuffer[__KeyMap[i][0]] ;
			}
		}
*/	}

	Result = 1 ;

ENDFUNCTION:

	return Result ;
}

// パッドの入力状態の更新の環境依存処理
extern int UpdateJoypadInputState_PF( int PadNo )
{
	INPUTPADDATA *pad = &InputSysData.Pad[ PadNo ] ;
	D_DIJOYSTATE InputState ;
	JOYINFOEX joyex ;
	HRESULT hr ;
	int Time, i ;

	SETUP_WIN_API

	// DirectInput を使用しているかどうかで処理を分岐
	if( InputSysData.PF.NoUseDirectInputFlag == TRUE )
	{
		// DirectInput を使用しない場合

		// 前回の取得から 1msec も経過していない場合は以前の状態を返す
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time ) return 0 ;
		pad->GetTime = Time ;

		// 入力状態を取得する
		_MEMSET( &joyex, 0, sizeof( joyex ) ) ;
		joyex.dwSize  = sizeof( JOYINFOEX ) ;
		joyex.dwFlags = JOY_RETURNALL ;
		if( WinAPIData.Win32Func.joyGetPosExFunc( ( UINT )PadNo, &joyex ) != JOYERR_NOERROR )
		{
			// 取得できなかった場合は接続されていないということなので情報をゼロで埋めておく
			_MEMSET( &pad->State, 0, sizeof( pad->State ) ) ;

			// POV は -1 にしておく
			for( i = 0 ; i < 4 ; i ++ )
			{
				pad->State.POV[ i ] = 0xffffffff ;
			}
		}
		else
		{
			// マルチメディアＡＰＩ用の入力値を DirectInput 用に変換する

			switch( joyex.dwXpos )
			{
			case 0x7fff :	pad->State.X =             0 ;	break ;
			case 0x0000 :	pad->State.X = -DIRINPUT_MAX ;	break ;
			case 0xffff :	pad->State.X =  DIRINPUT_MAX ;	break ;
			default :
				     if( joyex.dwXpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.X = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwXpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else if( joyex.dwXpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.X = ( LONG )(          ( ( (LONG)joyex.dwXpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else                                                               pad->State.X = 0 ;
				break ;
			}

			switch( joyex.dwYpos )
			{
			case 0x7fff :	pad->State.Y =             0 ;	break ;
			case 0x0000 :	pad->State.Y = -DIRINPUT_MAX ;	break ;
			case 0xffff :	pad->State.Y =  DIRINPUT_MAX ;	break ;
			default :
				     if( joyex.dwYpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Y = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwYpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else if( joyex.dwYpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Y = ( LONG )(          ( ( (LONG)joyex.dwYpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else                                                               pad->State.Y = 0 ;
				break ;
			}

			if( pad->PF.MultimediaAPICaps & JOYCAPS_HASZ )
			{
				switch( joyex.dwZpos )
				{
				case 0x7fff :	pad->State.Z =             0 ;	break ;
				case 0x0000 :	pad->State.Z = -DIRINPUT_MAX ;	break ;
				case 0xffff :	pad->State.Z =  DIRINPUT_MAX ;	break ;
				default :
						 if( joyex.dwZpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Z = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwZpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwZpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Z = ( LONG )(          ( ( (LONG)joyex.dwZpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.Z = 0 ;
					break ;
				}
			}
			else
			{
				pad->State.Z = 0 ;
			}

			if( pad->PF.MultimediaAPICaps & JOYCAPS_HASR )
			{
				switch( joyex.dwRpos )
				{
				case 0x7fff :	pad->State.Rz =             0 ;	break ;
				case 0x0000 :	pad->State.Rz = -DIRINPUT_MAX ;	break ;
				case 0xffff :	pad->State.Rz =  DIRINPUT_MAX ;	break ;
				default :
						 if( joyex.dwRpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Rz = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwRpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwRpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Rz = ( LONG )(          ( ( (LONG)joyex.dwRpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.Rz = 0 ;
					break ;
				}
			}
			else
			{
				pad->State.Rz = 0 ;
			}

			if( pad->PF.MultimediaAPICaps & JOYCAPS_HASU )
			{
				switch( joyex.dwUpos )
				{
				case 0x7fff :	pad->State.Rx =             0 ;	break ;
				case 0x0000 :	pad->State.Rx = -DIRINPUT_MAX ;	break ;
				case 0xffff :	pad->State.Rx =  DIRINPUT_MAX ;	break ;
				default :
						 if( joyex.dwUpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Rx = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwUpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwUpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Rx = ( LONG )(          ( ( (LONG)joyex.dwUpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.Rx = 0 ;
					break ;
				}
			}
			else
			{
				pad->State.Rx = 0 ;
			}

			if( pad->PF.MultimediaAPICaps & JOYCAPS_HASV )
			{
				switch( joyex.dwVpos )
				{
				case 0x7fff :	pad->State.Ry =             0 ;	break ;
				case 0x0000 :	pad->State.Ry = -DIRINPUT_MAX ;	break ;
				case 0xffff :	pad->State.Ry =  DIRINPUT_MAX ;	break ;
				default :
						 if( joyex.dwVpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Ry = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwVpos ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwVpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.Ry = ( LONG )(          ( ( (LONG)joyex.dwVpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * DIRINPUT_MAX / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.Ry = 0 ;
					break ;
				}
			}
			else
			{
				pad->State.Ry = 0 ;
			}

			for( i = 0 ; i < 24 ; i ++ )
			{
				if( joyex.dwButtons & ( 1 << i ) ) pad->State.Buttons[ i ] = 0x80 ;
				else                               pad->State.Buttons[ i ] = 0x00 ;
			}

			if( pad->PF.MultimediaAPICaps & JOYCAPS_HASPOV )
			{
				pad->State.POV[ 0 ] = joyex.dwPOV == 0xffff ? 0xffffffff : joyex.dwPOV ;
			}
			else
			{
				pad->State.POV[ 0 ] = 0xffffffff ;
			}
			pad->State.POV[ 1 ] = 0xffffffff ;
			pad->State.POV[ 2 ] = 0xffffffff ;
			pad->State.POV[ 3 ] = 0xffffffff ;
		}
	}
	else
	if( pad->PF.XInputDeviceNo < 0 )
	{
		// DirectInput を使用する場合

		// 前回の取得から 1msec も経過していない場合は以前の状態を返す
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time && WinAPIData.Win32Func.WaitForSingleObjectFunc( pad->PF.Event, 0 ) != WAIT_OBJECT_0 )
		{
			return 0 ;
		}
		pad->GetTime = Time ;

		// 入力データのポーリング
		pad->PF.Device->Poll() ;

		hr = pad->PF.Device->GetDeviceState( sizeof( D_DIJOYSTATE ) , &InputState ) ;
		if( hr != D_DI_OK )
		{
			if( NS_ProcessMessage() != 0 )
			{
				return -1 ;
			}
			if( InputSysData.PF.DirectInputObject == NULL )
			{
				return -1 ;
			}

			if( pad->PF.Device->Acquire() == D_DI_OK )
			{
				if( NS_ProcessMessage() )
				{
					return -1 ;
				}
				if( InputSysData.PF.DirectInputObject == NULL )
				{
					return -1 ;
				}

				hr = pad->PF.Device->GetDeviceState( sizeof( D_DIJOYSTATE ) , &InputState ) ;
			}
			else
			{
				pad->PF.Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
				if( pad->PF.Device->Acquire() == D_DI_OK )
				{
					if( NS_ProcessMessage() )
					{
						return -1 ;
					}
					if( InputSysData.PF.DirectInputObject == NULL )
					{
						return -1 ;
					}

					hr = pad->PF.Device->GetDeviceState( sizeof( D_DIJOYSTATE ) , &InputState ) ;
				}
			}
		}

		if( hr == D_DI_OK )
		{
			pad->State.X = InputState.lX ;
			pad->State.Y = InputState.lY ;
			pad->State.Z = InputState.lZ ;
			pad->State.Rx = InputState.lRx ;
			pad->State.Ry = InputState.lRy ;
			pad->State.Rz = InputState.lRz ;
			pad->State.Slider[ 0 ] = InputState.rglSlider[ 0 ] ;
			pad->State.Slider[ 1 ] = InputState.rglSlider[ 1 ] ;
			pad->State.POV[ 0 ] = InputState.rgdwPOV[ 0 ] ;
			pad->State.POV[ 1 ] = InputState.rgdwPOV[ 1 ] ;
			pad->State.POV[ 2 ] = InputState.rgdwPOV[ 2 ] ;
			pad->State.POV[ 3 ] = InputState.rgdwPOV[ 3 ] ;
			pad->State.Buttons[ 0 ] = InputState.rgbButtons[ 0 ] ;
			pad->State.Buttons[ 1 ] = InputState.rgbButtons[ 1 ] ;
			pad->State.Buttons[ 2 ] = InputState.rgbButtons[ 2 ] ;
			pad->State.Buttons[ 3 ] = InputState.rgbButtons[ 3 ] ;
			pad->State.Buttons[ 4 ] = InputState.rgbButtons[ 4 ] ;
			pad->State.Buttons[ 5 ] = InputState.rgbButtons[ 5 ] ;
			pad->State.Buttons[ 6 ] = InputState.rgbButtons[ 6 ] ;
			pad->State.Buttons[ 7 ] = InputState.rgbButtons[ 7 ] ;
			pad->State.Buttons[ 8 ] = InputState.rgbButtons[ 8 ] ;
			pad->State.Buttons[ 9 ] = InputState.rgbButtons[ 9 ] ;
			pad->State.Buttons[ 10 ] = InputState.rgbButtons[ 10 ] ;
			pad->State.Buttons[ 11 ] = InputState.rgbButtons[ 11 ] ;
			pad->State.Buttons[ 12 ] = InputState.rgbButtons[ 12 ] ;
			pad->State.Buttons[ 13 ] = InputState.rgbButtons[ 13 ] ;
			pad->State.Buttons[ 14 ] = InputState.rgbButtons[ 14 ] ;
			pad->State.Buttons[ 15 ] = InputState.rgbButtons[ 15 ] ;
			pad->State.Buttons[ 16 ] = InputState.rgbButtons[ 16 ] ;
			pad->State.Buttons[ 17 ] = InputState.rgbButtons[ 17 ] ;
			pad->State.Buttons[ 18 ] = InputState.rgbButtons[ 18 ] ;
			pad->State.Buttons[ 19 ] = InputState.rgbButtons[ 19 ] ;
			pad->State.Buttons[ 20 ] = InputState.rgbButtons[ 20 ] ;
			pad->State.Buttons[ 21 ] = InputState.rgbButtons[ 21 ] ;
			pad->State.Buttons[ 22 ] = InputState.rgbButtons[ 22 ] ;
			pad->State.Buttons[ 23 ] = InputState.rgbButtons[ 23 ] ;
			pad->State.Buttons[ 24 ] = InputState.rgbButtons[ 24 ] ;
			pad->State.Buttons[ 25 ] = InputState.rgbButtons[ 25 ] ;
			pad->State.Buttons[ 26 ] = InputState.rgbButtons[ 26 ] ;
			pad->State.Buttons[ 27 ] = InputState.rgbButtons[ 27 ] ;
			pad->State.Buttons[ 28 ] = InputState.rgbButtons[ 28 ] ;
			pad->State.Buttons[ 29 ] = InputState.rgbButtons[ 29 ] ;
			pad->State.Buttons[ 30 ] = InputState.rgbButtons[ 30 ] ;
			pad->State.Buttons[ 31 ] = InputState.rgbButtons[ 31 ] ;
		}

		// イベントのリセット
		if( WinAPIData.Win32Func.WaitForSingleObjectFunc( pad->PF.Event, 0 ) == WAIT_OBJECT_0 ) WinAPIData.Win32Func.ResetEventFunc( pad->PF.Event ) ;
	}
	else
	{
		D_XINPUT_STATE XInputState ;

		// XInput を使用する場合

		// 前回の取得から 1msec も経過していない場合は以前の状態を返す
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time ) return 0 ;
		pad->GetTime = Time ;

		if( InputSysData.PF.XInputGetStateFunc == NULL ||
			InputSysData.PF.XInputGetStateFunc( ( DWORD )pad->PF.XInputDeviceNo, &XInputState ) != ERROR_SUCCESS )
		{
			// 取得できなかった場合は接続されていないということなので情報をゼロで埋めておく
			_MEMSET( &pad->XInputState, 0, sizeof( pad->XInputState ) ) ;

			// POV は -1 にしておく
			for( i = 0 ; i < 4 ; i ++ )
			{
				pad->State.POV[ i ] = 0xffffffff ;
			}
		}
		else
		{
			WORD Buttons ;

			// XInput用の入力値を保存
			pad->XInputState.Buttons[ 0  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 0 ) & 1 ) ;
			pad->XInputState.Buttons[ 1  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 1 ) & 1 ) ;
			pad->XInputState.Buttons[ 2  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 2 ) & 1 ) ;
			pad->XInputState.Buttons[ 3  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 3 ) & 1 ) ;
			pad->XInputState.Buttons[ 4  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 4 ) & 1 ) ;
			pad->XInputState.Buttons[ 5  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 5 ) & 1 ) ;
			pad->XInputState.Buttons[ 6  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 6 ) & 1 ) ;
			pad->XInputState.Buttons[ 7  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 7 ) & 1 ) ;
			pad->XInputState.Buttons[ 8  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 8 ) & 1 ) ;
			pad->XInputState.Buttons[ 9  ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 9 ) & 1 ) ;
			pad->XInputState.Buttons[ 10 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 10 ) & 1 ) ;
			pad->XInputState.Buttons[ 11 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 11 ) & 1 ) ;
			pad->XInputState.Buttons[ 12 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 12 ) & 1 ) ;
			pad->XInputState.Buttons[ 13 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 13 ) & 1 ) ;
			pad->XInputState.Buttons[ 14 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 14 ) & 1 ) ;
			pad->XInputState.Buttons[ 15 ] = ( unsigned char )( ( XInputState.Gamepad.wButtons >> 15 ) & 1 ) ;
			pad->XInputState.LeftTrigger   = XInputState.Gamepad.bLeftTrigger ;
			pad->XInputState.RightTrigger  = XInputState.Gamepad.bRightTrigger ;
			pad->XInputState.ThumbLX       = XInputState.Gamepad.sThumbLX ;
			pad->XInputState.ThumbLY       = XInputState.Gamepad.sThumbLY ;
			pad->XInputState.ThumbRX       = XInputState.Gamepad.sThumbRX ;
			pad->XInputState.ThumbRY       = XInputState.Gamepad.sThumbRY ;

			// XInput用の入力値を DirectInput 用に変換する

			switch( XInputState.Gamepad.sThumbLX )
			{
			case  32767 : pad->State.X =  DIRINPUT_MAX ; break ;
			case -32768 : pad->State.X = -DIRINPUT_MAX ; break ;
			default :
				     if( XInputState.Gamepad.sThumbLX < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.X = -( ( -XInputState.Gamepad.sThumbLX - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( XInputState.Gamepad.sThumbLX >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.X =  ( (  XInputState.Gamepad.sThumbLX - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                        pad->State.X = 0 ;
				break ;
			}

			switch( XInputState.Gamepad.sThumbLY )
			{
			case  32767 : pad->State.Y = -DIRINPUT_MAX ; break ;
			case -32768 : pad->State.Y =  DIRINPUT_MAX ; break ;
			default :
				     if( XInputState.Gamepad.sThumbLY < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Y =  ( ( -XInputState.Gamepad.sThumbLY - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( XInputState.Gamepad.sThumbLY >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Y = -( (  XInputState.Gamepad.sThumbLY - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                        pad->State.Y = 0 ;
				break ;
			}

			switch( XInputState.Gamepad.sThumbRX )
			{
			case  32767 : pad->State.Rx =  DIRINPUT_MAX ; break ;
			case -32768 : pad->State.Rx = -DIRINPUT_MAX ; break ;
			default :
				     if( XInputState.Gamepad.sThumbRX < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Rx = -( ( -XInputState.Gamepad.sThumbRX - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( XInputState.Gamepad.sThumbRX >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Rx =  ( (  XInputState.Gamepad.sThumbRX - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                        pad->State.Rx = 0 ;
				break ;
			}

			switch( XInputState.Gamepad.sThumbRY )
			{
			case  32767 : pad->State.Ry = -DIRINPUT_MAX ; break ;
			case -32768 : pad->State.Ry =  DIRINPUT_MAX ; break ;
			default :
				     if( XInputState.Gamepad.sThumbRY < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Ry =  ( ( -XInputState.Gamepad.sThumbRY - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( XInputState.Gamepad.sThumbRY >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.Ry = -( (  XInputState.Gamepad.sThumbRY - DEADZONE_XINPUT( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                        pad->State.Ry = 0 ;
				break ;
			}

			Buttons = XInputState.Gamepad.wButtons ;
			for( i = 0 ; i < XINPUT_TO_DIRECTINPUT_BUTTONNUM ; i ++ )
			{
				pad->State.Buttons[ i ] = ( BYTE )( ( Buttons & XInputButtonToDirectInputButtonNo[ i ] ) ? 0x80 : 0x00 ) ;
			}

			     if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) == 0 ) pad->State.POV[ 0 ] =     0 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 ) pad->State.POV[ 0 ] =  4500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) == 0 ) pad->State.POV[ 0 ] =  9000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 ) pad->State.POV[ 0 ] = 13500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) == 0 ) pad->State.POV[ 0 ] = 18000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 ) pad->State.POV[ 0 ] = 22500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) == 0 ) pad->State.POV[ 0 ] = 27000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 ) pad->State.POV[ 0 ] = 31500 ;
			else                                                                                                                                                          pad->State.POV[ 0 ] = 0xffffffff ;
			pad->State.POV[ 1 ] = 0xffffffff ;
			pad->State.POV[ 2 ] = 0xffffffff ;
			pad->State.POV[ 3 ] = 0xffffffff ;

			pad->State.Z = 0 ;
			if( XInputState.Gamepad.bLeftTrigger  > DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) )
			{
				pad->State.Z += ( XInputState.Gamepad.bLeftTrigger  - DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT_TRIGGER( pad->DeadZone ) ;
			}
			if( XInputState.Gamepad.bRightTrigger > DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) )
			{
				pad->State.Z -= ( XInputState.Gamepad.bRightTrigger - DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) ) * DIRINPUT_MAX / VALIDRANGE_XINPUT_TRIGGER( pad->DeadZone ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// パッドエフェクトの再生状態を更新する関数の環境依存処理
extern int RefreshEffectPlayState_PF( void )
{
	int                 i ;
	int                 j ;
	int                 num ;
	int                 time ;
	int                 PlayState ;
	int                 SetPower ;
	int                 Flag ;
	INPUTPADDATA        *Pad ;
	D_DICONSTANTFORCE   cf ;
	D_DIEFFECT          eff;
	D_XINPUT_VIBRATION  VibState ;

	// 初期化判定
	if( InputSysData.PF.DirectInputObject == NULL /* && NS_GetWindowCloseFlag() == FALSE */ )
		return 0 ;

	// ＤｉｒｅｃｔＩｎｐｕｔを使用していない場合はエフェクト処理は出来ないので終了
	if( InputSysData.PF.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return 0 ;

	// パッドデバイスの再取得
	num  = InputSysData.PadNum ;
	Pad  = InputSysData.Pad ;
	time = NS_GetNowCount() ;

	// エフェクト構造体の初期化
	_MEMSET( &eff, 0, sizeof( eff ) ) ;
	eff.dwSize                = sizeof( D_DIEFFECT ) ;
	eff.dwFlags               = D_DIEFF_CARTESIAN | D_DIEFF_OBJECTOFFSETS ;
	eff.lpEnvelope            = 0 ;
	eff.cbTypeSpecificParams  = sizeof( D_DICONSTANTFORCE ) ;
	eff.lpvTypeSpecificParams = &cf ;
	eff.dwStartDelay          = 0 ;

	// ソフトのアクティブ状態によって処理を分岐
	if( NS_GetActiveFlag() == TRUE )
	{
		// ソフトがアクティブな場合
		for( i = 0 ; i < num ; i ++, Pad ++ )
		{
			// XInput かどうかで処理を分岐
			if( Pad->PF.XInputDeviceNo >= 0 )
			{
				// XInput の場合
				VibState.wLeftMotorSpeed  = 0 ;
				VibState.wRightMotorSpeed = 0 ;
				Flag = 0 ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].PlayFlag )
					{
						SetPower = Pad->Effect[ j ].Power ;
					}
					else
					{
						SetPower = 0 ;
					}

					switch( j )
					{
					case DINPUTPAD_MOTOR_LEFT :
						VibState.wLeftMotorSpeed  = ( WORD )( SetPower * 65535 / D_DI_FFNOMINALMAX ) ;
						break ;

					case DINPUTPAD_MOTOR_RIGHT :
						VibState.wRightMotorSpeed = ( WORD )( SetPower * 65535 / D_DI_FFNOMINALMAX ) ;
						break ;
					}

					if( Pad->Effect[ j ].SetPower != SetPower )
					{
						Flag = 1 ;
						Pad->Effect[ j ].SetPower = SetPower ;
					}

					Pad->Effect[ j ].PlayState = Pad->Effect[ j ].PlayFlag ;
				}

				if( InputSysData.PF.XInputSetStateFunc != NULL && Flag == 1 )
				{
					InputSysData.PF.XInputSetStateFunc( ( DWORD )Pad->PF.XInputDeviceNo, &VibState ) ;
				}
			}
			else
			{
				if( Pad->PF.Device == NULL ) continue ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].PF.DIEffect == NULL )
					{
						continue ;
					}

					if( Pad->Effect[ j ].PlayFlag )
					{
						SetPower = Pad->Effect[ j ].Power ;
					}
					else
					{
						SetPower = 0 ;
					}

					PlayState = SetPower > 0 ? TRUE : FALSE ;

					if( Pad->Effect[ j ].SetPower  != SetPower ||
						Pad->Effect[ j ].PlayState != PlayState )
					{
						cf.lMagnitude = SetPower ;

						if( PlayState == TRUE )
						{
							if( Pad->Effect[ j ].PlayState == TRUE )
							{
								if( Pad->Effect[ j ].SetPower != SetPower )
								{
									Pad->Effect[ j ].PF.DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS | D_DIEP_START ) ;
								}
							}
							else
							if( Pad->Effect[ j ].PlayState == FALSE )
							{
								if( Pad->Effect[ j ].SetPower != SetPower )
								{
									Pad->Effect[ j ].PF.DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
								}
								Pad->Effect[ j ].PF.DIEffect->Start( INFINITE, 0 ) ;
							}
						}
						else
						{
							if( Pad->Effect[ j ].PlayState )
							{
								Pad->Effect[ j ].PF.DIEffect->Stop() ;
							}

							if( Pad->Effect[ j ].SetPower != SetPower )
							{
								Pad->Effect[ j ].PF.DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
							}
						}

						Pad->Effect[ j ].SetPower  = SetPower ;
						Pad->Effect[ j ].PlayState = Pad->Effect[ j ].PlayFlag ;
					}
				}
			}
		}
	}
	else
	{
		// ソフトが非アクティブな場合
		for( i = 0 ; i < num ; i ++, Pad ++ )
		{
			if( Pad->PF.XInputDeviceNo >= 0 )
			{
				VibState.wLeftMotorSpeed  = 0 ;
				VibState.wRightMotorSpeed = 0 ;
				Flag = 0 ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].SetPower != 0 )
					{
						Flag = 1 ;
						Pad->Effect[ j ].SetPower = 0 ;
					}

					Pad->Effect[ j ].PlayState = FALSE ;
				}

				if( InputSysData.PF.XInputSetStateFunc != NULL && Flag == 1 )
				{
					InputSysData.PF.XInputSetStateFunc( ( DWORD )Pad->PF.XInputDeviceNo, &VibState ) ;
				}
			}
			else
			{
				if( Pad->PF.Device == NULL ) continue ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].PF.DIEffect == NULL )
					{
						continue ;
					}

					if( Pad->Effect[ j ].SetPower  != 0 ||
						Pad->Effect[ j ].PlayState != FALSE )
					{
						cf.lMagnitude = 0 ;

						if( Pad->Effect[ j ].PlayState )
						{
							Pad->Effect[ j ].PF.DIEffect->Stop() ;
						}

						if( Pad->Effect[ j ].SetPower != 0 )
						{
							Pad->Effect[ j ].PF.DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
						}

						Pad->Effect[ j ].SetPower  = 0 ;
						Pad->Effect[ j ].PlayState = FALSE ;
					}
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// 指定のパッドが振動に対応しているかどうかを取得する( TRUE:対応している  FALSE:対応していない )
extern int CheckJoypadVibrationEnable_PF( INPUTPADDATA *pad, int EffectIndex )
{
	return ( pad->PF.XInputDeviceNo >= 0 || ( pad->PF.Device != NULL && pad->Effect[ EffectIndex ].PF.DIEffect != NULL ) ) ? TRUE : FALSE ;
}

// 指定の入力デバイスが XInput に対応しているかどうかを取得する処理の環境依存処理( 戻り値  TRUE:XInput対応の入力デバイス  FALSE:XInput非対応の入力デバイス   -1:エラー )( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )の環境依存処理
extern int CheckJoypadXInput_PF( int InputType )
{
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	pad = &InputSysData.Pad[ JoypadNum ] ;

	return pad->PF.XInputDeviceNo < 0 ? FALSE : TRUE ;
}

// マウスのボタンの状態を得る処理の環境依存処理
extern int GetMouseInput_PF( void )
{
	D_DIMOUSESTATE2 input ;
	HRESULT hr ;
	int res ;

	// DirectInput を使用しているかどうかで処理を分岐
	// ( 非アクティブの場合も DirectInput を使用しない )
	if( CheckUseDirectInputMouse( TRUE ) == FALSE ||
		NS_GetActiveFlag() == FALSE )
	{
		// DirectInput を使用しない場合
		UpdateKeyboardInputState() ;
		if( InputSysData.MouseInputBufValidFlag == TRUE )
		{
			res = 0 ;
			if( InputSysData.MouseInputBuf[ 0 ] ) res |= MOUSE_INPUT_LEFT ;
			if( InputSysData.MouseInputBuf[ 1 ] ) res |= MOUSE_INPUT_RIGHT ;
			if( InputSysData.MouseInputBuf[ 2 ] ) res |= MOUSE_INPUT_MIDDLE ;

			if( WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA )
			{
				if( InputSysData.MouseInputBuf[ 3 ] ) res |= MOUSE_INPUT_4 ;
				if( InputSysData.MouseInputBuf[ 4 ] ) res |= MOUSE_INPUT_5 ;
			}
		}
		else
		{
			unsigned char KeyBuffer[256] ;

			WinAPIData.Win32Func.GetKeyboardStateFunc( KeyBuffer ) ;
			res = 0 ;
			if( KeyBuffer[ VK_LBUTTON ] & 0x80 ) res |= MOUSE_INPUT_LEFT ;
			if( KeyBuffer[ VK_RBUTTON ] & 0x80 ) res |= MOUSE_INPUT_RIGHT ;
			if( KeyBuffer[ VK_MBUTTON ] & 0x80 ) res |= MOUSE_INPUT_MIDDLE ;

			if( WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA )
			{
				if( KeyBuffer[ VK_XBUTTON1 ] & 0x80 ) res |= MOUSE_INPUT_4 ;
				if( KeyBuffer[ VK_XBUTTON2 ] & 0x80 ) res |= MOUSE_INPUT_5 ;
			}
		}
	}
	else
	{
		// DirectInput を使用する場合

		hr = InputSysData.PF.MouseDeviceObject->GetDeviceState( sizeof( D_DIMOUSESTATE2 ), (LPVOID)&input ) ;
		if( hr != D_DI_OK )
		{
			if( InputSysData.PF.MouseDeviceObject->Acquire() == D_DI_OK )
			{
				hr = InputSysData.PF.MouseDeviceObject->GetDeviceState( sizeof( D_DIMOUSESTATE2 ) , (LPVOID)&input ) ;
				if( hr != D_DI_OK ) return 0 ;
			}
			else return 0 ;
		}

		res = 0 ;
		if( input.rgbButtons[0] & 0x80 ) res |= MOUSE_INPUT_1 ;
		if( input.rgbButtons[1] & 0x80 ) res |= MOUSE_INPUT_2 ;
		if( input.rgbButtons[2] & 0x80 ) res |= MOUSE_INPUT_3 ;
		if( input.rgbButtons[3] & 0x80 ) res |= MOUSE_INPUT_4 ;
		if( input.rgbButtons[4] & 0x80 ) res |= MOUSE_INPUT_5 ;
		if( input.rgbButtons[5] & 0x80 ) res |= MOUSE_INPUT_6 ;
		if( input.rgbButtons[6] & 0x80 ) res |= MOUSE_INPUT_7 ;
		if( input.rgbButtons[7] & 0x80 ) res |= MOUSE_INPUT_8 ;

		InputSysData.MouseMoveZ += ( int )input.lZ ;
	}

	return res ;
}

// 垂直マウスホイールの回転量を取得する
extern int GetMouseWheelRotVol_PF( int CounterReset )
{
	int Vol ;

	// DirectInput を使用しているかどうかで処理を分岐
	if( CheckUseDirectInputMouse() == FALSE )
	{
		Vol = WinData.MouseMoveZ / WHEEL_DELTA ;
		if( CounterReset )
		{
			WinData.MouseMoveZ -= Vol * WHEEL_DELTA ;
		}
	}
	else
	{
		Vol = GetDirectInputMouseMoveZ( CounterReset ) ;
	}

	return Vol ;
}

// 水平マウスホイールの回転量を取得する
extern int GetMouseHWheelRotVol_PF( int CounterReset )
{
	int Vol ;

	Vol = WinData.MouseMoveHZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		WinData.MouseMoveHZ -= Vol * WHEEL_DELTA ;
	}

	return Vol ;
}

// 垂直マウスホイールの回転量を取得する( 戻り値が float 型 )
extern float GetMouseWheelRotVolF_PF( int CounterReset )
{
	float Vol ;

	// DirectInput を使用しているかどうかで処理を分岐
	if( CheckUseDirectInputMouse() == FALSE )
	{
		Vol = ( float )WinData.MouseMoveZ / WHEEL_DELTA ;
		if( CounterReset )
		{
			WinData.MouseMoveZ = 0 ;
		}
	}
	else
	{
		Vol = GetDirectInputMouseMoveZF( CounterReset ) ;
	}

	return Vol ;
}

// 水平マウスホイールの回転量を取得する( 戻り値が float 型 )
extern float GetMouseHWheelRotVolF_PF( int CounterReset )
{
	float Vol ;

	Vol = ( float )WinData.MouseMoveHZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		WinData.MouseMoveHZ = 0 ;
	}

	return Vol ;
}

// ジョイパッドの無効ゾーンの設定を行う関数の環境依存処理
extern int SetJoypadDeadZone_PF( INPUTPADDATA *pad )
{
	D_DIPROPDWORD dipdw ;

	// エラーチェック
	if( pad->PF.Device == NULL  )
	{
		return 0 ;
	}

	// Ｘの無効ゾーンを設定
	dipdw.diph.dwSize		= sizeof( dipdw ) ;
	dipdw.diph.dwHeaderSize	= sizeof( dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_X ;
	dipdw.diph.dwHow		= D_DIPH_BYOFFSET ;
	dipdw.dwData			= 10000 * pad->DeadZone / 65536 ;
	pad->PF.Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// Ｙの無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_Y ;
	pad->PF.Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// Ｚの無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_Z ;
	pad->PF.Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// Ｚ回転の無効ゾーンを設定
	dipdw.diph.dwObj		= D_DIJOFS_RZ ;
	pad->PF.Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// 正常終了
	return 0 ;
}

// キーボードの協調レベルを排他レベルにするかどうかのフラグをセットする
extern int NS_SetKeyExclusiveCooperativeLevelFlag( int Flag )
{
	int hr ;

	// 初期化されていなかったらフラグだけ保存して終了
	if( InputSysData.PF.KeyboardDeviceObject == NULL )
	{
		InputSysData.PF.KeyExclusiveCooperativeLevelFlag = Flag ;
		return 0 ;
	}

	// 状態が同じだったら何もせず終了
	if( InputSysData.PF.KeyExclusiveCooperativeLevelFlag == Flag )
		return 0 ;

	// 一度アクセス権を解放する
	InputSysData.PF.KeyboardDeviceObject->Unacquire() ;

	// 協調レベルをセット
	if( Flag )
	{
		hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
		if( hr != D_DI_OK )
		{
			hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
		}
	}
	else
	{
		hr = InputSysData.PF.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
	}

	// アクセス権を取得する
	hr = InputSysData.PF.KeyboardDeviceObject->Acquire() ;
	if( hr != D_DI_OK )
	{
		hr = InputSysData.PF.KeyboardDeviceObject->Acquire() ;
		if( hr != D_DI_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"キーボードデバイスの取得に失敗しました" @*/ ) ;
			return -1 ;
		}
	}

	// フラグを保存する
	InputSysData.PF.KeyExclusiveCooperativeLevelFlag = Flag ;

	// 終了
	return 0 ;
}

// キーボードの入力処理に DirectInput を使わないか、フラグをセットする
extern int NS_SetKeyboardNotDirectInputFlag( int Flag )
{
	InputSysData.PF.KeyboardNotUseDirectInputFlag = Flag ? TRUE : FALSE ;

	// 終了
	return 0 ;
}

// 入力処理に DirectInput を使用するかどうかのフラグをセットする
extern int NS_SetUseDirectInputFlag( int Flag )
{
	InputSysData.PF.NoUseDirectInputFlag = !Flag ;
	InputSysData.PF.UseDirectInputFlag = Flag ;

	// 終了
	return 0 ;
}

// Xbox360コントローラの入力処理に XInput を使用するかどうかを設定する( TRUE:XInput を使用する( デフォルト )　　FALSE:XInput を使用しない )
extern int NS_SetUseXInputFlag(	int Flag )
{
	InputSysData.PF.NoUseXInputFlag = !Flag ;

	// 終了
	return 0 ;
}

// ジョイパッドのＧＵIＤを得る
extern int NS_GetJoypadGUID( int PadIndex, GUID *GuidInstanceBuffer, GUID *GuidProductBuffer )
{
	D_DIDEVICEINSTANCEW inst ;

	if( InputSysData.PadNum <= PadIndex )
		return -1 ;

	if( InputSysData.Pad[ PadIndex ].PF.Device == NULL ) return -1 ;

	_MEMSET( &inst, 0, sizeof( inst ) ) ;
	inst.dwSize = sizeof( inst ) ;
	InputSysData.Pad[ PadIndex ].PF.Device->GetDeviceInfo( &inst ) ;
	if( GuidInstanceBuffer != NULL ) *GuidInstanceBuffer = inst.guidInstance ;
	if( GuidProductBuffer  != NULL ) *GuidProductBuffer  = inst.guidProduct ;

	return 0 ;
}

// ジョイパッドのデバイス登録名と製品登録名を取得する
extern int NS_GetJoypadName( int InputType, TCHAR *InstanceNameBuffer, TCHAR *ProductNameBuffer )
{
#ifdef UNICODE
	return GetJoypadName_WCHAR_T( InputType, InstanceNameBuffer, ProductNameBuffer ) ;
#else
	int Result ;
	wchar_t TempInstanceNameBuffer[ MAX_PATH ] ;
	wchar_t TempProductNameBuffer[ MAX_PATH ] ;

	Result = GetJoypadName_WCHAR_T( InputType, TempInstanceNameBuffer, TempProductNameBuffer ) ;

	if( InstanceNameBuffer )
	{
		ConvString( ( const char * )TempInstanceNameBuffer, -1, WCHAR_T_CHARCODEFORMAT, InstanceNameBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;
	}

	if( ProductNameBuffer )
	{
		ConvString( ( const char * )TempProductNameBuffer, -1, WCHAR_T_CHARCODEFORMAT, ProductNameBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;
	}

	return Result ;
#endif
}

// ジョイパッドのデバイス登録名と製品登録名を取得する
extern int GetJoypadName_WCHAR_T( int InputType, wchar_t *InstanceNameBuffer, wchar_t *ProductNameBuffer )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	INPUTPADDATA *pad = &InputSysData.Pad[ JoypadNum ] ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;
	
	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	// エラーチェック
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum || ( pad->PF.Device == NULL && pad->PF.XInputDeviceNo < 0 ) )
		return -1 ;

	// 名前をコピー
	if( InstanceNameBuffer != NULL ) _WCSCPY( InstanceNameBuffer, pad->PF.InstanceName ) ;
	if( ProductNameBuffer  != NULL ) _WCSCPY( ProductNameBuffer,  pad->PF.ProductName  ) ;

	// 終了
	return 0;
}

// ＤＸライブラリのキーコードから Windows の仮想キーコードを取得する
extern int NS_ConvertKeyCodeToVirtualKey( int KeyCode )
{
	int i ;

	for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
	{
		if( KeyCode == __KeyMap[i][1] ) break ;
	}

	return __KeyMap[i][0] ;
}

//  Windows の仮想キーコード( VK_LEFT など ) に対応するＤＸライブラリのキーコード( KEY_INPUT_A など )を取得する( VirtualKey:変換したいWindowsの仮想キーコード　戻り値：ＤＸライブラリのキーコード )
extern int NS_ConvertVirtualKeyToKeyCode( int VirtualKey )
{
	int i ;

	for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
	{
		if( VirtualKey == __KeyMap[i][0] ) break ;
	}

	return __KeyMap[i][1] ;
}

// キーボードのバッファからデータを取得する処理(バッファを空にするだけだけど・・・)
extern int KeyboradBufferProcess( void )
{
	D_DIDEVICEOBJECTDATA_DX7 data7[5] ;
	D_DIDEVICEOBJECTDATA_DX8 data8[5] ;
	DWORD num ;

	if( InputSysData.PF.KeyboardDeviceObject == NULL )
		return 0 ;

	num = 5 ;
	while( num == 5 )
	{
		if( InputSysData.PF.UseDirectInput8Flag == FALSE )
		{
			if( InputSysData.PF.KeyboardDeviceObject->GetDeviceData( sizeof( D_DIDEVICEOBJECTDATA_DX7 ), data7, &num, 0 ) != D_DI_OK ) return 0 ;
		}
		else
		{
			if( InputSysData.PF.KeyboardDeviceObject->GetDeviceData( sizeof( D_DIDEVICEOBJECTDATA_DX8 ), data8, &num, 0 ) != D_DI_OK ) return 0 ;
		}
//		if( num != 0 ) DXST_LOGFILEFMT_ADDW(( L"押されたか離されたかしたキー:0x%x  0x%x \n", data[0].dwOfs, data[0].dwData & 0xff )) ;
	}
	
	return 0 ;
}

// ＤｉｒｅｃｔＩｎｐｕｔデバイスの取得状態を更新する
extern int RefreshInputDeviceAcquireState( void )
{
	int i, num ;
	INPUTPADDATA *Pad ;
	HRESULT hr ;

	// 初期化判定
	if( InputSysData.PF.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return 0 ;

	// DirectInput を使用していない場合は何もせず終了
	if( InputSysData.PF.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return 0 ;

	// パッドデバイスの再取得
	num = InputSysData.PadNum ;
	Pad = InputSysData.Pad ;
	for( i = 0 ; i < num ; i ++, Pad ++ )
	{
		if( Pad->PF.Device != NULL )
		{
			Pad->PF.Device->Unacquire() ;

			// ソフトのアクティブ状態によって協調レベルを変更
			if( InputSysData.NoUseVibrationFlag == FALSE && NS_GetActiveFlag() == TRUE )
			{
				hr = Pad->PF.Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_FOREGROUND | D_DISCL_EXCLUSIVE ) ;
				if( hr != D_DI_OK )
				{
					hr = Pad->PF.Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
				}
			}
			else
			{
				Pad->PF.Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
			}

			// アクセス権の取得
			if( Pad->PF.Device->Acquire() != D_DI_OK )
			{
				// アクセス権の取得に失敗したら協調レベルを下げる
				Pad->PF.Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
				Pad->PF.Device->Acquire() ;
			}
		}
	}

	// キーボードデバイスの再取得
	if( InputSysData.PF.KeyboardDeviceObject != NULL )
	{
		InputSysData.PF.KeyboardDeviceObject->Unacquire() ;
		InputSysData.PF.KeyboardDeviceObject->Acquire() ;
	}

	// 終了
	return 0 ;
}

// マウスの情報取得にＤｉｒｅｃｔＩｎｐｕｔを使用しているかどうかを取得する( 戻り値  TRUE:DirectInputを使用している  FALSE:DirectInputを使用していない )
extern	int CheckUseDirectInputMouse( int /*IsButton*/ )
{
	if( InputSysData.PF.NoUseDirectInputFlag == TRUE ||
		InputSysData.PF.MouseDeviceObject == NULL ||
		( /* IsButton == FALSE && */ InputSysData.PF.UseDirectInputFlag == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA ) )
	{
		return FALSE ;
	}

	return TRUE ;
}

// ＤｉｒｅｃｔＩｎｐｕｔを使用したマウスホイールの移動値を取得する
extern	int GetDirectInputMouseMoveZ( int CounterReset )
{
	int Result ;

	NS_GetMouseInput() ;

	Result = InputSysData.MouseMoveZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		InputSysData.MouseMoveZ -= Result * WHEEL_DELTA ;
	}

	return Result ;
}

// ＤｉｒｅｃｔＩｎｐｕｔを使用したマウスホイールの移動値を取得する( float版 )
extern	float GetDirectInputMouseMoveZF( int CounterReset )
{
	float Result ;

	NS_GetMouseInput() ;

	Result = ( float )InputSysData.MouseMoveZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		InputSysData.MouseMoveZ = 0 ;
	}

	return Result ;
}




#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_INPUT


