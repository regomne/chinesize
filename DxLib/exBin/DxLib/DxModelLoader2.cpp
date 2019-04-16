// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＭＱＯモデルデータ読み込みプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define __DX_MAKE

#include "DxModelRead.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxMath.h"
#include "DxLog.h"
#include "DxFile.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxChar.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

#define ENDCHECK( Model )		( ( Model->TextNow - Model->Text ) >= Model->TextSize )
#define C16TOI( C )				( C >= 'A' ? C - 'A' + 10 : C - '0' )

// データ宣言 -----------------------------------

// メタセコイアデータ構造体
struct MQO_MODEL
{
	char *Text ;								// テキストデータ
	int TextSize ;								// テキストサイズ
	char *TextNow ;								// 現在解析中のアドレス
} ;

// 関数宣言 -------------------------------------

static int SkipSpace( MQO_MODEL *Model, int ReturnStopFlag = 0 ) ;	// スペースを飛ばす( 1:終端に達した  0:終端には達していない )
static int GetNextString( MQO_MODEL *Model, char *Buffer ) ;		// 次の意味のある文字列を取得する( 1:終端に達した  0:終端には達していない -1:エラー )
static int GetString( MQO_MODEL *Model, char *Buffer ) ;			// 文字列を取得する( -1:エラー )
static char GetNextChar( MQO_MODEL *Model ) ;						// 次の意味のある文字を取得する( -1:エラー )
static int GetInt( MQO_MODEL *Model ) ;								// 整数値を取得する
static float GetFloat( MQO_MODEL *Model ) ;							// 浮動小数点値を取得する
static int SkipChunk( MQO_MODEL *Model ) ;							// 次に登場するチャンクを飛ばす( -1:エラー )

// プログラム -----------------------------------

// 次に登場するチャンクを飛ばす( -1:エラー )
static int SkipChunk( MQO_MODEL *Model )
{
	char Buffer[ 1024 ] ;
	int Count ;

	// 大括弧が出るまでスキップ
	SkipSpace( Model ) ;
	while( !ENDCHECK( Model ) && *Model->TextNow != '{' )
	{
		GetNextString( Model, Buffer ) ;
		SkipSpace( Model ) ;
	}
	if( ENDCHECK( Model ) )
		return -1 ;

	// 閉じ大括弧が来るまで文字列をスキップし続ける
	Model->TextNow ++ ;
	Count = 1 ;
	while( Count > 0 && !ENDCHECK( Model ) )
	{
		if( *Model->TextNow == '{' )
			Count ++ ;

		if( *Model->TextNow == '}' )
			Count -- ;

		GetNextString( Model, Buffer ) ;
		SkipSpace( Model ) ;
	}

	return Count != 0 ? -1 : 0 ;
}

// 浮動小数点値を取得する
static float GetFloat( MQO_MODEL *Model )
{
	char Buffer[ 1024 ] ;

	// 文字列を取得する
	if( GetNextString( Model, Buffer ) != 0 )
		return -1 ;

	// 浮動小数点値に変換する
	return ( float )_ATOF( Buffer ) ;
}

// 整数値を取得する
static int GetInt( MQO_MODEL *Model )
{
	char Buffer[ 1024 ] ;

	// 文字列を取得する
	if( GetNextString( Model, Buffer ) != 0 )
		return -1 ;

	// 数値に変換する
	return _ATOI( Buffer ) ;
}

// 次の意味のある文字を取得する( -1:エラー )
static char GetNextChar( MQO_MODEL *Model )
{
	// スペースを飛ばす
	if( SkipSpace( Model ) == 1 )
		return -1 ;

	// 最初の文字を返す
	return *Model->TextNow ;
}

// 文字列を取得する( -1:エラー )
static int GetString( MQO_MODEL *Model, char *Buffer )
{
	int Len ;

	// 文字列取得
	if( GetNextString( Model, Buffer ) != 0 )
		return -1 ;
	Len = ( int )_STRLEN( Buffer ) ;

	// ダブルコーテーションで括られていなかったらエラー
	if( Buffer[ 0 ] != '\"' || Buffer[ Len - 1 ] != '\"' )
		return -1 ;

	// １バイト分ずらす
	_MEMMOVE( Buffer, Buffer + 1, ( size_t )( Len - 2 ) ) ;
	Buffer[ Len - 2 ] = '\0' ;

	// 終了
	return 0 ;
}

// 次の意味のある文字列を取得する( 1:終端に達した  0:終端には達していない -1:エラー )
static int GetNextString( MQO_MODEL *Model, char *Buffer )
{
	// スペースを飛ばす
	if( SkipSpace( Model ) == 1 )
		return 1 ;

	// 最初の文字によって処理を分岐
	switch( *Model->TextNow )
	{
	case '\"' :		// 文字列
		// 次のダブルコーテーションまで取得
		Model->TextNow ++ ;
		*Buffer = '\"' ;
		Buffer ++ ;
		while( !ENDCHECK( Model ) )
		{
			if( CHECK_SHIFTJIS_2BYTE( *Model->TextNow ) )
			{
				*( ( WORD * )Buffer ) = *( ( WORD * )Model->TextNow ) ;
				Buffer += 2 ;
				Model->TextNow += 2 ;
			}
			else
			{
				if( *Model->TextNow == '\"' ) break ;
				*Buffer = *Model->TextNow ;
				Buffer ++ ;
				Model->TextNow ++ ;
			}
		}
		if( ENDCHECK( Model ) )
			return -1 ;

		// 終端文字のセット
		Buffer[ 0 ] = '\"' ;
		Buffer[ 1 ] = '\0' ;

		// ダブルコーテーションの次の位置に移動
		Model->TextNow ++ ;
		break ;

	case '(' :		// 括弧たち
	case ')' :
	case '{' :
	case '}' :
		Buffer[ 0 ] = *Model->TextNow ;
		Buffer[ 1 ] = '\0' ;
		Model->TextNow ++ ;
		break ;

	case '-' :		// 数値
	case '.' :
	case '0' : case '1' : case '2' : case '3' : case '4' :
	case '5' : case '6' : case '7' : case '8' : case '9' :
		while( !ENDCHECK( Model ) && 
			( *Model->TextNow == '-' || *Model->TextNow == '.' ||
			  *Model->TextNow == '0' || *Model->TextNow == '1' ||
			  *Model->TextNow == '2' || *Model->TextNow == '3' ||
			  *Model->TextNow == '4' || *Model->TextNow == '5' ||
			  *Model->TextNow == '6' || *Model->TextNow == '7' ||
			  *Model->TextNow == '8' || *Model->TextNow == '9' ) )
		{
			*Buffer = *Model->TextNow ;
			Buffer ++ ;
			Model->TextNow ++ ;
		}

		// 終端文字のセット
		*Buffer = '\0' ;
		break ;

	default :		// それ以外
		// アルファベット以外はエラー
		if( ( *Model->TextNow < 'A' || *Model->TextNow > 'Z' ) &&
			( *Model->TextNow < 'a' || *Model->TextNow > 'z' ) )
			return -1 ;

		// アルファベットと数字とアンダーバーのみ取得
		while( !ENDCHECK( Model ) && 
			( ( *Model->TextNow >= 'A' && *Model->TextNow >= 'Z' ) ||
			  ( *Model->TextNow >= 'a' && *Model->TextNow >= 'z' ) ||
			  ( *Model->TextNow >= '0' && *Model->TextNow >= '9' ) ||
			  *Model->TextNow == '_' ) )
		{
			*Buffer = *Model->TextNow ;
			Buffer ++ ;
			Model->TextNow ++ ;
		}

		// 終端文字のセット
		*Buffer = '\0' ;
		break ;
	}

	// 終了
	return 0 ;
}

// スペースを飛ばす
static int SkipSpace( MQO_MODEL *Model, int ReturnStopFlag )
{
	if( ReturnStopFlag )
	{
		while( !ENDCHECK( Model ) &&
			( *Model->TextNow == ' '  ||
			  *Model->TextNow == '\t' ) )
			  Model->TextNow ++ ;
	}
	else
	{
		while( !ENDCHECK( Model ) &&
			( *Model->TextNow == ' '  ||
			  *Model->TextNow == '\t' ||
			  *Model->TextNow == '\r' ||
			  *Model->TextNow == '\n' ) )
			  Model->TextNow ++ ;
	}

	return ENDCHECK( Model ) ? 1 : 0 ;
}

// ＭＱＯファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern int MV1LoadModelToMQO( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread )
{
	int NewHandle = -1 ;
	int ErrorFlag = 1 ;
	int Num, i = 0, j, k, l, uvcount, colcount, mat/*, uv, col*/ ;
	MQO_MODEL MqoModel ;
	MV1_MODEL_R RModel ;
	MV1_TEXTURE_R *Texture ;
	MV1_MATERIAL_R *Material ;
	MV1_FRAME_R *Frame = NULL ;
	MV1_FRAME_R *FrameStack[ 1024 ] ;
	MV1_MESH_R *Mesh ;
	MV1_MESHFACE_R *MeshFace ;
	char String[ 1024 ] ;
	float r, g, b, a, f ;
	DWORD ColorCode ;
	int UseMaterialMap[ 1024 ], UseMaterialList[ 1024 ], UseMaterialTable[ 1024 ], UseMaterialNum, MatIndex ;
	int UseMaterialMapG[ 1024 ] ;
	int Mirror, Mirror_Axis, Depth ;
	BYTE *PositionUnionFlag = NULL ;
	int EnableShading;
	int Shading;

	// 読み込みようデータの初期化
	MV1InitReadModel( &RModel ) ;
	RModel.MeshFaceRightHand = FALSE ;

	// MQOモデルデータを０初期化
	_MEMSET( &MqoModel, 0, sizeof( MqoModel ) ) ;

	// MQOモデルデータの情報をセット
	MqoModel.Text = ( char * )LoadParam->DataBuffer ;
	MqoModel.TextSize = LoadParam->DataSize ;
	MqoModel.TextNow = MqoModel.Text + 43 ;

	// MQOファイルかどうかを確認
	if( _MEMCMP( LoadParam->DataBuffer, "Metasequoia Document\r\nFormat Text Ver 1.0\r\n", 22 ) != 0 )
		return -1 ;

	// モデル名とファイル名と文字コード形式をセット
	RModel.CharCodeFormat = DX_CHARCODEFORMAT_SHIFTJIS ;
	RModel.FilePath = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->FilePath ) + 1 ) * sizeof( wchar_t ) ) ;
	RModel.Name     = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->Name     ) + 1 ) * sizeof( wchar_t ) ) ;
	_WCSCPY( RModel.FilePath, LoadParam->FilePath ) ;
	_WCSCPY( RModel.Name,     LoadParam->Name ) ;

	// 法線の自動生成を使用
	RModel.AutoCreateNormal = TRUE ;

	// マテリアルチャンクの読み込み
	while( SkipSpace( &MqoModel ) == 0 )
	{
		if( GetNextString( &MqoModel, String ) == -1 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x88\x4e\x1f\x67\x5b\x30\x6c\x30\x20\x00\x45\x00\x4f\x00\x46\x00\x20\x00\x67\x30\x59\x30\x0a\x00\x00"/*@ L"MQO Load Error : 予期せぬ EOF です\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// マテリアルチャンクじゃない場合はスキップ
		if( _STRCMP( String, "Material" ) != 0 )
		{
			SkipChunk( &MqoModel ) ;
			continue ;
		}

		// マテリアルの数を取得
		Num = GetInt( &MqoModel ) ;
		if( Num < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x0a\x00\x00"/*@ L"MQO Load Error : マテリアルの数( %d )が不正です\n" @*/, Num ) ) ;
			goto ENDLABEL ;
		}

		// 括弧の中に入る
		GetNextString( &MqoModel, String ) ;
		if( String[ 0 ] != '{' )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\x70\x65\x6e\x30\x8c\x5f\x6b\x30\x20\x00\x7b\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Material 数の後に { がありません\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// マテリアルの数だけ繰り返し
		SkipSpace( &MqoModel ) ;
		for( i = 0 ; i < Num ; i ++ )
		{
			// マテリアルの名前を取得
			if( GetString( &MqoModel, String ) == -1 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\x0d\x54\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の Material 名の取得に失敗しました\n" @*/, i ) ) ;
				goto ENDLABEL ;
			}

			// マテリアルの追加
			Material = MV1RAddMaterial( &RModel, String ) ;
			if( Material == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の Material オブジェクトの追加に失敗しました\n" @*/, i ) ) ;
				goto ENDLABEL ;
			}
			
			// 情報の取得
			r = 1.0f ;
			g = 1.0f ;
			b = 1.0f ;
			a = 1.0f ;
			for(;;)
			{
				static const char *MaterialError1 = "MQO Load Error : No.%d \x82\xcc Material \x82\xcc %s \x82\xcc\x8c\xe3\x82\xc9 ( \x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "MQO Load Error : No.%d の Material の %s の後に ( がありませんでした\n" @*/ ;
				static const char *MaterialError2 = "MQO Load Error : No.%d \x82\xcc Material \x82\xcc %s \x82\xcc\x90\x94\x92\x6c\x82\xcc\x8c\xe3\x82\xc9 ) \x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "MQO Load Error : No.%d の Material の %s の数値の後に ) がありませんでした\n" @*/ ;

				SkipSpace( &MqoModel, 1 ) ;
				if( *MqoModel.TextNow == '\r' ) break ;

				if( GetNextString( &MqoModel, String ) != 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2d\x4e\x6b\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の Material の読み込み中にエラーが発生しました\n" @*/, i )) ;
					goto ENDLABEL ;
				}

				// 両面ポリゴン
				if( _STRCMP( String, "dbls" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "dbls" )) ;
						goto ENDLABEL ;
					}

					Material->TwoSide = GetInt( &MqoModel ) ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "dbls" )) ;
						goto ENDLABEL ;
					}
				}
				else
				// カラー
				if( _STRCMP( String, "col" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "col" )) ;
						goto ENDLABEL ;
					}

					r = GetFloat( &MqoModel ) ;
					g = GetFloat( &MqoModel ) ;
					b = GetFloat( &MqoModel ) ;
					a = GetFloat( &MqoModel ) ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "col" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "dif" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "dif" )) ;
						goto ENDLABEL ;
					}

					f = GetFloat( &MqoModel ) ;
					Material->Diffuse.r = r * f ;
					Material->Diffuse.g = g * f ;
					Material->Diffuse.b = b * f ;
					Material->Diffuse.a = 1.0f ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "dif" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "amb" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "amb" )) ;
						goto ENDLABEL ;
					}

					f = GetFloat( &MqoModel ) ;
					Material->Ambient.r = f ;
					Material->Ambient.g = f ;
					Material->Ambient.b = f ;
					Material->Ambient.a = 0.0f ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "amb" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "emi" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "emi" )) ;
						goto ENDLABEL ;
					}

					f = GetFloat( &MqoModel ) ;
					Material->Emissive.r = r * f ;
					Material->Emissive.g = g * f ;
					Material->Emissive.b = b * f ;
					Material->Emissive.a = 0.0f ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "emi" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "spc" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "spc" )) ;
						goto ENDLABEL ;
					}

					f = GetFloat( &MqoModel ) ;
					Material->Specular.r = f ;
					Material->Specular.g = f ;
					Material->Specular.b = f ;
					Material->Specular.a = 0.0f ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "spc" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "power" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "power" )) ;
						goto ENDLABEL ;
					}

					Material->Power = GetFloat( &MqoModel ) * 4.0f ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "power" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "tex" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "tex" )) ;
						goto ENDLABEL ;
					}

					if( GetString( &MqoModel, String ) == -1 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x74\x00\x65\x00\x78\x00\x20\x00\x6e\x30\x87\x65\x57\x5b\x17\x52\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の tex の文字列読み込みに失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}

					// テクスチャを追加
					Texture = MV1RAddTexture( &RModel, String, String ) ;
					if( Texture == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の テクスチャオブジェクトの作成に失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}
					Material->DiffuseTexNum = 1 ;
					Material->DiffuseTexs[ 0 ] = Texture ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "tex" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "aplane" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "aplane" )) ;
						goto ENDLABEL ;
					}

					if( GetString( &MqoModel, String ) == -1 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x61\x00\x70\x00\x6c\x00\x61\x00\x6e\x00\x65\x00\x20\x00\x6e\x30\x87\x65\x57\x5b\x17\x52\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の aplane の文字列読み込みに失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}

					// テクスチャが無い場合はここで追加
					if( Material->DiffuseTexs[ 0 ] == NULL )
					{
						Texture = MV1RAddTexture( &RModel, String, NULL ) ;
						if( Texture == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の テクスチャオブジェクトの作成に失敗しました\n" @*/, i ) ) ;
							goto ENDLABEL ;
						}
						Material->DiffuseTexNum = 1 ;
						Material->DiffuseTexs[ 0 ] = Texture ;
					}

					// アルファプレーンを追加
#ifndef UNICODE
					Material->DiffuseTexs[ 0 ]->AlphaFileNameA = MV1RAddString(     &RModel, String ) ;
#endif
					Material->DiffuseTexs[ 0 ]->AlphaFileNameW = MV1RAddStringAToW( &RModel, String ) ;
					if( 
#ifndef UNICODE
						Material->DiffuseTexs[ 0 ]->AlphaFileNameA == NULL ||
#endif
						Material->DiffuseTexs[ 0 ]->AlphaFileNameW == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x61\x00\x70\x00\x6c\x00\x61\x00\x6e\x00\x65\x00\x20\x00\x6e\x30\x87\x65\x57\x5b\x17\x52\x6e\x30\xdd\x4f\x58\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の aplane の文字列の保存に失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "aplane" )) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "bump" ) == 0 )
				{
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '(' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError1, i, "bump" )) ;
						goto ENDLABEL ;
					}

					if( GetString( &MqoModel, String ) == -1 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\x62\x00\x75\x00\x6d\x00\x70\x00\x20\x00\x6e\x30\x87\x65\x57\x5b\x17\x52\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の bump の文字列読み込みに失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}

					// テクスチャを追加
					{
						wchar_t StringW[ 1024 ] ;
						wchar_t FileNameW[ 512 ] ;

						ConvString( String, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )StringW, sizeof( StringW ), WCHAR_T_CHARCODEFORMAT ) ;
						AnalysisFileNameAndDirPathW_( StringW, FileNameW, sizeof( FileNameW ), NULL, 0 ) ;
						Texture = MV1RAddTextureW( &RModel, FileNameW, StringW, NULL, TRUE ) ;
					}
					if( Texture == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : No.%d の テクスチャオブジェクトの作成に失敗しました\n" @*/, i ) ) ;
						goto ENDLABEL ;
					}
					Material->NormalTexNum = 1 ;
					Material->NormalTexs[ 0 ] = Texture ;

					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != ')' )
					{
						DXST_LOGFILEFMT_ADDA(( MaterialError2, i, "bump" )) ;
						goto ENDLABEL ;
					}
				}
			}
			Material->Diffuse.a = a ;
		}

		// 括弧の中から出る
		GetNextString( &MqoModel, String ) ;
		if( String[ 0 ] != '}' )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x42\x7d\xef\x7a\x6b\x30\x20\x00\x7d\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Material チャンクの終端に } がありません\n" @*/ )) ;
			goto ENDLABEL ;
		}
		break ;
	}

	// マテリアルが割り当てられていないメッシュ用のマテリアルを追加
	Material = MV1RAddMaterial( &RModel, "NoMaterial" ) ;
	if( Material == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Material オブジェクトの追加に失敗しました\n" @*/, i ) ) ;
		goto ENDLABEL ;
	}
	Material->DiffuseTexNum = 0 ;
	Material->SpecularTexNum = 0 ;
	Material->NormalTexNum = 0 ;
	Material->Diffuse.r = 1.0f ;
	Material->Diffuse.g = 1.0f ;
	Material->Diffuse.b = 1.0f ;
	Material->Diffuse.a = 1.0f ;

	// チャンク読み込みループ
	_MEMSET( UseMaterialMapG, 0, sizeof( UseMaterialMapG ) ) ;
	MqoModel.TextNow = MqoModel.Text + 43 ;
	while( SkipSpace( &MqoModel ) == 0 )
	{
		if( GetNextString( &MqoModel, String ) == -1 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\xe3\x89\x90\x67\x2d\x4e\x6b\x30\x87\x65\x57\x5b\x17\x52\xe3\x89\x90\x67\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : チャンク解析中に文字列解析エラーが発生しました\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// オブジェクトチャンク
		if( _STRCMP( String, "Object" ) == 0 )
		{
			char FrameName[ 512 ] ;
			char FrameNameUTF16LE[ 512 ] ;

			// ミラー情報の初期化
			Mirror = 0 ;
			Mirror_Axis = 0 ;

			// シェーディング情報の初期化
			EnableShading = 0;
			Shading = 0;

			// オブジェクト名を取得
			if( GetString( &MqoModel, FrameName ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x0d\x54\x4d\x52\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object チャンクの名前の取得に失敗しました\n" @*/ )) ;
				goto ENDLABEL ;
			}

			// 括弧の中に入る
			GetNextString( &MqoModel, String ) ;
			if( String[ 0 ] != '{' )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x0d\x54\x6e\x30\x8c\x5f\x6b\x30\x20\x00\x7b\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object チャンク名の後に { がありません\n" @*/ )) ;
				goto ENDLABEL ;
			}

			// 深さを取得する
			Depth = 0 ;
			if( GetNextString( &MqoModel, String ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x64\x00\x65\x00\x70\x00\x74\x00\x68\x00\x20\x00\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object チャンクの depth 取得に失敗しました\n" @*/ )) ;
				goto ENDLABEL ;
			}
			if( _STRCMP( String, "depth" ) == 0 )
			{
				Depth = GetInt( &MqoModel ) ;
			}

			// フレームの追加
			Frame = MV1RAddFrame( &RModel, FrameName, Depth == 0 ? NULL : FrameStack[ Depth - 1 ] ) ;
			if( Frame == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xec\x30\xfc\x30\xe0\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : フレームオブジェクトの追加に失敗しました\n" @*/ )) ;
				goto ENDLABEL ;
			}
			FrameStack[ Depth ] = Frame ;

			ConvString( ( const char * )Frame->NameW, -1, WCHAR_T_CHARCODEFORMAT, FrameNameUTF16LE, sizeof( FrameNameUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;

			// スケールの初期値をセット
			Frame->Scale.x = 1.0f ;
			Frame->Scale.y = 1.0f ;
			Frame->Scale.z = 1.0f ;

			// 親の行列は無視する
//			Frame->IgnoreParentTransform = 1 ;

			// 回転オーダーの初期値をセット
			Frame->RotateOrder = MV1_ROTATE_ORDER_XYZ ;

			// メッシュの追加
			Mesh = MV1RAddMesh( &RModel, Frame ) ;
			if( Mesh == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : メッシュオブジェクトの追加に失敗しました\n" @*/ )) ;
				goto ENDLABEL ;
			}

			// 閉じ括弧が来るまで解析
			for(;;)
			{
				// 次の文字列を読み込み
				if( GetNextString( &MqoModel, String ) != 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\xe3\x89\x90\x67\x2d\x4e\x6b\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの解析中にエラーが発生しました\n" @*/, FrameNameUTF16LE ) ) ;
					goto ENDLABEL ;
				}

				// 閉じ括弧だったら終了
				if( _STRCMP( String, "}" ) == 0 )
				{
					break ;
				}
				else
				// ビジブル値だった場合
				if( _STRCMP( String, "visible" ) == 0 )
				{
					Frame->Visible = GetInt( &MqoModel ) == 15 ? 1 : 0 ;
				}
				else
				// シェーディングだった場合
				if( _STRCMP( String, "shading" ) == 0 )
				{
					EnableShading = 1;
					Shading = GetInt( &MqoModel ) ;

					if( Shading == 0 )
					{
						Frame->SmoothingAngle = 0.0f ;
					}
				}
				else
				// 拡大値だった場合
				if( _STRCMP( String, "scale" ) == 0 )
				{
					Frame->Scale.x = GetFloat( &MqoModel ) ;
					Frame->Scale.y = GetFloat( &MqoModel ) ;
					Frame->Scale.z = GetFloat( &MqoModel ) ;
				}
				else
				// 回転値だった場合
				if( _STRCMP( String, "rotation" ) == 0 )
				{
					Frame->Rotate.y = -GetFloat( &MqoModel ) * DX_PI_F / 180.0f ;
					Frame->Rotate.x = -GetFloat( &MqoModel ) * DX_PI_F / 180.0f ;
					Frame->Rotate.z =  GetFloat( &MqoModel ) * DX_PI_F / 180.0f ;
				}
				else
				// 平行移動値だった場合
				if( _STRCMP( String, "translation" ) == 0 )
				{
					Frame->Translate.x =  GetFloat( &MqoModel ) ;
					Frame->Translate.y =  GetFloat( &MqoModel ) ;
					Frame->Translate.z = -GetFloat( &MqoModel ) ;
				}
				else
				// スムージング角度情報だった場合
				if( _STRCMP( String, "facet" ) == 0 )
				{
					if( EnableShading == 0 || Shading == 1 )
					{
						Frame->SmoothingAngle = ( float )( GetFloat( &MqoModel ) * DX_PI_F / 180.0f ) ;
					}
					else
					{
						Frame->SmoothingAngle = 0.0f ;
					}
				}
				else
				// 鏡面処理だった場合
				if( _STRCMP( String, "mirror" ) == 0 )
				{
					Mirror = GetInt( &MqoModel ) ;
				}
				else
				// 鏡面処理の角度だった場合
				if( _STRCMP( String, "mirror_axis" ) == 0 )
				{
					Mirror_Axis = GetInt( &MqoModel ) ;
				}
				else
				// 頂点情報だった場合
				if( _STRCMP( String, "vertex" ) == 0 )
				{
					// 頂点数を取得
					Mesh->PositionNum = ( DWORD )GetInt( &MqoModel ) ;
					if( ( int )Mesh->PositionNum < 0 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x76\x00\x65\x00\x72\x00\x74\x00\x65\x00\x78\x00\x20\x00\x6e\x30\x70\x65\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの vertex の数の取得に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 頂点データを格納するためのメモリ領域の確保
					Mesh->Positions = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->PositionNum, &RModel.Mem ) ;
					if( Mesh->Positions == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの頂点座標を保存するメモリ領域の確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 括弧の中に入る
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '{' )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x76\x00\x65\x00\x72\x00\x74\x00\x65\x00\x78\x00\x20\x00\x8c\x5f\x6b\x30\x20\x00\x7b\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの vertex 後に { がありません\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 頂点データを取得
					for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++ )
					{
						Mesh->Positions[ i ].x =  GetFloat( &MqoModel ) ;
						Mesh->Positions[ i ].y =  GetFloat( &MqoModel ) ;
						Mesh->Positions[ i ].z = -GetFloat( &MqoModel ) ;
					}

					// 括弧の中から出る
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '}' )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x76\x00\x65\x00\x72\x00\x74\x00\x65\x00\x78\x00\x20\x00\x6e\x30\x42\x7d\xef\x7a\x6b\x30\x20\x00\x7d\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの vertex の終端に } がありません\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}
				}
				else
				if( _STRCMP( String, "face" ) == 0 )
				{
					int FaceNum ;

					// 面の数を取得
					FaceNum = GetInt( &MqoModel ) ;
					if( FaceNum < 0 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x70\x65\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の数の取得に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 面情報を格納するメモリ領域の確保
					if( MV1RSetupMeshFaceBuffer( &RModel, Mesh, FaceNum, 4 ) < 0 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x62\x97\xc5\x60\x31\x58\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの面情報を保存するメモリ領域の確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// ＵＶ値を格納するためのメモリ領域の確保
					Mesh->UVNum[ 0 ] = Mesh->FaceNum * 4 ;
					Mesh->UVs[ 0 ] = ( FLOAT4 * )ADDMEMAREA( sizeof( FLOAT4 ) * Mesh->UVNum[ 0 ], &RModel.Mem ) ;
					if( Mesh->UVs[ 0 ] == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x75\x00\x76\x00\xa7\x5e\x19\x6a\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの uv座標を保存するメモリ領域の確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 頂点カラーを格納するためのメモリ領域の確保
					Mesh->VertexColorNum = Mesh->FaceNum * 4 ;
					Mesh->VertexColors = ( COLOR_F * )ADDMEMAREA( sizeof( COLOR_F ) * Mesh->VertexColorNum, &RModel.Mem ) ;
					if( Mesh->VertexColors == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの 頂点カラーを保存するメモリ領域の確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 頂点カラーの初期化
					for( i = 0 ; i < ( int )Mesh->VertexColorNum ; i ++ )
					{
						Mesh->VertexColors[ i ].r = 1.0f ;
						Mesh->VertexColors[ i ].g = 1.0f ;
						Mesh->VertexColors[ i ].b = 1.0f ;
						Mesh->VertexColors[ i ].a = 1.0f ;
					}

					// 括弧の中に入る
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '{' )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x8c\x5f\x6b\x30\x20\x00\x7b\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face 後に { がありません\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}

					// 面の情報を取得
					MeshFace = Mesh->Faces ;
					uvcount = 0 ;
					colcount = 0 ;
//					col = 0 ;
					_MEMSET( UseMaterialList, 0, sizeof( UseMaterialList ) ) ;
					_MEMSET( UseMaterialMap, 0, sizeof( UseMaterialMap ) ) ;
					UseMaterialNum = 0 ;
					for( i = 0 ; i < ( int )Mesh->FaceNum ; )
					{
						mat = 0 ;
//						uv = 0 ;

						// インデックスの数を取得
						MeshFace->IndexNum = ( DWORD )GetInt( &MqoModel ) ;
						if( Mesh->FaceUnitMaxIndexNum < MeshFace->IndexNum )
						{
							int FaceIndex ;

							FaceIndex = ( int )( MeshFace - Mesh->Faces ) ;
							if( MV1RSetupMeshFaceBuffer( &RModel, Mesh, ( int )Mesh->FaceNum, ( int )MeshFace->IndexNum ) < 0 )
							{
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\x8d\x51\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load : 面情報を格納するメモリの再確保に失敗しました\n" @*/ )) ;
								return -1 ;
							}
							MeshFace = Mesh->Faces + FaceIndex ;
						}

						// 頂点インデックスの取得
						{
							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != 'V' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x02\x98\xb9\x70\x70\x65\x6e\x30\x70\x65\x6e\x30\x8c\x5f\x6b\x30\x20\x00\x56\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の頂点数の数の後に V がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != '(' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x56\x00\x20\x00\x6e\x30\x20\x00\x28\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の V の ( がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++ )
							{
								MeshFace->VertexIndex[ j ] = ( DWORD )GetInt( &MqoModel ) ;
								if( MeshFace->VertexIndex[ j ] == 0xffffffff )
								{
									DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x25\x00\x64\x00\x6a\x75\xee\x76\x6e\x30\x20\x00\x56\x00\x20\x00\x6e\x30\x02\x98\xb9\x70\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の %d番目の V の頂点インデックスが不正です\n" @*/, FrameNameUTF16LE, j ) ) ;
									goto ENDLABEL ;
								}
							}

							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != ')' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x56\x00\x20\x00\x6e\x30\x20\x00\x29\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の V の ) がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}
						}

						// マテリアルインデックスの取得
						SkipSpace( &MqoModel, 1 ) ;
						if( MqoModel.TextNow[ 0 ] == 'M' )
						{
							GetNextString( &MqoModel, String ) ;

							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != '(' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x4d\x00\x20\x00\x6e\x30\x20\x00\x28\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の M の ( がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							MatIndex = GetInt( &MqoModel ) ;
							if( UseMaterialMap[ MatIndex ] == 0 )
							{
								UseMaterialMap[ MatIndex ] = 1 ;
								UseMaterialMapG[ MatIndex ] = 1 ;
								UseMaterialList[ UseMaterialNum ] = MatIndex ;
								UseMaterialTable[ MatIndex ] = UseMaterialNum ;
								UseMaterialNum ++ ;
							}
							MeshFace->MaterialIndex = ( DWORD )UseMaterialTable[ MatIndex ] ;
							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != ')' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x4d\x00\x20\x00\x6e\x30\x20\x00\x29\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の M の ) がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							mat = 1 ;
						}

						// ＵＶ値の取得
						SkipSpace( &MqoModel, 1 ) ;
						if( MqoModel.TextNow[ 0 ] == 'U' && MqoModel.TextNow[ 1 ] == 'V' )
						{
							GetNextString( &MqoModel, String ) ;

							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != '(' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x55\x00\x56\x00\x20\x00\x6e\x30\x20\x00\x28\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の UV の ( がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++, uvcount ++ )
							{
								MeshFace->UVIndex[ 0 ][ j ] = ( DWORD )uvcount ;
								Mesh->UVs[ 0 ][ uvcount ].x = GetFloat( &MqoModel ) ;
								Mesh->UVs[ 0 ][ uvcount ].y = GetFloat( &MqoModel ) ;
							}
							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != ')' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x55\x00\x56\x00\x20\x00\x6e\x30\x20\x00\x29\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の UV の ) がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

//							uv = 1 ;
						}

						// 頂点カラーの取得
						SkipSpace( &MqoModel, 1 ) ;
						if( MqoModel.TextNow[ 0 ] == 'C' && MqoModel.TextNow[ 1 ] == 'O' && MqoModel.TextNow[ 2 ] == 'L' )
						{
							COLOR_F MatColor ;

							GetNextString( &MqoModel, String ) ;

							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != '(' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x43\x00\x4f\x00\x4c\x00\x20\x00\x6e\x30\x20\x00\x28\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の COL の ( がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

							if( mat )
							{
								Material = MV1RGetMaterial( &RModel, UseMaterialList[ MeshFace->MaterialIndex ] ) ;
								MatColor = Material->Diffuse ;
							}
							else
							{
								MatColor.r = 1.0f ;
								MatColor.g = 1.0f ;
								MatColor.b = 1.0f ;
								MatColor.a = 1.0f ;
							}

							for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++, colcount ++ )
							{
								MeshFace->VertexColorIndex[ j ] = ( DWORD )colcount ;
								ColorCode = ( DWORD )GetInt( &MqoModel ) ;
								Mesh->VertexColors[ colcount ].r = MatColor.r * ( ( ColorCode >>  0 ) & 0xff ) / 255.0f ;
								Mesh->VertexColors[ colcount ].g = MatColor.g * ( ( ColorCode >>  8 ) & 0xff ) / 255.0f ;
								Mesh->VertexColors[ colcount ].b = MatColor.b * ( ( ColorCode >> 16 ) & 0xff ) / 255.0f ;
								Mesh->VertexColors[ colcount ].a = MatColor.a * ( ( ColorCode >> 24 ) & 0xff ) / 255.0f ;
							}
							GetNextString( &MqoModel, String ) ;
							if( String[ 0 ] != ')' )
							{
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x20\x00\x55\x00\x56\x00\x20\x00\x6e\x30\x20\x00\x29\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の UV の ) がありません\n" @*/, FrameNameUTF16LE ) ) ;
								goto ENDLABEL ;
							}

//							col = 1 ;
						}
						else
						{
							// マテリアルカラーを頂点カラーとしてセット
							if( mat )
							{
								for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++, colcount ++ )
								{
									MeshFace->VertexColorIndex[ j ] = ( DWORD )colcount ;
									Material = MV1RGetMaterial( &RModel, UseMaterialList[ MeshFace->MaterialIndex ] ) ;
									Mesh->VertexColors[ colcount ] = Material->Diffuse ;
								}
							}
						}

						// マテリアルが無い場合は、マテリアル無し用のマテリアルを割り当てる
						if( mat == 0 )
						{
							MatIndex = ( int )( RModel.MaterialNum - 1 ) ;
							if( UseMaterialMap[ MatIndex ] == 0 )
							{
								UseMaterialMap[ MatIndex ] = 1 ;
								UseMaterialMapG[ MatIndex ] = 1 ;
								UseMaterialList[ UseMaterialNum ] = MatIndex ;
								UseMaterialTable[ MatIndex ] = UseMaterialNum ;
								UseMaterialNum ++ ;
							}
							MeshFace->MaterialIndex = ( DWORD )UseMaterialTable[ MatIndex ] ;
						}

						// 頂点数が３以上では無い場合は場合は面を追加しない
						if(MeshFace->IndexNum >= 3 )
						{
							i ++ ;
							MeshFace ++ ;
						}
						else
						{
							Mesh->FaceNum -- ;
						}
					}

					// 有効な面が一つも無くなったらメッシュを削除する
					if( Mesh->FaceNum == 0 )
					{
						MV1RSubMesh( &RModel, Frame, Mesh ) ;
					}
					else
					{
						// 使用しているマテリアルのセット
						Mesh->MaterialNum = ( DWORD )UseMaterialNum ;
						for( i = 0 ; i < UseMaterialNum ; i ++ )
							Mesh->Materials[ i ] = MV1RGetMaterial( &RModel, UseMaterialList[ i ] ) ;

						// 使用しているマテリアルが透明度１００％だったらメッシュを削除する
						if( UseMaterialNum == 1 && Mesh->Materials[ 0 ]->Diffuse.a == 0.0f )
						{
							MV1RSubMesh( &RModel, Frame, Mesh ) ;
						}
					}

/*					// 頂点カラーが一つも無かったら頂点カラーを無しにする
					if( col == 0 )
					{
						Mesh->VertexColorNum = 0 ;
						Mesh->VertexColors = NULL ;
					}
*/
					// 括弧の中から出る
					GetNextString( &MqoModel, String ) ;
					if( String[ 0 ] != '}' )
					{
						DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\x20\x00\x66\x00\x61\x00\x63\x00\x65\x00\x20\x00\x6e\x30\x42\x7d\xef\x7a\x6b\x30\x20\x00\x7d\x00\x20\x00\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクの face の終端に } がありません\n" @*/, FrameNameUTF16LE ) ) ;
						goto ENDLABEL ;
					}
				}
			}

			// 座標変換パラメータを反映する
			{
				// このフレームでの行列を作成する
				Frame->Matrix = MGetScale( Frame->Scale ) ;
				if( Frame->Rotate.x != 0.0f )
				{
					Frame->Matrix = MMult( Frame->Matrix, MGetRotX( Frame->Rotate.x ) ) ;
				}
				if( Frame->Rotate.y != 0.0f )
				{
					Frame->Matrix = MMult( Frame->Matrix, MGetRotY( Frame->Rotate.y ) ) ;
				}
				if( Frame->Rotate.z != 0.0f )
				{
					Frame->Matrix = MMult( Frame->Matrix, MGetRotZ( Frame->Rotate.z ) ) ;
				}
				Frame->Matrix.m[ 3 ][ 0 ] = Frame->Translate.x ;
				Frame->Matrix.m[ 3 ][ 1 ] = Frame->Translate.y ;
				Frame->Matrix.m[ 3 ][ 2 ] = Frame->Translate.z ;

				// このフレームでの逆行列を作成する
				MATRIX NowFrameInvMatrix ;
				if( Frame->Parent )
				{
					Frame->LocalWorldMatrix = MMult( Frame->Matrix, Frame->Parent->LocalWorldMatrix ) ;
				}
				else
				{
					Frame->LocalWorldMatrix = Frame->Matrix ;
				}
				NowFrameInvMatrix = MInverse( Frame->LocalWorldMatrix ) ;

				// 頂点データに逆行列を掛ける
				for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++ )
				{
					Mesh->Positions[ i ] = VTransform( Mesh->Positions[ i ], NowFrameInvMatrix ) ;
				}

				Frame->Rotate.x = 0.0f ;
				Frame->Rotate.y = 0.0f ;
				Frame->Rotate.z = 0.0f ;
				Frame->Scale.x = 1.0f ;
				Frame->Scale.y = 1.0f ;
				Frame->Scale.z = 1.0f ;
				Frame->Translate.x = 0.0f ;
				Frame->Translate.y = 0.0f ;
				Frame->Translate.z = 0.0f ;
			}

			// ミラーフラグが立っている場合はミラーリングメッシュも追加する
			if( Mirror == 1 && Frame->MeshNum != 0 )
			{
				VECTOR *Positions ;
				MV1_MESHFACE_R *Faces, *OrigFacesT ;
				int TotalMeshNum, SetCount, BitNum ;
				int OrigFaceNum ;

				// ミラーリングする数を算出
				TotalMeshNum = 1 ;
				if( Mirror_Axis & 1 ) TotalMeshNum <<= 1 ;
				if( Mirror_Axis & 2 ) TotalMeshNum <<= 1 ;
				if( Mirror_Axis & 4 ) TotalMeshNum <<= 1 ;

				// 頂点座標を共有するかどうかのフラグを格納するメモリ領域の確保
				PositionUnionFlag = ( BYTE * )DXALLOC( Mesh->PositionNum ) ;
				if( PositionUnionFlag == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\xdf\x30\xe9\x30\xfc\x30\xea\x30\xf3\x30\xb0\x30\x02\x98\xb9\x70\xc7\x30\xfc\x30\xbf\x30\xc5\x60\x31\x58\xd5\x30\xe9\x30\xb0\x30\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクのミラーリング頂点データ情報フラグ格納用メモリの確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
					goto ENDLABEL ;
				}

				// ミラーリングされたメッシュデータを格納するための頂点バッファと面バッファを確保する
				Positions = Mesh->Positions ;
				Mesh->Positions = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->PositionNum * TotalMeshNum, &RModel.Mem ) ;
				if( Mesh->Positions == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\xdf\x30\xe9\x30\xfc\x30\xea\x30\xf3\x30\xb0\x30\x02\x98\xb9\x70\xc7\x30\xfc\x30\xbf\x30\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクのミラーリング頂点データ格納用メモリの確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
					goto ENDLABEL ;
				}
				OrigFaceNum = ( int )Mesh->FaceNum ;
				if( MV1RSetupMeshFaceBuffer( &RModel, Mesh, ( int )( OrigFaceNum * TotalMeshNum ), ( int )Mesh->FaceUnitMaxIndexNum ) < 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE( ( "\x4d\x00\x51\x00\x4f\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4f\x00\x62\x00\x6a\x00\x65\x00\x63\x00\x74\x00\x20\x00\x25\x00\x73\x00\x20\x00\xc1\x30\xe3\x30\xf3\x30\xaf\x30\x6e\x30\xdf\x30\xe9\x30\xfc\x30\xea\x30\xf3\x30\xb0\x30\x62\x97\xc7\x30\xfc\x30\xbf\x30\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"MQO Load Error : Object %s チャンクのミラーリング面データ格納用メモリの確保に失敗しました\n" @*/, FrameNameUTF16LE ) ) ;
					goto ENDLABEL ;
				}

				// 確保した部分に現時点でのデータをコピー
				for( i = 0 ; i < TotalMeshNum ; i ++ )
				{
					_MEMCPY( ( BYTE * )Mesh->Positions + sizeof( VECTOR )         * Mesh->PositionNum * i, Positions, sizeof( VECTOR )         * Mesh->PositionNum ) ;
				}
				for( i = 1 ; i < TotalMeshNum ; i ++ )
				{
					Faces = Mesh->Faces + OrigFaceNum * i ;
					OrigFacesT = Mesh->Faces ;
					for( j = 0 ; j < OrigFaceNum ; j ++, Faces ++, OrigFacesT ++ )
					{
						Faces->IndexNum = OrigFacesT->IndexNum ;
						Faces->PolygonNum = OrigFacesT->PolygonNum ;
						for( k = 0 ; k < ( int )OrigFacesT->IndexNum ; k ++ )
						{
							Faces->VertexIndex[ k ] = OrigFacesT->VertexIndex[ k ] ;
							Faces->NormalIndex[ k ] = OrigFacesT->NormalIndex[ k ] ;
							Faces->VertexColorIndex[ k ] = OrigFacesT->VertexColorIndex[ k ] ;
							for( l = 0 ; l < MV1_READ_MAX_UV_NUM ; l ++ )
							{
								Faces->UVIndex[ l ][ k ] = OrigFacesT->UVIndex[ l ][ k ] ;
							}
						}
						Faces->MaterialIndex = OrigFacesT->MaterialIndex ;
						Faces->Normal = OrigFacesT->Normal ;
					}
				}

				// 頂点データの作成
				SetCount = 1 ;
				for( j = 1 ; j < 8 ; j ++ )
				{
					if( ( j & Mirror_Axis ) != j ) continue ;

					// 立っているビットの数を数える
					BitNum = 0 ;
					if( j & 1 ) BitNum ++ ;
					if( j & 2 ) BitNum ++ ;
					if( j & 4 ) BitNum ++ ;

					// 頂点座標を共有するかどうかのフラグを格納するメモリ領域の初期化
					_MEMSET( PositionUnionFlag, 0, Mesh->PositionNum ) ;

					// 頂点座標を反転

					// X軸を反転
					if( j & 1 )
					{
						Positions = Mesh->Positions + Mesh->PositionNum * SetCount ;
						for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++, Positions ++ )
						{
							if( Positions->x == 0.0f )
								PositionUnionFlag[ i ] ++ ;
							else
								Positions->x = -Positions->x ;
						}
					}

					// Y軸を反転
					if( j & 2 )
					{
						Positions = Mesh->Positions + Mesh->PositionNum * SetCount ;
						for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++, Positions ++ )
						{
							if( Positions->y == 0.0f )
								PositionUnionFlag[ i ] ++ ;
							else
								Positions->y = -Positions->y ;
						}
					}

					// Z軸を反転
					if( j & 4 )
					{
						Positions = Mesh->Positions + Mesh->PositionNum * SetCount ;
						for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++, Positions ++ )
						{
							if( Positions->z == 0.0f )
								PositionUnionFlag[ i ] ++ ;
							else
								Positions->z = -Positions->z ;
						}
					}

					// 面が指定する座標インデックスのセット
					Faces = Mesh->Faces + OrigFaceNum * SetCount ;
					OrigFacesT = Mesh->Faces ;
					for( i = 0 ; i < ( int )OrigFaceNum ; i ++, Faces ++, OrigFacesT ++ )
					{
						if( BitNum & 1 )
						{
							switch( Faces->IndexNum )
							{
							case 3 :
								Faces->VertexIndex[ 0 ]      = OrigFacesT->VertexIndex[ 0 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 0 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 0 ]      = OrigFacesT->NormalIndex[ 0 ] ;
								Faces->UVIndex[ 0 ][ 0 ]     = OrigFacesT->UVIndex[ 0 ][ 0 ] ;
								Faces->VertexColorIndex[ 0 ] = OrigFacesT->VertexColorIndex[ 0 ] ;

								Faces->VertexIndex[ 1 ]      = OrigFacesT->VertexIndex[ 2 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 2 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 1 ]      = OrigFacesT->NormalIndex[ 2 ] ;
								Faces->UVIndex[ 0 ][ 1 ]     = OrigFacesT->UVIndex[ 0 ][ 2 ] ;
								Faces->VertexColorIndex[ 1 ] = OrigFacesT->VertexColorIndex[ 2 ] ;

								Faces->VertexIndex[ 2 ]      = OrigFacesT->VertexIndex[ 1 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 1 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 2 ]      = OrigFacesT->NormalIndex[ 1 ] ;
								Faces->UVIndex[ 0 ][ 2 ]     = OrigFacesT->UVIndex[ 0 ][ 1 ] ;
								Faces->VertexColorIndex[ 2 ] = OrigFacesT->VertexColorIndex[ 1 ] ;
								break ;

							case 4 :
								Faces->VertexIndex[ 0 ]      = OrigFacesT->VertexIndex[ 3 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 3 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 0 ]      = OrigFacesT->NormalIndex[ 3 ] ;
								Faces->UVIndex[ 0 ][ 0 ]     = OrigFacesT->UVIndex[ 0 ][ 3 ] ;
								Faces->VertexColorIndex[ 0 ] = OrigFacesT->VertexColorIndex[ 3 ] ;

								Faces->VertexIndex[ 1 ]      = OrigFacesT->VertexIndex[ 2 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 2 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 1 ]      = OrigFacesT->NormalIndex[ 2 ] ;
								Faces->UVIndex[ 0 ][ 1 ]     = OrigFacesT->UVIndex[ 0 ][ 2 ] ;
								Faces->VertexColorIndex[ 1 ] = OrigFacesT->VertexColorIndex[ 2 ] ;

								Faces->VertexIndex[ 2 ]      = OrigFacesT->VertexIndex[ 1 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 1 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 2 ]      = OrigFacesT->NormalIndex[ 1 ] ;
								Faces->UVIndex[ 0 ][ 2 ]     = OrigFacesT->UVIndex[ 0 ][ 1 ] ;
								Faces->VertexColorIndex[ 2 ] = OrigFacesT->VertexColorIndex[ 1 ] ;

								Faces->VertexIndex[ 3 ]      = OrigFacesT->VertexIndex[ 0 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 0 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 3 ]      = OrigFacesT->NormalIndex[ 0 ] ;
								Faces->UVIndex[ 0 ][ 3 ]     = OrigFacesT->UVIndex[ 0 ][ 0 ] ;
								Faces->VertexColorIndex[ 3 ] = OrigFacesT->VertexColorIndex[ 0 ] ;
								break ;
							}
						}
						else
						{
							switch( Faces->IndexNum )
							{
							case 3 :
								Faces->VertexIndex[ 0 ]      = OrigFacesT->VertexIndex[ 0 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 0 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 0 ]      = OrigFacesT->NormalIndex[ 0 ] ;
								Faces->UVIndex[ 0 ][ 0 ]     = OrigFacesT->UVIndex[ 0 ][ 0 ] ;
								Faces->VertexColorIndex[ 0 ] = OrigFacesT->VertexColorIndex[ 0 ] ;

								Faces->VertexIndex[ 1 ]      = OrigFacesT->VertexIndex[ 1 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 1 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 1 ]      = OrigFacesT->NormalIndex[ 1 ] ;
								Faces->UVIndex[ 0 ][ 1 ]     = OrigFacesT->UVIndex[ 0 ][ 1 ] ;
								Faces->VertexColorIndex[ 1 ] = OrigFacesT->VertexColorIndex[ 1 ] ;

								Faces->VertexIndex[ 2 ]      = OrigFacesT->VertexIndex[ 2 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 2 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 2 ]      = OrigFacesT->NormalIndex[ 2 ] ;
								Faces->UVIndex[ 0 ][ 2 ]     = OrigFacesT->UVIndex[ 0 ][ 2 ] ;
								Faces->VertexColorIndex[ 2 ] = OrigFacesT->VertexColorIndex[ 2 ] ;
								break ;

							case 4 :
								Faces->VertexIndex[ 0 ]      = OrigFacesT->VertexIndex[ 0 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 0 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 0 ]      = OrigFacesT->NormalIndex[ 0 ] ;
								Faces->UVIndex[ 0 ][ 0 ]     = OrigFacesT->UVIndex[ 0 ][ 0 ] ;
								Faces->VertexColorIndex[ 0 ] = OrigFacesT->VertexColorIndex[ 0 ] ;

								Faces->VertexIndex[ 1 ]      = OrigFacesT->VertexIndex[ 1 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 1 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 1 ]      = OrigFacesT->NormalIndex[ 1 ] ;
								Faces->UVIndex[ 0 ][ 1 ]     = OrigFacesT->UVIndex[ 0 ][ 1 ] ;
								Faces->VertexColorIndex[ 1 ] = OrigFacesT->VertexColorIndex[ 1 ] ;

								Faces->VertexIndex[ 2 ]      = OrigFacesT->VertexIndex[ 2 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 2 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 2 ]      = OrigFacesT->NormalIndex[ 2 ] ;
								Faces->UVIndex[ 0 ][ 2 ]     = OrigFacesT->UVIndex[ 0 ][ 2 ] ;
								Faces->VertexColorIndex[ 2 ] = OrigFacesT->VertexColorIndex[ 2 ] ;

								Faces->VertexIndex[ 3 ]      = OrigFacesT->VertexIndex[ 3 ] + ( PositionUnionFlag[ OrigFacesT->VertexIndex[ 3 ] ] != BitNum ? Mesh->PositionNum * SetCount : 0 ) ;
								Faces->NormalIndex[ 3 ]      = OrigFacesT->NormalIndex[ 3 ] ;
								Faces->UVIndex[ 0 ][ 3 ]     = OrigFacesT->UVIndex[ 0 ][ 3 ] ;
								Faces->VertexColorIndex[ 3 ] = OrigFacesT->VertexColorIndex[ 3 ] ;
								break ;
							}
						}
					}

					SetCount ++ ;
				}

				// ポリゴンの数を増やす
				RModel.TriangleNum += Mesh->TriangleNum * ( TotalMeshNum - 1 ) ;
				Mesh->TriangleNum += Mesh->TriangleNum * ( TotalMeshNum - 1 ) ;

				// 座標の数も増やす
				Mesh->PositionNum += Mesh->PositionNum * ( TotalMeshNum - 1 ) ;

				// フラグ格納用のメモリを解放
				DXFREE( PositionUnionFlag ) ;
				PositionUnionFlag = NULL ;
			}
		}
		else
		if( _STRCMP( String, "Eof" ) == 0 )
		{
			break ;
		}
		else
		{
			SkipChunk( &MqoModel ) ;
		}
	}

	// 使用されていないマテリアルはテクスチャ読み込みエラーをなくす為にレイヤーを０にする
	Material = RModel.MaterialFirst ;
	for( i = 0 ; i < ( int )RModel.MaterialNum ; i ++, Material = Material->DataNext )
	{
		if( UseMaterialMapG[ i ] == 0 )
			Material->DiffuseTexNum = 0 ;
	}

	// モデル基データハンドルの作成
	NewHandle = MV1LoadModelToReadModel( &LoadParam->GParam, &RModel, LoadParam->CurrentDir, LoadParam->FileReadFunc, ASyncThread ) ;
	if( NewHandle < 0 ) goto ENDLABEL ;

	// エラーフラグを倒す
	ErrorFlag = 0 ;

ENDLABEL :

	// エラーフラグが立っていたらモデルハンドルを解放
	if( ErrorFlag == 1 && NewHandle != -1 )
	{
		MV1SubModelBase( NewHandle ) ;
		NewHandle = -1 ;
	}

	// メモリの解放
	if( PositionUnionFlag )
	{
		DXFREE( PositionUnionFlag ) ;
		PositionUnionFlag = NULL ;
	}

	// 読み込みようモデルを解放
	MV1TermReadModel( &RModel ) ; 

	// ハンドルを返す
	return NewHandle ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif


