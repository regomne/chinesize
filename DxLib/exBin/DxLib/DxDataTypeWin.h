// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用データタイプ定義ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXDATATYPEWIN_H__
#define __DXDATATYPEWIN_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"

#if defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
struct IUnknown;
#endif
#include <windows.h>

#include <tchar.h>
#include <commctrl.h>

// ライブラリリンク定義--------------------------------------------------------

#ifndef __DX_MAKE
	#ifndef DX_LIB_NOT_DEFAULTPATH
		#ifndef DX_GCC_COMPILE
			#ifndef DX_SRC_COMPILE
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "DxDrawFunc_x64_d.lib"		)		//  描画部分の抜き出し
								#ifdef UNICODE
									#pragma comment( lib, "DxLibW_x64_d.lib"		)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLibW_x64_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#else
									#pragma comment( lib, "DxLib_x64_d.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLib_x64_d.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#endif
							#else // _DEBUG
								#pragma comment( lib, "DxDrawFunc_x64.lib"			)		//  描画部分の抜き出し
								#ifdef UNICODE
									#pragma comment( lib, "DxLibW_x64.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLibW_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#else
									#pragma comment( lib, "DxLib_x64.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLib_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#endif
							#endif // _DEBUG
						#else // _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "DxDrawFunc_d.lib"		)		//  描画部分の抜き出し
								#ifdef UNICODE
									#pragma comment( lib, "DxLibW_d.lib"		)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLibW_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#else
									#pragma comment( lib, "DxLib_d.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLib_d.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#endif
							#else // _DEBUG
								#pragma comment( lib, "DxDrawFunc.lib"			)		//  描画部分の抜き出し
								#ifdef UNICODE
									#pragma comment( lib, "DxLibW.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLibW.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#else
									#pragma comment( lib, "DxLib.lib"			)		//  ＤＸライブラリ使用指定
									#pragma comment( lib, "DxUseCLib.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
								#endif
							#endif // _DEBUG
						#endif // _WIN64
					#else	// _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x64_MDd.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x64_MDd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLibW_vs2015_x64_ItrDbgLv0_MDd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLibW_vs2015_x64_MDd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#else
											#pragma comment( lib, "DxLib_vs2015_x64_MDd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLib_vs2015_x64_ItrDbgLv0_MDd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLib_vs2015_x64_MDd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#endif
									#else // _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x64_MD.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x64_MD.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLibW_vs2015_x64_MD.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxLib_vs2015_x64_MD.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLib_vs2015_x64_MD.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif
									#endif // _DEBUG
								#else // _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x86_MDd.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x86_MDd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLibW_vs2015_x86_ItrDbgLv0_MDd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLibW_vs2015_x86_MDd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#else
											#pragma comment( lib, "DxLib_vs2015_x86_MDd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLib_vs2015_x86_ItrDbgLv0_MDd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLib_vs2015_x86_MDd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#endif
									#else // _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x86_MD.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x86_MD.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLibW_vs2015_x86_MD.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxLib_vs2015_x86_MD.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLib_vs2015_x86_MD.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif
									#endif // _DEBUG
								#endif // _WIN64
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x64_MTd.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x64_MTd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLibW_vs2015_x64_ItrDbgLv0_MTd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLibW_vs2015_x64_MTd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#else
											#pragma comment( lib, "DxLib_vs2015_x64_MTd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLib_vs2015_x64_ItrDbgLv0_MTd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLib_vs2015_x64_MTd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#endif
									#else // _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x64_MT.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x64_MT.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLibW_vs2015_x64_MT.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxLib_vs2015_x64_MT.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLib_vs2015_x64_MT.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif
									#endif // _DEBUG
								#else // _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x86_MTd.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x86_MTd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLibW_vs2015_x86_ItrDbgLv0_MTd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLibW_vs2015_x86_MTd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#else
											#pragma comment( lib, "DxLib_vs2015_x86_MTd.lib"						)		//  ＤＸライブラリ使用指定
											#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
												#pragma comment( lib, "DxUseCLib_vs2015_x86_ItrDbgLv0_MTd.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#else
												#pragma comment( lib, "DxUseCLib_vs2015_x86_MTd.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
											#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#endif
									#else // _DEBUG
										#pragma comment( lib, "DxDrawFunc_vs2015_x86_MT.lib"						)		//  描画部分の抜き出し
										#ifdef UNICODE
											#pragma comment( lib, "DxLibW_vs2015_x86_MT.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLibW_vs2015_x86_MT.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxLib_vs2015_x86_MT.lib"						)		//  ＤＸライブラリ使用指定
											#pragma comment( lib, "DxUseCLib_vs2015_x86_MT.lib"					)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif
									#endif // _DEBUG
								#endif // _WIN64
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x64_d.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x64_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLibW_vs2013_x64_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLibW_vs2013_x64_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "DxLib_vs2012_x64_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLib_vs2013_x64_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLib_vs2013_x64_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#endif
								#else // _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x64.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x64.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLibW_vs2013_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#else
										#pragma comment( lib, "DxLib_vs2012_x64.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLib_vs2013_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#endif
								#endif // _DEBUG
							#else // _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x86_d.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x86_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLibW_vs2013_x86_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLibW_vs2013_x86_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "DxLib_vs2012_x86_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLib_vs2013_x86_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLib_vs2013_x86_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#endif
								#else // _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x86.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x86.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLibW_vs2013_x86.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#else
										#pragma comment( lib, "DxLib_vs2012_x86.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLib_vs2013_x86.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#endif
								#endif // _DEBUG
							#endif // _WIN64
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x64_d.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x64_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLibW_vs2012_x64_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLibW_vs2012_x64_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "DxLib_vs2012_x64_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLib_vs2012_x64_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLib_vs2012_x64_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#endif
								#else // _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x64.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x64.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLibW_vs2012_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#else
										#pragma comment( lib, "DxLib_vs2012_x64.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLib_vs2012_x64.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#endif
								#endif // _DEBUG
							#else // _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x86_d.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x86_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLibW_vs2012_x86_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLibW_vs2012_x86_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "DxLib_vs2012_x86_d.lib"			)		//  ＤＸライブラリ使用指定
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "DxUseCLib_vs2012_x86_ItrDbgLv0_d.lib"	)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#else
											#pragma comment( lib, "DxUseCLib_vs2012_x86_d.lib"				)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#endif
								#else // _DEBUG
									#pragma comment( lib, "DxDrawFunc_vs2012_x86.lib"			)		//  描画部分の抜き出し
									#ifdef UNICODE
										#pragma comment( lib, "DxLibW_vs2012_x86.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLibW_vs2012_x86.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#else
										#pragma comment( lib, "DxLib_vs2012_x86.lib"			)		//  ＤＸライブラリ使用指定
										#pragma comment( lib, "DxUseCLib_vs2012_x86.lib"		)		//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
									#endif
								#endif // _DEBUG
							#endif // _WIN64
						#endif // // _MSC_VER >= 1700
					#endif // // _MSC_VER <  1700
				#else // _MSC_VER
					#pragma comment( lib, "DxDrawFunc.lib"		)			//  描画部分の抜き出し
					#ifdef UNICODE
						#pragma comment( lib, "DxLibW.lib"		)			//  ＤＸライブラリ使用指定
						#pragma comment( lib, "DxUseCLibW.lib"	)			//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
					#else
						#pragma comment( lib, "DxLib.lib"		)			//  ＤＸライブラリ使用指定
						#pragma comment( lib, "DxUseCLib.lib"	)			//  標準Ｃライブラリを使用する部分の lib ファイルの使用指定
					#endif
				#endif // _MSC_VER

				#ifdef _DEBUG
					#pragma comment( linker, "/NODEFAULTLIB:libcmt.lib" )
					#pragma comment( linker, "/NODEFAULTLIB:libc.lib" )
					#pragma comment( linker, "/NODEFAULTLIB:libcd.lib" )
		//			#pragma comment( linker, "/NODEFAULTLIB:msvcrt.lib" )
		//			#pragma comment( linker, "/NODEFAULTLIB:msvcrtd.lib" )
				#else
					#pragma comment( linker, "/NODEFAULTLIB:libcmtd.lib" )
					#pragma comment( linker, "/NODEFAULTLIB:libc.lib" )
					#pragma comment( linker, "/NODEFAULTLIB:libcd.lib" )
		//			#pragma comment( linker, "/NODEFAULTLIB:msvcrt.lib" )
		//			#pragma comment( linker, "/NODEFAULTLIB:msvcrtd.lib" )
				#endif
			#endif
			//#pragma comment( lib, "libcmt.lib"		)				//  C標準マルチスレッド対応ライブラリ
			#pragma comment( lib, "kernel32.lib"		)			//  Win32カーネルライブラリ
			//#pragma comment( lib, "comctl32.lib"		)			//　Win32APIライブラリ
			#pragma comment( lib, "user32.lib"		)				//  Win32APIライブラリ
			#pragma comment( lib, "gdi32.lib"		)				//  Win32APIライブラリ
			#pragma comment( lib, "advapi32.lib"		)			//  Win32APIライブラリ
			//#pragma comment( lib, "ole32.lib"		)				//  Win32APIライブラリ
			#pragma comment( lib, "shell32.lib"		)				//  マルチメディアライブラリ
			//#pragma comment( lib, "winmm.lib"		)				//  マルチメディアライブラリ
			#ifndef DX_NON_MOVIE
				//#pragma comment( lib, "Strmiids.lib" )			//　DirectShowライブラリ
			#endif
			#ifndef DX_NON_NETWORK
				//#pragma comment( lib, "wsock32.lib" )				//  WinSocketsライブラリ
			#endif
			#ifndef DX_NON_KEYEX
				//#pragma comment( lib, "imm32.lib" )					// ＩＭＥ操作用ライブラリ
			#endif
			#ifndef DX_NON_ACM
				//#pragma comment( lib, "msacm32.lib" )				// ＡＣＭ操作用ライブラリ 
			#endif
			#ifndef DX_NON_BULLET_PHYSICS
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef DX_USE_VC8_BULLET_PHYSICS_LIB
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libbulletcollision_vc8_x64_d.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8_x64_d.lib" )
									#pragma comment( lib, "libbulletmath_vc8_x64_d.lib" )
								#else
									#pragma comment( lib, "libbulletcollision_vc8_x64.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8_x64.lib" )
									#pragma comment( lib, "libbulletmath_vc8_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libbulletcollision_vc8_d.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8_d.lib" )
									#pragma comment( lib, "libbulletmath_vc8_d.lib" )
								#else
									#pragma comment( lib, "libbulletcollision_vc8.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8.lib" )
									#pragma comment( lib, "libbulletmath_vc8.lib" )
								#endif
							#endif
						#else // DX_USE_VC8_BULLET_PHYSICS_LIB
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libbulletcollision_vc8_x64_d.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8_x64_d.lib" )
									#pragma comment( lib, "libbulletmath_vc8_x64_d.lib" )
								#else
									#pragma comment( lib, "libbulletcollision_vc8_x64.lib" )	// Visual C++ 8.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc8_x64.lib" )
									#pragma comment( lib, "libbulletmath_vc8_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libbulletcollision_vc6_d.lib" )	// Visual C++ 6.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc6_d.lib" )
									#pragma comment( lib, "libbulletmath_vc6_d.lib" )
								#else
									#pragma comment( lib, "libbulletcollision_vc6.lib" )	// Visual C++ 6.0 でコンパイルした Bullet Physics ライブラリ 
									#pragma comment( lib, "libbulletdynamics_vc6.lib" )
									#pragma comment( lib, "libbulletmath_vc6.lib" )
								#endif
							#endif
						#endif // DX_USE_VC8_BULLET_PHYSICS_LIB
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "libbulletcollision_vs2015_x64_ItrDbgLv0_MDd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x64_ItrDbgLv0_MDd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x64_ItrDbgLv0_MDd.lib" )
										#else
											#pragma comment( lib, "libbulletcollision_vs2015_x64_MDd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x64_MDd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x64_MDd.lib" )
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "libbulletcollision_vs2015_x64_MD.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2015_x64_MD.lib" )
										#pragma comment( lib, "libbulletmath_vs2015_x64_MD.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "libbulletcollision_vs2015_x86_ItrDbgLv0_MDd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x86_ItrDbgLv0_MDd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x86_ItrDbgLv0_MDd.lib" )
										#else
											#pragma comment( lib, "libbulletcollision_vs2015_x86_MDd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x86_MDd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x86_MDd.lib" )
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "libbulletcollision_vs2015_x86_MD.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2015_x86_MD.lib" )
										#pragma comment( lib, "libbulletmath_vs2015_x86_MD.lib" )
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "libbulletcollision_vs2015_x64_ItrDbgLv0_MTd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x64_ItrDbgLv0_MTd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x64_ItrDbgLv0_MTd.lib" )
										#else
											#pragma comment( lib, "libbulletcollision_vs2015_x64_MTd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x64_MTd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x64_MTd.lib" )
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "libbulletcollision_vs2015_x64_MT.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2015_x64_MT.lib" )
										#pragma comment( lib, "libbulletmath_vs2015_x64_MT.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
											#pragma comment( lib, "libbulletcollision_vs2015_x86_ItrDbgLv0_MTd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x86_ItrDbgLv0_MTd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x86_ItrDbgLv0_MTd.lib" )
										#else
											#pragma comment( lib, "libbulletcollision_vs2015_x86_MTd.lib" )
											#pragma comment( lib, "libbulletdynamics_vs2015_x86_MTd.lib" )
											#pragma comment( lib, "libbulletmath_vs2015_x86_MTd.lib" )
										#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
									#else
										#pragma comment( lib, "libbulletcollision_vs2015_x86_MT.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2015_x86_MT.lib" )
										#pragma comment( lib, "libbulletmath_vs2015_x86_MT.lib" )
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#pragma comment( lib, "libbulletcollision_vs2013_x64_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2013_x64_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2013_x64_ItrDbgLv0_d.lib" )
									#else
										#pragma comment( lib, "libbulletcollision_vs2013_x64_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2013_x64_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2013_x64_d.lib" )
									#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
								#else
									#pragma comment( lib, "libbulletcollision_vs2013_x64.lib" )
									#pragma comment( lib, "libbulletdynamics_vs2013_x64.lib" )
									#pragma comment( lib, "libbulletmath_vs2013_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#pragma comment( lib, "libbulletcollision_vs2013_x86_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2013_x86_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2013_x86_ItrDbgLv0_d.lib" )
									#else
										#pragma comment( lib, "libbulletcollision_vs2013_x86_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2013_x86_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2013_x86_d.lib" )
									#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
								#else
									#pragma comment( lib, "libbulletcollision_vs2013_x86.lib" )
									#pragma comment( lib, "libbulletdynamics_vs2013_x86.lib" )
									#pragma comment( lib, "libbulletmath_vs2013_x86.lib" )
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#pragma comment( lib, "libbulletcollision_vs2012_x64_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2012_x64_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2012_x64_ItrDbgLv0_d.lib" )
									#else
										#pragma comment( lib, "libbulletcollision_vs2012_x64_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2012_x64_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2012_x64_d.lib" )
									#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
								#else
									#pragma comment( lib, "libbulletcollision_vs2012_x64.lib" )
									#pragma comment( lib, "libbulletdynamics_vs2012_x64.lib" )
									#pragma comment( lib, "libbulletmath_vs2012_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
										#pragma comment( lib, "libbulletcollision_vs2012_x86_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2012_x86_ItrDbgLv0_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2012_x86_ItrDbgLv0_d.lib" )
									#else
										#pragma comment( lib, "libbulletcollision_vs2012_x86_d.lib" )
										#pragma comment( lib, "libbulletdynamics_vs2012_x86_d.lib" )
										#pragma comment( lib, "libbulletmath_vs2012_x86_d.lib" )
									#endif // defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0
								#else
									#pragma comment( lib, "libbulletcollision_vs2012_x86.lib" )
									#pragma comment( lib, "libbulletdynamics_vs2012_x86.lib" )
									#pragma comment( lib, "libbulletmath_vs2012_x86.lib" )
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER < 1700
				#else // _MSC_VER
					#pragma comment( lib, "libbulletcollision.lib" )	// Bullet Physics ライブラリ 
					#pragma comment( lib, "libbulletdynamics.lib" )
					#pragma comment( lib, "libbulletmath.lib" )
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_TIFFREAD
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "libtiff_x64_d.lib" )		// ＴＩＦＦライブラリ
							#else
								#pragma comment( lib, "libtiff_x64.lib" )		// ＴＩＦＦライブラリ
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "libtiff_d.lib" )			// ＴＩＦＦライブラリ
							#else
								#pragma comment( lib, "libtiff.lib" )			// ＴＩＦＦライブラリ
							#endif
						#endif
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libtiff_vs2015_x64_MDd.lib" )	// ＴＩＦＦライブラリ
									#else
										#pragma comment( lib, "libtiff_vs2015_x64_MD.lib" )	// ＴＩＦＦライブラリ
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libtiff_vs2015_x86_MDd.lib" )		// ＴＩＦＦライブラリ
									#else
										#pragma comment( lib, "libtiff_vs2015_x86_MD.lib" )		// ＴＩＦＦライブラリ
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libtiff_vs2015_x64_MTd.lib" )	// ＴＩＦＦライブラリ
									#else
										#pragma comment( lib, "libtiff_vs2015_x64_MT.lib" )	// ＴＩＦＦライブラリ
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libtiff_vs2015_x86_MTd.lib" )		// ＴＩＦＦライブラリ
									#else
										#pragma comment( lib, "libtiff_vs2015_x86_MT.lib" )		// ＴＩＦＦライブラリ
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libtiff_vs2013_x64_d.lib" )		// ＴＩＦＦライブラリ
								#else
									#pragma comment( lib, "libtiff_vs2013_x64.lib" )		// ＴＩＦＦライブラリ
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libtiff_vs2013_x86_d.lib" )		// ＴＩＦＦライブラリ
								#else
									#pragma comment( lib, "libtiff_vs2013_x86.lib" )		// ＴＩＦＦライブラリ
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libtiff_vs2012_x64_d.lib" )	// ＴＩＦＦライブラリ
								#else
									#pragma comment( lib, "libtiff_vs2012_x64.lib" )	// ＴＩＦＦライブラリ
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libtiff_vs2012_x86_d.lib" )	// ＴＩＦＦライブラリ
								#else
									#pragma comment( lib, "libtiff_vs2012_x86.lib" )	// ＴＩＦＦライブラリ
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER <  1700
				#else // _MSC_VER
					#pragma comment( lib, "libtiff.lib" )			// ＴＩＦＦライブラリ
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_PNGREAD
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "libpng_x64_d.lib" )		// ＰＮＧライブラリ
								#pragma comment( lib, "zlib_x64_d.lib" )
							#else
								#pragma comment( lib, "libpng_x64.lib" )		// ＰＮＧライブラリ
								#pragma comment( lib, "zlib_x64.lib" )
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "libpng_d.lib" )			// ＰＮＧライブラリ
								#pragma comment( lib, "zlib_d.lib" )
							#else
								#pragma comment( lib, "libpng.lib" )			// ＰＮＧライブラリ
								#pragma comment( lib, "zlib.lib" )
							#endif
						#endif
					#else // MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libpng_vs2015_x64_MDd.lib" )	// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x64_MDd.lib" )
									#else
										#pragma comment( lib, "libpng_vs2015_x64_MD.lib" )		// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x64_MD.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libpng_vs2015_x86_MDd.lib" )	// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x86_MDd.lib" )
									#else
										#pragma comment( lib, "libpng_vs2015_x86_MD.lib" )		// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x86_MD.lib" )
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libpng_vs2015_x64_MTd.lib" )	// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x64_MTd.lib" )
									#else
										#pragma comment( lib, "libpng_vs2015_x64_MT.lib" )		// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x64_MT.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libpng_vs2015_x86_MTd.lib" )	// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x86_MTd.lib" )
									#else
										#pragma comment( lib, "libpng_vs2015_x86_MT.lib" )		// ＰＮＧライブラリ
										#pragma comment( lib, "zlib_vs2015_x86_MT.lib" )
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libpng_vs2013_x64_d.lib" )	// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2013_x64_d.lib" )
								#else
									#pragma comment( lib, "libpng_vs2013_x64.lib" )		// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2013_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libpng_vs2013_x86_d.lib" )	// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2013_x86_d.lib" )
								#else
									#pragma comment( lib, "libpng_vs2013_x86.lib" )		// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2013_x86.lib" )
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libpng_vs2012_x64_d.lib" )	// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2012_x64_d.lib" )
								#else
									#pragma comment( lib, "libpng_vs2012_x64.lib" )		// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2012_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libpng_vs2012_x86_d.lib" )	// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2012_x86_d.lib" )
								#else
									#pragma comment( lib, "libpng_vs2012_x86.lib" )		// ＰＮＧライブラリ
									#pragma comment( lib, "zlib_vs2012_x86.lib" )
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER < 1700
				#else // _MSC_VER
					#pragma comment( lib, "libpng.lib" )			// ＰＮＧライブラリ
					#pragma comment( lib, "zlib.lib" )
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_JPEGREAD
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "libjpeg_x64_d.lib" )		// ＪＰＥＧライブラリ
							#else
								#pragma comment( lib, "libjpeg_x64.lib" )		// ＪＰＥＧライブラリ
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "libjpeg_d.lib" )			// ＪＰＥＧライブラリ
							#else
								#pragma comment( lib, "libjpeg.lib" )			// ＪＰＥＧライブラリ
							#endif
						#endif
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libjpeg_vs2015_x64_MDd.lib" )		// ＪＰＥＧライブラリ
									#else
										#pragma comment( lib, "libjpeg_vs2015_x64_MD.lib" )		// ＪＰＥＧライブラリ
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libjpeg_vs2015_x86_MDd.lib" )		// ＪＰＥＧライブラリ
									#else
										#pragma comment( lib, "libjpeg_vs2015_x86_MD.lib" )		// ＪＰＥＧライブラリ
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "libjpeg_vs2015_x64_MTd.lib" )		// ＪＰＥＧライブラリ
									#else
										#pragma comment( lib, "libjpeg_vs2015_x64_MT.lib" )		// ＪＰＥＧライブラリ
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "libjpeg_vs2015_x86_MTd.lib" )		// ＪＰＥＧライブラリ
									#else
										#pragma comment( lib, "libjpeg_vs2015_x86_MT.lib" )		// ＪＰＥＧライブラリ
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libjpeg_vs2013_x64_d.lib" )		// ＪＰＥＧライブラリ
								#else
									#pragma comment( lib, "libjpeg_vs2013_x64.lib" )		// ＪＰＥＧライブラリ
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libjpeg_vs2013_x86_d.lib" )		// ＪＰＥＧライブラリ
								#else
									#pragma comment( lib, "libjpeg_vs2013_x86.lib" )		// ＪＰＥＧライブラリ
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "libjpeg_vs2012_x64_d.lib" )		// ＪＰＥＧライブラリ
								#else
									#pragma comment( lib, "libjpeg_vs2012_x64.lib" )		// ＪＰＥＧライブラリ
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "libjpeg_vs2012_x86_d.lib" )		// ＪＰＥＧライブラリ
								#else
									#pragma comment( lib, "libjpeg_vs2012_x86.lib" )		// ＪＰＥＧライブラリ
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER <  1700
				#else // _MSC_VER
					#pragma comment( lib, "libjpeg.lib" )			// ＪＰＥＧライブラリ
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_OGGVORBIS								// ＯｇｇＶｏｒｂｉｓライブラリ
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "ogg_static_x64_d.lib" )
								#pragma comment( lib, "vorbis_static_x64_d.lib" )
								#pragma comment( lib, "vorbisfile_static_x64_d.lib" )
							#else
								#pragma comment( lib, "ogg_static_x64.lib" )
								#pragma comment( lib, "vorbis_static_x64.lib" )
								#pragma comment( lib, "vorbisfile_static_x64.lib" )
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "ogg_static_d.lib" )
								#pragma comment( lib, "vorbis_static_d.lib" )
								#pragma comment( lib, "vorbisfile_static_d.lib" )
							#else
								#pragma comment( lib, "ogg_static.lib" )
								#pragma comment( lib, "vorbis_static.lib" )
								#pragma comment( lib, "vorbisfile_static.lib" )
							#endif
						#endif
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MDd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x64_MD.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MD.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MD.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MDd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x86_MD.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MD.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MD.lib" )
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MTd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x64_MT.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MT.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MT.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MTd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x86_MT.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MT.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MT.lib" )
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2013_x64_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x64_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x64_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2013_x64.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x64.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2013_x86_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x86_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x86_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2013_x86.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x86.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x86.lib" )
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2012_x64_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x64_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x64_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2012_x64.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x64.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2012_x86_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x86_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x86_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2012_x86.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x86.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x86.lib" )
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER < 1700
				#else // _MSC_VER
					#pragma comment( lib, "ogg_static.lib" )
					#pragma comment( lib, "vorbis_static.lib" )
					#pragma comment( lib, "vorbisfile_static.lib" )
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_OGGTHEORA								// ＯｇｇＴｈｅｏｒａライブラリ
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "ogg_static_x64_d.lib" )
								#pragma comment( lib, "vorbis_static_x64_d.lib" )
								#pragma comment( lib, "vorbisfile_static_x64_d.lib" )

								#pragma comment( lib, "libtheora_static_x64_d.lib" )
							#else
								#pragma comment( lib, "ogg_static_x64.lib" )
								#pragma comment( lib, "vorbis_static_x64.lib" )
								#pragma comment( lib, "vorbisfile_static_x64.lib" )

								#pragma comment( lib, "libtheora_static_x64.lib" )
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "ogg_static_d.lib" )
								#pragma comment( lib, "vorbis_static_d.lib" )
								#pragma comment( lib, "vorbisfile_static_d.lib" )

								#pragma comment( lib, "libtheora_static_d.lib" )
							#else
								#pragma comment( lib, "ogg_static.lib" )
								#pragma comment( lib, "vorbis_static.lib" )
								#pragma comment( lib, "vorbisfile_static.lib" )

								#pragma comment( lib, "libtheora_static.lib" )
							#endif
						#endif
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MDd.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x64_MDd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x64_MD.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MD.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MD.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x64_MD.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MDd.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x86_MDd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x86_MD.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MD.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MD.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x86_MD.lib" )
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MTd.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x64_MTd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x64_MT.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x64_MT.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x64_MT.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x64_MT.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "ogg_static_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MTd.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x86_MTd.lib" )
									#else
										#pragma comment( lib, "ogg_static_vs2015_x86_MT.lib" )
										#pragma comment( lib, "vorbis_static_vs2015_x86_MT.lib" )
										#pragma comment( lib, "vorbisfile_static_vs2015_x86_MT.lib" )

										#pragma comment( lib, "libtheora_static_vs2015_x86_MT.lib" )
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2013_x64_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x64_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x64_d.lib" )

									#pragma comment( lib, "libtheora_static_vs2013_x64_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2013_x64.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x64.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x64.lib" )

									#pragma comment( lib, "libtheora_static_vs2013_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2013_x86_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x86_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x86_d.lib" )

									#pragma comment( lib, "libtheora_static_vs2013_x86_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2013_x86.lib" )
									#pragma comment( lib, "vorbis_static_vs2013_x86.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2013_x86.lib" )

									#pragma comment( lib, "libtheora_static_vs2013_x86.lib" )
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2012_x64_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x64_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x64_d.lib" )

									#pragma comment( lib, "libtheora_static_vs2012_x64_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2012_x64.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x64.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x64.lib" )

									#pragma comment( lib, "libtheora_static_vs2012_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "ogg_static_vs2012_x86_d.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x86_d.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x86_d.lib" )

									#pragma comment( lib, "libtheora_static_vs2012_x86_d.lib" )
								#else
									#pragma comment( lib, "ogg_static_vs2012_x86.lib" )
									#pragma comment( lib, "vorbis_static_vs2012_x86.lib" )
									#pragma comment( lib, "vorbisfile_static_vs2012_x86.lib" )

									#pragma comment( lib, "libtheora_static_vs2012_x86.lib" )
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER < 1700
				#else // _MSC_VER
					#pragma comment( lib, "ogg_static.lib" )
					#pragma comment( lib, "vorbis_static.lib" )
					#pragma comment( lib, "vorbisfile_static.lib" )

					#pragma comment( lib, "libtheora_static.lib" )
				#endif // _MSC_VER
			#endif
			#ifndef DX_NON_OPUS								// Opusライブラリ
				#ifdef _MSC_VER
					#if _MSC_VER <  1700
						#ifdef _WIN64
							#ifdef _DEBUG
								#pragma comment( lib, "opus_x64_d.lib" )
								#pragma comment( lib, "opusfile_x64_d.lib" )
								#pragma comment( lib, "silk_common_x64_d.lib" )
								#pragma comment( lib, "celt_x64_d.lib" )
							#else
								#pragma comment( lib, "opus_x64.lib" )
								#pragma comment( lib, "opusfile_x64.lib" )
								#pragma comment( lib, "silk_common_x64.lib" )
								#pragma comment( lib, "celt_x64.lib" )
							#endif
						#else
							#ifdef _DEBUG
								#pragma comment( lib, "opus_d.lib" )
								#pragma comment( lib, "opusfile_d.lib" )
								#pragma comment( lib, "silk_common_d.lib" )
								#pragma comment( lib, "celt_d.lib" )
							#else
								#pragma comment( lib, "opus.lib" )
								#pragma comment( lib, "opusfile.lib" )
								#pragma comment( lib, "silk_common.lib" )
								#pragma comment( lib, "celt.lib" )
							#endif
						#endif
					#else // _MSC_VER <  1700
						#if _MSC_VER >= 1900
							#ifdef _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "opus_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "opusfile_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "silk_common_vs2015_x64_MDd.lib" )
										#pragma comment( lib, "celt_vs2015_x64_MDd.lib" )
									#else
										#pragma comment( lib, "opus_vs2015_x64_MD.lib" )
										#pragma comment( lib, "opusfile_vs2015_x64_MD.lib" )
										#pragma comment( lib, "silk_common_vs2015_x64_MD.lib" )
										#pragma comment( lib, "celt_vs2015_x64_MD.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "opus_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "opusfile_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "silk_common_vs2015_x86_MDd.lib" )
										#pragma comment( lib, "celt_vs2015_x86_MDd.lib" )
									#else
										#pragma comment( lib, "opus_vs2015_x86_MD.lib" )
										#pragma comment( lib, "opusfile_vs2015_x86_MD.lib" )
										#pragma comment( lib, "silk_common_vs2015_x86_MD.lib" )
										#pragma comment( lib, "celt_vs2015_x86_MD.lib" )
									#endif
								#endif
							#else // _DLL
								#ifdef _WIN64
									#ifdef _DEBUG
										#pragma comment( lib, "opus_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "opusfile_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "silk_common_vs2015_x64_MTd.lib" )
										#pragma comment( lib, "celt_vs2015_x64_MTd.lib" )
									#else
										#pragma comment( lib, "opus_vs2015_x64_MT.lib" )
										#pragma comment( lib, "opusfile_vs2015_x64_MT.lib" )
										#pragma comment( lib, "silk_common_vs2015_x64_MT.lib" )
										#pragma comment( lib, "celt_vs2015_x64_MT.lib" )
									#endif
								#else
									#ifdef _DEBUG
										#pragma comment( lib, "opus_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "opusfile_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "silk_common_vs2015_x86_MTd.lib" )
										#pragma comment( lib, "celt_vs2015_x86_MTd.lib" )
									#else
										#pragma comment( lib, "opus_vs2015_x86_MT.lib" )
										#pragma comment( lib, "opusfile_vs2015_x86_MT.lib" )
										#pragma comment( lib, "silk_common_vs2015_x86_MT.lib" )
										#pragma comment( lib, "celt_vs2015_x86_MT.lib" )
									#endif
								#endif
							#endif // _DLL
						#elif _MSC_VER >= 1800
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "opus_vs2013_x64_d.lib" )
									#pragma comment( lib, "opusfile_vs2013_x64_d.lib" )
									#pragma comment( lib, "silk_common_vs2013_x64_d.lib" )
									#pragma comment( lib, "celt_vs2013_x64_d.lib" )
								#else
									#pragma comment( lib, "opus_vs2013_x64.lib" )
									#pragma comment( lib, "opusfile_vs2013_x64.lib" )
									#pragma comment( lib, "silk_common_vs2013_x64.lib" )
									#pragma comment( lib, "celt_vs2013_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "opus_vs2013_x86_d.lib" )
									#pragma comment( lib, "opusfile_vs2013_x86_d.lib" )
									#pragma comment( lib, "silk_common_vs2013_x86_d.lib" )
									#pragma comment( lib, "celt_vs2013_x86_d.lib" )
								#else
									#pragma comment( lib, "opus_vs2013_x86.lib" )
									#pragma comment( lib, "opusfile_vs2013_x86.lib" )
									#pragma comment( lib, "silk_common_vs2013_x86.lib" )
									#pragma comment( lib, "celt_vs2013_x86.lib" )
								#endif
							#endif
						#elif _MSC_VER >= 1700
							#ifdef _WIN64
								#ifdef _DEBUG
									#pragma comment( lib, "opus_vs2012_x64_d.lib" )
									#pragma comment( lib, "opusfile_vs2012_x64_d.lib" )
									#pragma comment( lib, "silk_common_vs2012_x64_d.lib" )
									#pragma comment( lib, "celt_vs2012_x64_d.lib" )
								#else
									#pragma comment( lib, "opus_vs2012_x64.lib" )
									#pragma comment( lib, "opusfile_vs2012_x64.lib" )
									#pragma comment( lib, "silk_common_vs2012_x64.lib" )
									#pragma comment( lib, "celt_vs2012_x64.lib" )
								#endif
							#else
								#ifdef _DEBUG
									#pragma comment( lib, "opus_vs2012_x86_d.lib" )
									#pragma comment( lib, "opusfile_vs2012_x86_d.lib" )
									#pragma comment( lib, "silk_common_vs2012_x86_d.lib" )
									#pragma comment( lib, "celt_vs2012_x86_d.lib" )
								#else
									#pragma comment( lib, "opus_vs2012_x86.lib" )
									#pragma comment( lib, "opusfile_vs2012_x86.lib" )
									#pragma comment( lib, "silk_common_vs2012_x86.lib" )
									#pragma comment( lib, "celt_vs2012_x86.lib" )
								#endif
							#endif
						#endif // _MSC_VER >= 1700
					#endif // _MSC_VER < 1700
				#else // _MSC_VER
					#pragma comment( lib, "opus.lib" )
					#pragma comment( lib, "opusfile.lib" )
					#pragma comment( lib, "silk_common.lib" )
					#pragma comment( lib, "celt.lib" )
				#endif // _MSC_VER
			#endif
		#endif  // DX_GCC_COMPILE
	#endif	// DX_LIB_NOT_DEFAULTPATH
#endif // __DX_MAKE

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 環境依存キーワードなど
#ifndef FASTCALL
#define FASTCALL	__fastcall
#endif

// ＤｉｒｅｃｔＩｎｐｕｔのバージョン設定
#define DIRECTINPUT_VERSION 0x700

#ifndef DWORD_PTR
#ifdef _WIN64
#define DWORD_PTR	ULONGLONG
#else
#define DWORD_PTR	DWORD
#endif
#endif

#ifndef LONG_PTR
#ifdef _WIN64
#define LONG_PTR	__int64
#else
#define LONG_PTR	int
#endif
#endif

#define DX_READSOUNDFUNCTION_ACM					(1 << ( DX_READSOUNDFUNCTION_DEFAULT_NUM + 0 ))		// ACM を使用した読み込み処理
#define DX_READSOUNDFUNCTION_MP3					(1 << ( DX_READSOUNDFUNCTION_DEFAULT_NUM + 1 ))		// ACM を使用した MP3 の読み込み処理
#define DX_READSOUNDFUNCTION_DSMP3					(1 << ( DX_READSOUNDFUNCTION_DEFAULT_NUM + 2 ))		// DirectShow を使用した MP3 の読み込み処理
#define DX_READSOUNDFUNCTION_MF						(1 << ( DX_READSOUNDFUNCTION_DEFAULT_NUM + 3 ))		// Media Foundation を使用した読み込み処理

// Direct3D9 用テクスチャフォーマット
#define DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8				(1)
#define DX_TEXTUREFORMAT_DIRECT3D9_A8R8G8B8				(2)
#define DX_TEXTUREFORMAT_DIRECT3D9_X8R8G8B8				(3)
#define DX_TEXTUREFORMAT_DIRECT3D9_R5G6B5				(4)
#define DX_TEXTUREFORMAT_DIRECT3D9_X1R5G5B5				(5)
#define DX_TEXTUREFORMAT_DIRECT3D9_A1R5G5B5				(6)
#define DX_TEXTUREFORMAT_DIRECT3D9_A4R4G4B4				(7)
#define DX_TEXTUREFORMAT_DIRECT3D9_R3G3B2				(8)
#define DX_TEXTUREFORMAT_DIRECT3D9_A8R3G3B2				(9)
#define DX_TEXTUREFORMAT_DIRECT3D9_X4R4G4B4				(10)
#define DX_TEXTUREFORMAT_DIRECT3D9_A2B10G10R10			(11)
#define DX_TEXTUREFORMAT_DIRECT3D9_G16R16				(12)
#define DX_TEXTUREFORMAT_DIRECT3D9_A8B8G8R8				(13)
#define DX_TEXTUREFORMAT_DIRECT3D9_X8B8G8R8				(14)
#define DX_TEXTUREFORMAT_DIRECT3D9_A2R10G10B10			(15)
#define DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16			(16)
#define DX_TEXTUREFORMAT_DIRECT3D9_R16F					(17)
#define DX_TEXTUREFORMAT_DIRECT3D9_G16R16F				(18)
#define DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16F		(19)
#define DX_TEXTUREFORMAT_DIRECT3D9_R32F					(20)
#define DX_TEXTUREFORMAT_DIRECT3D9_G32R32F				(21)
#define DX_TEXTUREFORMAT_DIRECT3D9_A32B32G32R32F		(22)

// エラーコード
#define DX_ERRORCODE_WIN_DESKTOP_24BIT_COLOR				(0x01010001)				// デスクトップが２４ビットカラーモードだった
#define DX_ERRORCODE_WIN_DOUBLE_START						(0x01010002)				// 二重起動
#define DX_ERRORCODE_WIN_FAILED_CREATEWINDOW				(0x01010003)				// ウインドウの作成に失敗
#define DX_ERRORCODE_WIN_FAILED_ASYNCLOAD_CREATE_THREAD		(0x01010004)				// 非同期読み込み処理を行うスレッドの立ち上げに失敗

#define DX_ERRORCODE_WIN_FAILED_CREATE_DIRECTDRAW7			(0x01020001)				// DirectDraw7 の取得に失敗
#define DX_ERRORCODE_WIN_FAILED_INITIALIZE_DIRECTDRAW7		(0x01020002)				// DirectDraw7 の初期化に失敗
#define DX_ERRORCODE_WIN_NOT_COMPATIBLE_SCREEN_COLOR_MODE	(0x01020003)				// 非対応の画面カラーモードが指定された
#define DX_ERRORCODE_WIN_FAILED_CHANGE_DISPLAY_SETTINGS		(0x01020004)				// Win32API の ChangeDisplaySettings を使用した画面モードの変更に失敗
	
// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXDATATYPEWIN_H__
