// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		リングバッファプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxRingBuffer.h"
#include "DxStatic.h"
#include "DxLib.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

// プログラム --------------------------------------------------------------------

// リングバッファを初期化する
extern int RingBufInitialize( RINGBUF * RingBuf )
{
	// データの初期化
	_MEMSET( RingBuf, 0, sizeof( RINGBUF ) ) ;
	RingBuf->DataBuffer = NULL ;
	RingBuf->Start = RingBuf->End = 0 ;
	RingBuf->DataLength = 0 ;
	RingBuf->BufferSize = 0 ;
	
	// 終了
	return 0 ;
}

// リングバッファの後始末をする
extern int RingBufTerminate( RINGBUF * RingBuf )
{
	// データが確保されていたら解放する
	if( RingBuf->DataBuffer != NULL ) DXFREE( RingBuf->DataBuffer ) ;
	
	// ゼロ初期化
	_MEMSET( RingBuf, 0, sizeof( RINGBUF ) ) ;
	
	// 終了
	return 0 ;
}

// リングバッファにデータを追加する
extern int RingBufDataAdd( RINGBUF * RingBuf, const void *Data, int Len )
{
	// アドレスチェック
	RingBufAddressCheck( RingBuf ) ;
	
	// データが収まるようにリサイズ
	if( RingBufReSize( RingBuf, Len ) == -1 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xea\x30\xf3\x30\xb0\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x78\x30\x6e\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\xfd\x8f\xa0\x52\x59\x30\x8b\x30\x77\x95\x55\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"リングバッファへのデータの追加に失敗しました(追加する長さ %d byte)" @*/, Len )) ;
		return -1 ;
	}

	// ２回に分けて格納しなければならないかどうかで処理を分岐
	if( RingBuf->End + Len > RingBuf->BufferSize )
	{
		// ２回に別けて格納する場合の処理

		_MEMCPY( (BYTE *)RingBuf->DataBuffer + RingBuf->End, Data,                                                  ( size_t )(         RingBuf->BufferSize - RingBuf->End )   ) ;
		_MEMCPY( RingBuf->DataBuffer,                        (BYTE *)Data + ( RingBuf->BufferSize - RingBuf->End ), ( size_t )( Len - ( RingBuf->BufferSize - RingBuf->End ) ) ) ;
		
		RingBuf->End = Len - ( RingBuf->BufferSize - RingBuf->End ) ;
	}
	else
	{
		// １回で格納する場合の処理
		
		_MEMCPY( (BYTE *)RingBuf->DataBuffer + RingBuf->End, Data, ( size_t )Len ) ;
		
		RingBuf->End += Len ;
	}
	
	// 格納しているデータの量を増やす
	RingBuf->DataLength += Len ;

	// 終了
	return 0 ;
}

// リングバッファからデータを読みこむ
extern int RingBufDataGet( RINGBUF * RingBuf , void *Buf , int Len , int PeekFlag )
{
	// 存在するデータ量より多かったらエラー
	if( RingBuf->DataLength < Len ) return -1 ;

	// アドレスチェック
	RingBufAddressCheck( RingBuf ) ;

	// ２回に別けなければいけないかどうかで処理を分岐
	if( RingBuf->Start + Len > RingBuf->BufferSize )
	{
		// ２回に別ける場合の処理
		_MEMCPY( Buf,                                                    (BYTE *)RingBuf->DataBuffer + RingBuf->Start, ( size_t )(         RingBuf->BufferSize - RingBuf->Start )   ) ;
		_MEMCPY( (BYTE *)Buf + ( RingBuf->BufferSize - RingBuf->Start ), RingBuf->DataBuffer,                          ( size_t )( Len - ( RingBuf->BufferSize - RingBuf->Start ) ) ) ;

		if( PeekFlag == FALSE ) RingBuf->Start = Len - ( RingBuf->BufferSize - RingBuf->Start ) ;
	}
	else
	{
		// １回で格納する場合の処理
		_MEMCPY( Buf, (BYTE *)RingBuf->DataBuffer + RingBuf->Start, ( size_t )Len ) ;
		
		if( PeekFlag == FALSE )  RingBuf->Start += Len ;
	}

	// データの量を減らす
	if( PeekFlag == FALSE )  RingBuf->DataLength -= Len ;

	// 終了
	return 0 ;
}

// リングバッファの有効データ量の取得 
extern int RingBufGetDataLength( RINGBUF * RingBuf )
{
	return RingBuf->DataLength ;
}

// リングバッファの残り空きデータ領域
extern int RingBufGetRestDataLength( RINGBUF * RingBuf )
{
	return RingBuf->BufferSize - RingBuf->DataLength ;
}

// リングバッファの容量を指定量のデータを追加しても収まるサイズに調整する
extern int RingBufReSize( RINGBUF * RingBuf, int AddSize )
{
	// アドレスチェック
	RingBufAddressCheck( RingBuf ) ;

	// 格納するデータが現在のバッファ容量では入らない場合はバッファを再確保する
	if( RingBuf->BufferSize < AddSize + RingBuf->DataLength )
	{
		void *OldBuffer ;
		int OldSize ;

		OldBuffer = RingBuf->DataBuffer ;
		OldSize = RingBuf->BufferSize ;

		// サイズを再設定、バッファを再確保
		RingBuf->BufferSize = RingBuf->DataLength * 3 / 2 + AddSize + 1000 ;
		if( ( RingBuf->DataBuffer = ( char * )DXALLOC( ( size_t )RingBuf->BufferSize ) ) == NULL )
		{
			// 再確保に失敗したらもとのバッファも解放して終了
			DXFREE( OldBuffer ) ;
			_MEMSET( RingBuf, 0, sizeof( RINGBUF ) ) ;
			
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xea\x30\xf3\x30\xb0\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x8d\x51\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\xba\x78\xdd\x4f\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x5f\x30\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"リングバッファの再確保に失敗しました(確保しようとしたサイズ %d byte)" @*/, RingBuf->BufferSize )) ;
			return -1 ;
		}

		// 再確保したメモリ領域に今までのバッファの内容をコピーする
		if( RingBuf->DataLength != 0 )
		{
			if( RingBuf->Start + RingBuf->DataLength > OldSize )
			{
				_MEMCPY( RingBuf->DataBuffer,                                          ( BYTE * )OldBuffer + RingBuf->Start, ( size_t )(                         OldSize - RingBuf->Start )   ) ;
				_MEMCPY( ( BYTE * )RingBuf->DataBuffer + ( OldSize - RingBuf->Start ), OldBuffer,                            ( size_t )( RingBuf->DataLength - ( OldSize - RingBuf->Start ) ) ) ;
			}
			else
			{
				_MEMCPY( RingBuf->DataBuffer, ( BYTE * )OldBuffer + RingBuf->Start, ( size_t )RingBuf->DataLength ) ;
			}
		}

		// アドレスも再設定
		RingBuf->Start = 0 ;
		RingBuf->End = RingBuf->DataLength ;

		// 元々使用していたメモリ領域を解放
		DXFREE( OldBuffer ) ;
	}
	
	// 終了
	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

