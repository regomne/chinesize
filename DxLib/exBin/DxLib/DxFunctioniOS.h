// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		iOS専用関数プロトタイプ宣言用ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXFUNCTIONIOS
#define __DXFUNCTIONIOS

// インクルード ------------------------------------------------------------------
#include "DxLib.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{
    
#endif // DX_NON_NAMESPACE

// マクロ定義---------------------------------------------------------------------

// 振動の種類
#define IOS_VIBRATE_ONE_STRONG_LONG		(4095)
#define IOS_VIBRATE_TWO_STRONG_SHORT	(1011)
#define IOS_VIBRATE_TWO_WEAK_SHORT		(1102)
#define IOS_VIBRATE_ONE_WEAK_SHORT		(1519)
#define IOS_VIBRATE_ONE_MIDDLE_SHORT	(1520)
#define IOS_VIBRATE_THREE_WEAK_SHORT	(1521)

// 構造体定義 --------------------------------------------------------------------

typedef struct tagDEVICEMOTIONINFO_IOS
{
	double				Attitude_Roll ;
	double				Attitude_Pitch ;
	double				Attitude_Yaw ;
	VECTOR_D			RotationRate ;
	VECTOR_D			Gravity ;
	VECTOR_D			UserAcceleration ;
	VECTOR_D			MagneticField ;
} DEVICEMOTIONINFO_IOS ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// アプリの外部データ保存用のディレクトリパスを取得する
extern int GetDocumentsDirPath( TCHAR *PathBuffer, size_t PathBufferBytes ) ;

// アプリのデータ保存用のディレクトリパスを取得する
extern int GetLibraryPreferencesDirPath( TCHAR *PathBuffer, size_t PathBufferBytes ) ;

// アプリのキャッシュファイル保存用のディレクトリパスを取得する
extern int GetLibraryCachesDirPath( TCHAR *PathBuffer, size_t PathBufferBytes ) ;

// アプリの一時ファイル保存用のディレクトリパスを取得する
extern int GetTmpDirPath( TCHAR *PathBuffer, size_t PathBufferBytes ) ;

// ディスプレイの解像度を取得する
extern int GetDisplayResolution_iOS( int *SizeX, int *SizeY ) ;

// 端末に設定されている言語を取得する( 戻り値　-1：エラー　0以上：言語名文字列の格納に必要なバイト数 )
extern int GetLanguage_iOS( TCHAR *StringBuffer, int StringBufferSize ) ;

// 端末に設定されている国を取得する( 戻り値　-1：エラー　0以上：国名文字列の格納に必要なバイト数 )
extern int GetCountry_iOS( TCHAR *StringBuffer, int StringBufferSize ) ;

// 文字列入力ダイアログを出す
// StringBuffer : 入力された文字列を代入するバッファの先頭アドレス
// StringBufferBytes : 入力された文字列を代入するバッファのサイズ( 単位：バイト )
// Title : ダイアログのタイトル
// Message : ダイアログのメッセージ
// IsBlock : 入力が完了するまで関数から出てこないかどうか( 1:入力が完了するまで関数から出てこない  0:ダイアログを表示したら関数から出てくる( 入力完了のチェックは GetStateInputStringDialogBox で行う ) )
// 戻り値 : 0=成功  2=キャンセルされた  -1=エラー発生
extern int StartInputStringDialogBox( TCHAR *StringBuffer, size_t StringBufferBytes, const TCHAR *Title, const TCHAR *Message, int IsBlock ) ;

// 文字列入力ダイアログの処理が終わったかどうかを取得する
// 戻り値： 0=入力中   1=入力完了  2=キャンセルされた
extern int GetStateInputStringDialogBox( void ) ;

// モーションセンサーの情報を取得する
extern int GetDeviceMotionInfo_iOS( DEVICEMOTIONINFO_IOS *DeviceMotionInfo ) ;

// 振動を再生する
// VibrationType : 振動の種類( IOS_VIBRATE_ONE_STRONG_LONG など )
extern int PlayVibration_iOS( int VibrationType ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXFUNCTIONIOS

