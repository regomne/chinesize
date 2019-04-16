// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		文字列入力プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxInputString.h"

#ifndef DX_NON_INPUTSTRING

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxGraphics.h"
#include "DxFont.h"
#include "DxUseCLib.h"
#include "DxLog.h"
#include "DxMath.h"
#include "Windows/DxWindow.h"
#include "Windows/DxWinAPI.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 文字列入力ハンドルの有効性チェック
#define KEYHCHK( HAND, KPOINT )																			\
	  ( ( ( (HAND) & DX_HANDLEERROR_MASK ) != 0 ) ||													\
		( ( (HAND) & DX_HANDLETYPE_MASK ) != DX_HANDLETYPE_MASK_KEYINPUT ) ||								\
		( ( (HAND) & DX_HANDLEINDEX_MASK ) >= MAX_INPUT_NUM ) ||										\
		( ( KPOINT = &CharBuf.InputData[ (HAND) & DX_HANDLEINDEX_MASK ] )->UseFlag == FALSE ) ||		\
		( ( (KPOINT)->ID << DX_HANDLECHECK_ADDRESS ) != ( (HAND) & DX_HANDLECHECK_MASK ) ) )


#define CURSORBRINKWAIT				(500)				// カーソル点滅の間隔

#ifndef WM_IME_REQUEST
#define WM_IME_REQUEST				(0x0288)
#endif
#ifndef IMR_RECONVERTSTRING
#define IMR_RECONVERTSTRING			(0x0004)
#endif
#ifndef SCS_SETRECONVERTSTRING
#define SCS_SETRECONVERTSTRING		(0x00010000)
#endif
#ifndef SCS_QUERYRECONVERTSTRING
#define SCS_QUERYRECONVERTSTRING	(0x00020000)
#endif

// 構造体定義 --------------------------------------------------------------------

typedef struct tagD_RECONVERTSTRING
{
	DWORD dwSize ;
	DWORD dwVersion ;
	DWORD dwStrLen ;
	DWORD dwStrOffset ;
	DWORD dwCompStrLen ;
	DWORD dwCompStrOffset ;
	DWORD dwTargetStrLen ;
	DWORD dwTargetStrOffset ;
} D_RECONVERTSTRING ;

// テーブル-----------------------------------------------------------------------

// 仮想キーコードをコントロール文字コードに変換するためのテーブル
char CtrlCode[ 10 ][ 2 ] =
{
	VK_DELETE	,	CTRL_CODE_DEL		,	// ＤＥＬキー

	VK_LEFT		,	CTRL_CODE_LEFT		,	// ←キー
	VK_RIGHT	,	CTRL_CODE_RIGHT		,	// →キー
	VK_UP		,	CTRL_CODE_UP		,	// ↑キー
	VK_DOWN		,	CTRL_CODE_DOWN		,	// ↓キー

	VK_HOME		,	CTRL_CODE_HOME		,	// ＨＯＭＥキー
	VK_END		,	CTRL_CODE_END		,	// ＥＮＤキー
	VK_PRIOR	,	CTRL_CODE_PAGE_UP	,	// ＰＡＧＥ　ＵＰキー
	VK_NEXT		,	CTRL_CODE_PAGE_DOWN	,	// ＰＡＧＥ　ＤＯＷＮキー
	0			,	0
} ;

// 内部大域変数宣言 --------------------------------------------------------------

CHARBUFFER CharBuf ;										// 文字コードバッファ

// 関数プロトタイプ宣言-----------------------------------------------------------

// 文字コードバッファ操作関係
#ifndef DX_NON_KEYEX
static	int			ResetKeyInputCursorBrinkCount( void ) ;										// キー入力時のカーソル点滅処理のカウンタをリセット
static	int			SetIMEOpenState( int OpenFlag ) ;											// ＩＭＥを使用状態を変更する
static	int			KeyInputSelectAreaDelete( INPUTDATA *Input ) ;								// キー入力の選択範囲を削除する
#endif

// プログラム --------------------------------------------------------------------

DX_UIElementSink::DX_UIElementSink()
{
	RefCount = 1 ;
}


DX_UIElementSink::~DX_UIElementSink()
{
}

HRESULT __stdcall DX_UIElementSink::QueryInterface( REFIID riid, void **ppvObj )
{
	if( ppvObj == NULL )
	{
		return E_INVALIDARG ;
	}

	*ppvObj = NULL ;

	if( IsEqualIID( riid, IID_IUNKNOWN ) ||
		IsEqualIID( riid, IID_ITFUIELEMENTSINK ) )
	{
		*ppvObj = ( void * )this ;
	}

	if( *ppvObj )
	{
		AddRef() ;
		return S_OK ;
	}

	return E_NOINTERFACE ;
}

ULONG __stdcall DX_UIElementSink::AddRef()
{
	RefCount ++ ;
	return RefCount;
}

ULONG __stdcall DX_UIElementSink::Release()
{
	ULONG RetNum ;

	RefCount -- ;
	RetNum = ( ULONG )RefCount ;

	if( RefCount == 0 )
	{
		delete this;
	}

	return RetNum ;
}

static D_ITfUIElement *Tsf_GetUIElement( DWORD UIElementId )
{
	D_ITfUIElementMgr *puiem;
	D_ITfUIElement *pElement = NULL;

	if( CharBuf.ITfThreadMgrEx->QueryInterface( IID_ITFUIELEMENTMGR, ( void ** )&puiem ) >= 0 )
	{
		puiem->GetUIElement( UIElementId, &pElement ) ;
		puiem->Release() ;
	}

	return pElement ;
}

static void Tsf_MakeCandidateStrings( D_ITfCandidateListUIElement* pcandidate )
{
	UINT uIndex = 0 ;
	UINT uCount = 0 ;
	UINT i ;
	DWORD Offset = 0 ;
	BSTR bstr ;
	HRESULT hr ;
	HRESULT hr2 ;

	SETUP_WIN_API

	// 変換候補の情報をリセット
	if( CharBuf.CandidateList )
	{
		DXFREE( CharBuf.CandidateList ) ;
		CharBuf.CandidateList = NULL ;
		CharBuf.CandidateListSize = 0 ;
	}

	hr2 = pcandidate->GetSelection( &uIndex ) ;
	hr = pcandidate->GetCount( &uCount ) ;

	if( uCount == 0 || hr != S_OK || hr2 != S_OK )
	{
		uIndex = 0 ;
		uCount = 0 ;
		CharBuf.CandidateListSize = sizeof( CANDIDATELIST ) - sizeof( DWORD ) ;
	}
	else
	{
		CharBuf.CandidateListSize = sizeof( CANDIDATELIST ) + ( uCount - 1 ) * sizeof( DWORD ) ;
		for( i = 0; i < uCount; i++ )
		{
			hr = pcandidate->GetString( i, &bstr ) ;
			if( bstr != NULL && hr == S_OK )
			{
				CharBuf.CandidateListSize += ( int )( ( _WCSLEN( bstr ) + 1 ) * sizeof( wchar_t ) ) ;
				WinAPIData.Win32Func.SysFreeStringFunc( bstr ) ;
			}
			else
			{
				CharBuf.CandidateListSize += sizeof( wchar_t ) ;
			}
		}
	}

	CharBuf.CandidateList = ( CANDIDATELIST * )DXALLOC( CharBuf.CandidateListSize ) ;
	if( CharBuf.CandidateList == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x22\x6f\x57\x5b\x09\x59\xdb\x63\x19\x50\xdc\x88\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\xba\x78\xdd\x4f\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x5f\x30\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\xd0\x30\xa4\x30\xc8\x30\x20\x00\x29\x00\x00"/*@ L"漢字変換候補を保存するメモリの確保に失敗しました( 確保しようとしたサイズ %d バイト )" @*/, CharBuf.CandidateListSize )) ;
		CharBuf.CandidateListSize = 0 ;
		return ;
	}

	CharBuf.CandidateList->dwCount = uCount ;
	CharBuf.CandidateList->dwSelection = uIndex ;

	Offset = sizeof( CANDIDATELIST ) + ( uCount - 1 ) * sizeof( DWORD ) ;
	for( i = 0; i < uCount; i++ )
	{
		wchar_t *Buffer ;

		CharBuf.CandidateList->dwOffset[ i ] = Offset ;
		Buffer = ( wchar_t * )( ( BYTE * )CharBuf.CandidateList + CharBuf.CandidateList->dwOffset[ i ] ) ;

		hr = pcandidate->GetString( i, &bstr ) ;
		if( bstr != NULL && hr == S_OK )
		{
			_WCSCPY( Buffer, bstr ) ;
			Offset += ( DWORD )( ( _WCSLEN( bstr ) + 1 ) * sizeof( wchar_t ) ) ;
			WinAPIData.Win32Func.SysFreeStringFunc( bstr ) ;
		}
		else
		{
			Buffer[ 0 ] = L'\0' ;
			Offset += sizeof( wchar_t ) ;
		}
	}
}

HRESULT __stdcall DX_UIElementSink::BeginUIElement(DWORD dwUIElementId, BOOL *pbShow)
{
	D_ITfUIElement *pElement = Tsf_GetUIElement( dwUIElementId ) ;
	if( pElement == NULL )
	{
		return E_INVALIDARG ;
	}

//	D_ITfReadingInformationUIElement *preading = NULL ;
	D_ITfCandidateListUIElement *pcandidate = NULL ;

	*pbShow = FALSE ;
//	if( pElement->QueryInterface( IID_ITFREADINGINFORMATIONUIELEMENT, ( void ** )&preading ) >= 0 )
//	{
//		MakeReadingInformationString( preading ) ;
//		preading->Release();
//	}
//	else
	if( pElement->QueryInterface( IID_ITFCANDIDATELISTUIELEMENT, ( void ** )&pcandidate ) >= 0 )
	{
		CharBuf.TsfCandidateRefCount ++ ;
		Tsf_MakeCandidateStrings( pcandidate ) ;
		pcandidate->Release() ;
	}

	pElement->Release() ;
	return S_OK ;
}

HRESULT __stdcall DX_UIElementSink::UpdateUIElement(DWORD dwUIElementId)
{
	D_ITfUIElement *pElement = Tsf_GetUIElement( dwUIElementId ) ;
	if( pElement == NULL )
	{
		return E_INVALIDARG ;
	}

//	D_ITfReadingInformationUIElement *preading = NULL ;
	D_ITfCandidateListUIElement *pcandidate = NULL ;

//	if( pElement->QueryInterface( IID_ITFREADINGINFORMATIONUIELEMENT, ( void ** )&preading ) >= 0 )
//	{
//		MakeReadingInformationString( preading ) ;
//		preading->Release();
//	}
//	else
	if( pElement->QueryInterface( IID_ITFCANDIDATELISTUIELEMENT, ( void ** )&pcandidate ) >= 0 )
	{
		Tsf_MakeCandidateStrings( pcandidate ) ;
		pcandidate->Release() ;
	}

    pElement->Release() ;
    return S_OK ;
}

HRESULT __stdcall DX_UIElementSink::EndUIElement(DWORD dwUIElementId)
{
	D_ITfUIElement *pElement = Tsf_GetUIElement( dwUIElementId ) ;
	if( pElement == NULL )
	{
		return E_INVALIDARG ;
	}

//    ITfReadingInformationUIElement   *preading = NULL;
//	if (!g_bCandList && SUCCEEDED(pElement->QueryInterface(__uuidof(ITfReadingInformationUIElement),
//                                      (void **)&preading)))
//    {
//		g_dwCount = 0;
//        preading->Release();
//    }

	D_ITfCandidateListUIElement *pcandidate = NULL;
	if( pElement->QueryInterface( IID_ITFCANDIDATELISTUIELEMENT, ( void ** )&pcandidate ) >= 0 )
	{
		CharBuf.TsfCandidateRefCount --;
		if( CharBuf.TsfCandidateRefCount == 0 )
		{
			// メモリの解放
			if( CharBuf.CandidateList != NULL )
			{
				DXFREE( CharBuf.CandidateList ) ;
				CharBuf.CandidateList = NULL ;
			}
			CharBuf.CandidateListSize = 0 ;
		}
		pcandidate->Release();
	}

    pElement->Release() ;

    return S_OK ;
}


// 文字コードバッファ操作関係

// 文字コードバッファ関係の初期化
extern int InitializeInputCharBuf( void )
{
	int IMEUseFlag ;
	int IMEUseFlag_OSSet ;
	int	TSFNotUseFlag ;

	SETUP_WIN_API

	DXST_LOGFILE_ADDUTF16LE( "\x87\x65\x57\x5b\xb3\x30\xfc\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"文字コードバッファの初期化を行います... " @*/ ) ; 

	// ゼロ初期化
	IMEUseFlag               = CharBuf.IMEUseFlag ;
	IMEUseFlag_OSSet         = CharBuf.IMEUseFlag_OSSet ;
	TSFNotUseFlag            = CharBuf.TSFNotUseFlag ;
	_MEMSET( &CharBuf, 0, sizeof( CharBuf ) ) ;
	CharBuf.IMEUseFlag       = IMEUseFlag ;
	CharBuf.IMEUseFlag_OSSet = IMEUseFlag_OSSet ;
	CharBuf.TSFNotUseFlag    = TSFNotUseFlag ;

#ifndef DX_NON_KEYEX
	// デフォルト色をセット
	{
		int i ;
		for( i = 0 ; i < DX_KEYINPSTRCOLOR_NUM ; i ++ )
		{
			NS_ResetKeyInputStringColor2( i ) ;
		}
	}

	// Windows7 以降は TSF の初期化も行う
	if( CharBuf.TSFNotUseFlag == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_7 )
	{
		HRESULT hr ;
		D_TfClientId cid ;
		int Result = -1 ;
		D_ITfSource *TsfSource ;

		hr = WinAPIData.Win32Func.CoCreateInstanceFunc(
			CLSID_TF_THREADMGR, 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			IID_ITFTHREADMGREX,
			( void ** )&CharBuf.ITfThreadMgrEx
		) ;
		if( hr != S_OK )
		{
			goto TSF_SETUP_END ;
		}

		if( CharBuf.ITfThreadMgrEx->ActivateEx( &cid, D_TF_TMAE_UIELEMENTENABLEDONLY ) < 0 )
		{
			goto TSF_SETUP_END ;
		}

		// Setup sinks
		CharBuf.TsfSink = new DX_UIElementSink() ;
		if( CharBuf.TsfSink == NULL )
		{
			goto TSF_SETUP_END ;
		}

		hr = CharBuf.ITfThreadMgrEx->QueryInterface( IID_ITFSOURCE, ( void ** )&TsfSource ) ;
		if( hr < 0 )
		{
			goto TSF_SETUP_END ;
		}

		// Sink for reading window change
		hr = TsfSource->AdviseSink( IID_ITFUIELEMENTSINK, ( D_ITfUIElementSink * )CharBuf.TsfSink, &CharBuf.TsfElementSinkCookie ) ;
		TsfSource->Release() ;
		TsfSource = NULL ;
		if( hr < 0 )
		{
			goto TSF_SETUP_END ;
		}

		Result = 0 ;

TSF_SETUP_END :

		if( Result < 0 )
		{
			if( CharBuf.TsfSink != NULL )
			{
				CharBuf.TsfSink->Release() ;
				CharBuf.TsfSink = NULL ;
			}

			if( CharBuf.ITfThreadMgrEx != NULL )
			{
				CharBuf.ITfThreadMgrEx->Release() ;
				CharBuf.ITfThreadMgrEx = NULL ;
			}
		}
	}

#endif // DX_NON_KEYEX

	// 描画に使用するフォントはデフォルトフォント
	CharBuf.UseFontHandle = -1 ;

	// 入力アクティブハンドルを初期化
	CharBuf.ActiveInputHandle = -1 ;
	
	// カーソル点滅処理関係の初期化
	CharBuf.CBrinkFlag = FALSE ;
	CharBuf.CBrinkCount = 0 ;
	CharBuf.CBrinkWait = CURSORBRINKWAIT ;
	CharBuf.CBrinkDrawFlag = TRUE ;

	DXST_LOGFILE_ADDUTF16LE( "\x8c\x5b\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"完了しました\n" @*/ ) ;

	// 終了
	return 0 ;
}

// 文字コードバッファ関係の後始末
extern int TerminateInputCharBuf( void )
{
#ifndef DX_NON_KEYEX

	if( CharBuf.ITfThreadMgrEx != NULL )
	{
		if( CharBuf.TsfSink != NULL )
		{
			D_ITfSource *TsfSource ;
			HRESULT hr ;

			hr = CharBuf.ITfThreadMgrEx->QueryInterface( IID_ITFSOURCE, ( void ** )&TsfSource ) ;
			if( hr >= 0 )
			{
				TsfSource->UnadviseSink( CharBuf.TsfElementSinkCookie ) ;

				TsfSource->Release() ;
				TsfSource = NULL ;
			}
		}

		CharBuf.ITfThreadMgrEx->Deactivate() ;

		CharBuf.ITfThreadMgrEx->Release() ;
		CharBuf.ITfThreadMgrEx = NULL ;

		if( CharBuf.TsfSink != NULL )
		{
			CharBuf.TsfSink->Release() ;
			CharBuf.TsfSink = NULL ;
		}
	}

#endif // DX_NON_KEYEX

	// 終了
	return 0 ;
}

// バッファにコードをストックする
extern int NS_StockInputChar( TCHAR CharCode )
{
#ifdef UNICODE
	return StockInputChar_WCHAR_T( CharCode ) ;
#else
	int CharBytes ;

	// ストックに代入
	CharBuf.TempStock[ CharBuf.TempStockNum     ] = CharCode ;
	CharBuf.TempStock[ CharBuf.TempStockNum + 1 ] = 0 ;
	CharBuf.TempStockNum ++ ;

	// 文字のバイト数を取得
	CharBytes = GetCharBytes_( CharBuf.TempStock, _TCHARCODEFORMAT ) ;

	// １バイト文字の場合は直ぐに wchar_t版関数に渡す
	if( CharBytes == 1 )
	{
		StockInputChar_WCHAR_T( ( wchar_t )CharCode ) ;
		CharBuf.TempStockNum = 0 ;
	}
	else
	// ２バイト以上の文字の場合は文字のバイト数に達したら wchar_t 文字列に変換して wchar_t版関数に渡す
	if( CharBuf.TempStockNum >= CharBytes )
	{
		wchar_t WCharString[ 16 ] ;
		int StrCharNum ;
		int i ;

		StrCharNum = ConvString( CharBuf.TempStock, -1, _TCHARCODEFORMAT, ( char * )WCharString, sizeof( WCharString ), WCHAR_T_CHARCODEFORMAT ) / sizeof( wchar_t ) - 1 ;
		for( i = 0 ; i < StrCharNum ; i ++ )
		{
			StockInputChar_WCHAR_T( WCharString[ i ] ) ;
		}

		CharBuf.TempStockNum = 0 ;
	}

	return 0 ;
#endif
}


// バッファにコードをストックする
extern int StockInputChar_WCHAR_T( wchar_t CharCode )
{
	// バッファが一杯の場合はなにもしない
	if( ( CharBuf.EdPoint + 1 == CharBuf.StPoint ) ||
		( CharBuf.StPoint == 0 && CharBuf.EdPoint == CHARBUFFER_SIZE ) )
	{
		return -1 ;
	}

	// バッファに文字コードを代入
	CharBuf.CharBuffer[ CharBuf.EdPoint ] = CharCode ;
	CharBuf.EdPoint ++ ;
	if( CharBuf.EdPoint == CHARBUFFER_SIZE + 1 ) CharBuf.EdPoint = 0 ;

	// 終了
	return 0 ;
}

// 文字コードバッファをクリアする
extern int NS_ClearInputCharBuf( void )
{
	SETUP_WIN_API

	while( NS_ProcessMessage() == 0 )
	{
		if( NS_GetInputChar( TRUE ) == 0 ) break ;
	} 

#ifndef UNICODE
	// マルチバイト文字セット版の場合はストックも初期化する
	CharBuf.TempStockNum    = 0 ;
	CharBuf.TempGetStockNum = 0 ;
#endif // UNICODE

	// スタートポインタとエンドポインタを初期化
	CharBuf.EdPoint = CharBuf.StPoint = 0 ;

#ifndef DX_NON_KEYEX
	// IME入力の状態もリセットする
	if( WinData.MainWindow )
	{
		if( CharBuf.IMEUseFlag_OSSet == TRUE && CharBuf.IMESwitch == TRUE )
		{
			HIMC Imc;

			Imc = WinAPIData.ImmFunc.ImmGetContextFunc( WinData.MainWindow ) ;
			if( Imc )
			{
				_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE );
				WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc , NI_COMPOSITIONSTR ,  CPS_CANCEL , 0  );

				WinAPIData.ImmFunc.ImmReleaseContextFunc( WinData.MainWindow , Imc );
			}
		}
	}
#endif

	// 終了
	return 0 ;
}

// 文字コードバッファに溜まったデータから１バイト分取得する
extern TCHAR NS_GetInputChar( int DeleteFlag )
{
#ifdef UNICODE
	return GetInputChar_WCHAR_T( DeleteFlag ) ;
#else
	char Result = 0 ;

	// ストックが無い場合は wchar_t 文字列から文字を取得する
	if( CharBuf.TempGetStockNum == 0 )
	{
		wchar_t WCharString[ 16 ] ;

		do
		{
			// wchar_t のストックから１文字取得
			WCharString[ 0 ] = GetInputChar_WCHAR_T( TRUE ) ;
			WCharString[ 1 ] = L'\0' ;

			// wchar_t のストックが無かったら 0 を返す
			if( WCharString[ 0 ] == 0 )
			{
				return 0 ;
			}

			// ストックがあったらサロゲートペアチェック
			if( CHECK_WCHAR_T_DOUBLE( WCharString[ 0 ] ) )
			{
				// サロゲートペアだったら更に１文字取得
				WCharString[ 1 ] = GetInputChar_WCHAR_T( TRUE ) ;
				WCharString[ 2 ] = L'\0' ;
			}

			// マルチバイト文字列に変換
			CharBuf.TempGetStockNum = ConvString( ( const char * )WCharString, -1, WCHAR_T_CHARCODEFORMAT, CharBuf.TempGetStock, sizeof( CharBuf.TempGetStock ), _TCHARCODEFORMAT ) ;

			// マルチバイト文字列に変換できたらループを抜ける
		}while( CharBuf.TempGetStockNum > 0 ) ;
	}

	// ストックから１文字返す
	Result = CharBuf.TempGetStock[ 0 ] ;
	if( DeleteFlag )
	{
		CharBuf.TempGetStockNum -- ;
		if( CharBuf.TempGetStockNum > 0 )
		{
			_MEMMOVE( &CharBuf.TempGetStock[ 0 ], &CharBuf.TempGetStock[ 1 ], sizeof( char ) * CharBuf.TempGetStockNum ) ;
		}
	}

	return Result ;
#endif
}

// 文字コードバッファに溜まったデータから１バイト分取得する
extern wchar_t GetInputChar_WCHAR_T( int DeleteFlag )
{
	wchar_t RetChar ;

	// バッファに文字がなかった場合は０を返す
	if( CharBuf.EdPoint == CharBuf.StPoint )
	{
		return 0 ;
	}

	// 返す文字コードをセット
	RetChar = CharBuf.CharBuffer[ CharBuf.StPoint ] ;

	// 有効データスタートポインタをインクリメント
	if( DeleteFlag )
	{
		CharBuf.StPoint ++ ;
		if( CharBuf.StPoint == CHARBUFFER_SIZE + 1 ) CharBuf.StPoint = 0 ;
	}

	if( RetChar == -1 ) 
	{
		DXST_LOGFILE_ADDUTF16LE( "\x87\x65\x57\x5b\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xa8\x30\xe9\x30\xfc\x30\x0a\x00\x00"/*@ L"文字バッファエラー\n" @*/ ) ;
		return ( wchar_t )-1 ;
	}

	// 文字コードを返す
	return RetChar ;
}

// 文字コードバッファに溜まったデータから１文字分取得する
extern int NS_GetOneChar( TCHAR *CharBuffer , int DeleteFlag )
{
#ifdef UNICODE
	return GetOneChar_WCHAR_T( CharBuffer, DeleteFlag ) ;
#else
	int CharBytes ;
	int Result = 0 ;

	// 1バイト目を取得
	CharBuffer[ 0 ] = NS_GetInputChar( FALSE ) ;

	// 何も文字が無かったら 0 を返す
	if( CharBuffer[ 0 ] == 0 )
	{
		return 0 ;
	}

	// 複数バイト文字かどうかで処理を分岐
	CharBytes = GetCharBytes_( CharBuffer, _TCHARCODEFORMAT ) ;
	if( CharBytes == 1 )
	{
		// バッファ削除指定がある場合は削除
		if( DeleteFlag )
		{
			NS_GetInputChar( TRUE ) ;
		}

		// 1 を返す
		return 1 ;
	}

	// 複数バイトの文字をコピー
	_MEMCPY( CharBuffer, CharBuf.TempGetStock, CharBuf.TempGetStockNum ) ;
	Result = CharBuf.TempGetStockNum ;

	// バッファ削除指定がある場合はストックを 0 にする
	if( DeleteFlag )
	{
		CharBuf.TempGetStockNum = 0 ;
	}

	// 文字のバイト数を返す
	return Result ;
#endif
}

// 文字コードバッファに溜まったデータから１文字分取得する
extern int GetOneChar_WCHAR_T( wchar_t *CharBuffer , int DeleteFlag )
{
	int Ret ;

	CharBuffer[ 0 ] = 0 ;
	CharBuffer[ 1 ] = 0 ;

	Ret = 0 ;

	// バッファに文字がなかった場合は０を返す
	if( CharBuf.EdPoint == CharBuf.StPoint )
	{
		return 0 ;
	}

	// １文字目を取得
	CharBuffer[ 0 ] = CharBuf.CharBuffer[ CharBuf.StPoint ] ;
	Ret ++ ;

	// 有効データスタートポインタをインクリメント
	CharBuf.StPoint ++ ;
	if( CharBuf.StPoint == CHARBUFFER_SIZE + 1 )
	{
		CharBuf.StPoint = 0 ;
	}

	// サロゲートペアかどうかをチェック
	if( GetCtrlCodeCmp_WCHAR_T( CharBuffer[ 0 ] ) == 0 && CHECK_WCHAR_T_DOUBLE( *CharBuffer ) )
	{
		// バッファに文字がなかった場合は０を返す	
		if( CharBuf.EdPoint == CharBuf.StPoint )
		{
			// 1 wchar_t 分データスタートポインタを戻す
			if( CharBuf.StPoint == 0 )
			{
				CharBuf.StPoint = CHARBUFFER_SIZE ;
			}
			else
			{
				CharBuf.StPoint -- ;
			}
			
			return 0 ;
		}

		// 2 wchar_t 目を取得
		CharBuffer[ 1 ] = CharBuf.CharBuffer[ CharBuf.StPoint ] ;
		Ret ++ ;

		// 有効データスタートポインタをインクリメント
		if( DeleteFlag )
		{
			CharBuf.StPoint ++ ;
			if( CharBuf.StPoint == CHARBUFFER_SIZE + 1 )
			{
				CharBuf.StPoint = 0 ;
			}
		}
	}

	// もしバッファ削除フラグがっ立っていなければ１バイト分戻す
	if( !DeleteFlag )
	{
		if( CharBuf.StPoint == 0 )
		{
			CharBuf.StPoint = CHARBUFFER_SIZE ;
		}
		else
		{
			CharBuf.StPoint -- ;
		}
	}

	// 取得したバイト数を返す
	return Ret ;
}

// 文字コードバッファに溜まったデータから１バイト分取得する、バッファになにも文字コードがない場合はキーが押されるまで待つ
extern TCHAR NS_GetInputCharWait( int DeleteFlag )
{
#ifdef UNICODE
	return GetInputCharWait_WCHAR_T( DeleteFlag ) ;
#else
	TCHAR RetChar = 0 ;

	// キーバッファに文字コードが溜まるまで待つ
	while( NS_ProcessMessage() == 0 )
	{
		RetChar = NS_GetInputChar( DeleteFlag ) ;
		if( RetChar != 0 )
		{
			break ;
		}
	}

	// キーコードを返す
	return RetChar ;
#endif
}

// 文字コードバッファに溜まったデータから１バイト分取得する、バッファになにも文字コードがない場合はキーが押されるまで待つ
extern wchar_t GetInputCharWait_WCHAR_T( int DeleteFlag )
{
	wchar_t RetChar = 0 ;

	// キーバッファに文字コードが溜まるまで待つ
	while( NS_ProcessMessage() == 0 )
	{
		RetChar = GetInputChar_WCHAR_T( DeleteFlag ) ;
		if( RetChar != 0 )
		{
			break ;
		}
	}

	// キーコードを返す
	return RetChar ;
}

// 文字コードバッファに溜まったデータから１文字分取得する、バッファに何も文字コードがない場合はキーが押されるまで待つ
extern int NS_GetOneCharWait( TCHAR *CharBuffer , int DeleteFlag ) 
{
#ifdef UNICODE
	return GetOneCharWait_WCHAR_T( CharBuffer, DeleteFlag ) ;
#else
	int Ret = 0 ;

	// キーバッファに文字コードが溜まるまで待つ
	while( NS_ProcessMessage() == 0 )
	{
		Ret = NS_GetOneChar( CharBuffer , DeleteFlag ) ;
		if( Ret != 0 )
		{
			break ;
		}
	}

	// 取得したバイト数を返す
	return Ret ;
#endif
}

// 文字コードバッファに溜まったデータから１文字分取得する、バッファに何も文字コードがない場合はキーが押されるまで待つ
extern int GetOneCharWait_WCHAR_T( wchar_t *CharBuffer , int DeleteFlag ) 
{
	int Ret = 0 ;

	// キーバッファに文字コードが溜まるまで待つ
	while( NS_ProcessMessage() == 0 )
	{
		Ret = GetOneChar_WCHAR_T( CharBuffer , DeleteFlag ) ;
		if( Ret != 0 )
		{
			break ;
		}
	}

	// 取得したバイト数を返す
	return Ret ;
}


#ifndef DX_NON_KEYEX

// ＩＭＥメッセージのコールバック関数
extern LRESULT IMEProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HIMC Imc ;
	static int Flag = FALSE ;
	INPUTDATA * Input ;

	SETUP_WIN_API

	// ＩＭＥ無視状態だった場合はなにもせず終了
/*	if( CharBuf.IMEUseFlag == FALSE )
	{
//		WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;
		return 0;
	}
*/
	if( KEYHCHK( CharBuf.ActiveInputHandle, Input ) )
	{
		Input = NULL ;
	}

	// 変化があったフラグセット
	CharBuf.ChangeFlag = TRUE ;

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;

	// スイッチ
	switch( message )
	{
	// ＩＭＥリクエスト
	case WM_IME_REQUEST :
		if( Input != NULL && wParam == IMR_RECONVERTSTRING )
		{
			int smin = 0 ;
			int smax ;
			wchar_t *TargetStr = NULL ;
			int TargetStrLength ;
			D_RECONVERTSTRING *ReConvString = ( D_RECONVERTSTRING * )lParam ;

			// 再変換対象の文字数を取得
			if( Input->SelectStart != -1 )
			{
				if( Input->SelectStart > Input->SelectEnd )
				{
					smin = Input->SelectEnd ;
					smax = Input->SelectStart ;
				}
				else
				{
					smin = Input->SelectStart ;
					smax = Input->SelectEnd ;
				}

				TargetStrLength = smax - smin ;
			}
			else
			{
				TargetStrLength = ( int )_WCSLEN( Input->Buffer ) ;
			}

			// 再変換情報の設定が要求されていない場合は必要データサイズを返す
			if( ReConvString == NULL )
			{
				return ( LRESULT )( sizeof( D_RECONVERTSTRING ) + ( TargetStrLength + 1 ) * sizeof( wchar_t ) ) ;
			}

			// 再変換情報の設定を行う
			TargetStr = ( wchar_t * )( ( BYTE * )ReConvString + sizeof( D_RECONVERTSTRING ) ) ;
			if( Input != NULL && Input->SelectStart != -1 )
			{
				_MEMCPY( TargetStr, &Input->Buffer[ smin ], TargetStrLength * sizeof( wchar_t ) ) ;
				TargetStr[ TargetStrLength ] = _T( '\0' ) ;
			}
			else
			{
				_WCSCPY( TargetStr, Input->Buffer ) ;
			}

			ReConvString->dwStrLen          = ( DWORD )TargetStrLength ;
			ReConvString->dwStrOffset       = sizeof( D_RECONVERTSTRING ) ;
			ReConvString->dwTargetStrLen    = 0 ;
			ReConvString->dwTargetStrOffset = 0 ;

			Imc = WinAPIData.ImmFunc.ImmGetContextFunc( hWnd ) ;
			if( Imc != NULL )
			{
				// 再変換対象文字列の位置と文字数をセット
				if( Input->SelectStart != -1 )
				{
					// 再変換の対象となる部分は、渡した文字列の先頭からすべて
					ReConvString->dwCompStrOffset = 0 ;
					ReConvString->dwCompStrLen    = ( DWORD )TargetStrLength ;
				}
				else
				{
					// 範囲選択が無いときは入力文字列全体を渡し、カーソル位置を指定する
					ReConvString->dwCompStrOffset = Input->Point * sizeof( wchar_t ) ;
					ReConvString->dwCompStrLen    = ( DWORD )_WCSLEN( Input->Buffer ) ;
				}

				// 再変換の準備
				WinAPIData.ImmFunc.ImmSetCompositionStringFunc( Imc, SCS_QUERYRECONVERTSTRING, ReConvString, ReConvString->dwSize, NULL, 0 ) ;

				// 再変換の実行
				if( WinAPIData.ImmFunc.ImmSetCompositionStringFunc( Imc, SCS_SETRECONVERTSTRING, ReConvString, ReConvString->dwSize, NULL, 0 ) != 0 )
				{
					// 再変換対象となった箇所を削除する
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = ( int )( ReConvString->dwCompStrOffset / sizeof( wchar_t ) ) ;
						Input->SelectEnd   = ( int )( Input->SelectStart + ReConvString->dwCompStrLen ) ;
						Input->Point       = Input->SelectStart ;
					}
					KeyInputSelectAreaDelete( Input ) ;
				}

				WinAPIData.ImmFunc.ImmReleaseContextFunc( hWnd, Imc );
			}
		}
		else
		{
			WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;
		}
		break ;

	// ＩＭＥウインドウアクティブ状態変化
	case WM_IME_SETCONTEXT :
//		lParam &= ~ISC_SHOWUICANDIDATEWINDOW ;
		lParam = 0;
		return WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;

	// ＩＭＥ入力開始メッセージ
	case WM_IME_STARTCOMPOSITION :

		CharBuf.InputPoint = 0 ;
		CharBuf.IMEInputFlag = TRUE ;

//		// ＩＭＥの使用状態を得る
//		if( CharBuf.IMESwitch == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA )
//		{
//			CharBuf.IMESwitch = TRUE ;
//		}
		return 0;

	// ＩＭＥ入力終了メッセージ
	case WM_IME_ENDCOMPOSITION :
		_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE );
		CharBuf.IMEInputFlag = FALSE ;

//		// ＩＭＥの使用状態を得る
//		if( CharBuf.IMESwitch == TRUE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA )
//		{
//			CharBuf.IMESwitch = FALSE ;
//		}
		break ;


	// ＩＭＥ文字編集変化メッセージ
	case WM_IME_COMPOSITION :
		{
//			HWND DefHwnd = ImmGetDefaultIMEWnd( WinData.MainWindow ) ;
			HWND DefHwnd = hWnd;

			// 入力コンテキストを取得
//			SendMessage( DefHwnd , WM_CLOSE , 0 , 0 ) ;
			Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;
//			Imc = WinAPIData.ImmFunc.ImmGetContextFunc( hWnd ) ;

			// 編集文字変化時の処理
			if( lParam & GCS_COMPSTR )
			{
				// 編集中文字列の取得
				_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE );
				WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPSTR , CharBuf.InputString , CHARBUFFER_SIZE );
			}

			// 編集中文字決定時の処理
			if( lParam & GCS_RESULTSTR )
			{
				_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE );
				WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_RESULTSTR , CharBuf.InputString , CHARBUFFER_SIZE );
				// バッファにストック
				{
					int StrLen , i ;

					StrLen = ( int )_WCSLEN( CharBuf.InputString ) ;
					for( i = 0 ; i < StrLen ; i ++ )
					{
						StockInputChar_WCHAR_T( CharBuf.InputString[ i ] ) ;
					}
				}

				// 編集中文字列初期化
				_MEMSET( CharBuf.InputString, 0, sizeof( CharBuf.InputString ) );
				WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPSTR , CharBuf.InputString , CHARBUFFER_SIZE );

				// 変換候補の情報をリセット
				if( CharBuf.CandidateList )
				{
					DXFREE( CharBuf.CandidateList ) ;
					CharBuf.CandidateList = NULL ;
					CharBuf.CandidateListSize = 0 ;
				}
			}

			// カーソル位置変化時の処理
			if( lParam & GCS_CURSORPOS )
			{
				int OlgPoint = CharBuf.InputPoint ;
				CharBuf.InputPoint = GetStringPoint2_WCHAR_T( CharBuf.InputString , WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_CURSORPOS , NULL , 0 ) ) ;

				// 変換候補の情報をリセット
				if( CharBuf.CandidateList && OlgPoint != CharBuf.InputPoint )
				{
					DXFREE( CharBuf.CandidateList ) ;
					CharBuf.CandidateList = NULL ;
					CharBuf.CandidateListSize = 0 ;
				}
			}

			// 文節情報変化時
			if( lParam & GCS_COMPCLAUSE )
			{
				// 文節情報の取得
				_MEMSET( CharBuf.ClauseData, 0, sizeof( CharBuf.ClauseData ) ) ;
				CharBuf.ClauseNum = WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPCLAUSE , ( void * )CharBuf.ClauseData , 1024 * sizeof( int ) ) / 4 ;
			}

			// 入力文字属性変化時
			if( lParam & GCS_COMPATTR )
			{
				_MEMSET( CharBuf.CharAttr, 0, sizeof( CharBuf.CharAttr ) ) ;
				CharBuf.CharAttrNum = WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPATTR , ( void * )CharBuf.CharAttr , 1024 * sizeof( BYTE ) ) ;
			}

			// 入力コンテキストの削除
			WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
//			WinAPIData.ImmFunc.ImmReleaseContextFunc( hWnd , Imc ) ;
		}
		break;

	// ＩＭＥ状態変化メッセージ
	case WM_IME_NOTIFY :

		switch( wParam )
		{
		// ＩＭＥのＯＮ、ＯＦＦ変化
		case IMN_SETOPENSTATUS :

			if( Flag ) break ;

			Flag = TRUE ;

			WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;

			// キーが押されている間ここで止まる
			{
				__int64 Time = NS_GetNowHiPerformanceCount() ;

				while( NS_ProcessMessage() == 0 && NS_GetNowHiPerformanceCount() - Time < 2000 ){}
			}

			Flag = FALSE ;

			// ＩＭＥの使用状態を得る
			{
				HWND DefHwnd = hWnd;
//				HWND DefHwnd = ImmGetDefaultIMEWnd( hWnd ) ;
				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;

//				SendMessage( DefHwnd , WM_CLOSE , 0 , 0 ) ;
//				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( hWnd ) ;

				CharBuf.IMESwitch = WinAPIData.ImmFunc.ImmGetOpenStatusFunc( Imc ) ;
				WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
//				WinAPIData.ImmFunc.ImmReleaseContextFunc( hWnd , Imc );
			}

			break ;


		// 候補変化時の処理
		case IMN_CHANGECANDIDATE:
		case IMN_OPENCANDIDATE :
		case IMN_SETCANDIDATEPOS :
			if( CharBuf.ITfThreadMgrEx == NULL )
			{
				DWORD BufSize ;
//				HWND DefHwnd = ImmGetDefaultIMEWnd( hWnd ) ;
				HWND DefHwnd = hWnd;
//				int Result ;

				// 入力コンテキストを取得
				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;
//				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( hWnd ) ;

				// バッファサイズ取得
//				BufSize = ImmGetCandidateListCount( Imc , &ListSize ) ;
				BufSize = WinAPIData.ImmFunc.ImmGetCandidateListFunc( Imc , 0 , NULL , 0 ) ;
				if( BufSize != 0 )
				{
					CharBuf.CandidateListSize = ( int )BufSize ;
					if( BufSize == 0 )
					{
						WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x22\x6f\x57\x5b\x09\x59\xdb\x63\x19\x50\xdc\x88\x6e\x30\xc7\x30\xfc\x30\xbf\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x10\xff\x67\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x44\x00\x65\x00\x66\x00\x48\x00\x57\x00\x4e\x00\x44\x00\x3a\x00\x25\x00\x78\x00\x20\x00\x49\x00\x4d\x00\x43\x00\x3a\x00\x25\x00\x78\x00\x0a\x00\x00"/*@ L"漢字変換候補のデータサイズが０でした  DefHWND:%x IMC:%x\n" @*/, DefHwnd, Imc )) ;
						return 0 ;
					}

					// バッファ用メモリの確保
					if( CharBuf.CandidateList != NULL )
					{
						DXFREE( CharBuf.CandidateList ) ;
					}
					if( ( CharBuf.CandidateList = ( CANDIDATELIST * )DXALLOC( BufSize ) ) == NULL )
					{
						WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
#ifndef DX_NON_LITERAL_STRING
						return DxLib_FmtErrorUTF16LE( "\x22\x6f\x57\x5b\x09\x59\xdb\x63\x19\x50\xdc\x88\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\xba\x78\xdd\x4f\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x5f\x30\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\xd0\x30\xa4\x30\xc8\x30\x20\x00\x29\x00\x00"/*@ L"漢字変換候補を保存するメモリの確保に失敗しました( 確保しようとしたサイズ %d バイト )" @*/, BufSize ) ;
#else
						return DxLib_FmtError( "" ) ;
#endif
					}

					// データの取得
					if( WinAPIData.ImmFunc.ImmGetCandidateListFunc( Imc , 0 , CharBuf.CandidateList , BufSize ) == 0 )
					{
						DXFREE( CharBuf.CandidateList ) ;
						CharBuf.CandidateList = NULL ;
						WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );

						DXST_LOGFILEFMT_ADDUTF16LE(( "\x22\x6f\x57\x5b\x09\x59\xdb\x63\x19\x50\xdc\x88\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x42\x00\x75\x00\x66\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x64\x00\x0a\x00\x00"/*@ L"漢字変換候補の取得に失敗しました  BufSize:%d\n" @*/, BufSize )) ;
						return 0 ;
					}
				}

				// 入力コンテキストの削除
				WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
//				WinAPIData.ImmFunc.ImmReleaseContextFunc( hWnd , Imc );
			}
			break ;

		// 入力候補ウインドウを閉じようとしている時の処理
		case IMN_CLOSECANDIDATE :

			// メモリの解放
			if( CharBuf.CandidateList )
			{
				DXFREE( CharBuf.CandidateList ) ;
			}
			CharBuf.CandidateList = NULL ;
			CharBuf.CandidateListSize = 0 ;
			break ;

		default :
//			return 0;
			return WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;
		}
		break;
	}

	return 0 ;
}



// 画面上に入力中の文字列を描画する
extern int NS_DrawIMEInputString( int x , int y , int SelectStringNum , int DrawCandidateList )
{
	int StrLen , FontSize , FontHandle ;
	int PointX , PointY ;
	int CPointX , CPointY ;
	int Width ;
	RECT DrawRect ;
	int Use3DFlag ;
	int IsSelect ;
	int Mult ;
	int Point ;
	static wchar_t StringBuf[2048] ;

	if( CharBuf.IMEUseFlag_OSSet == FALSE || CharBuf.IMESwitch == FALSE ) return -1 ;

	// 使用するフォントのハンドルをセットする
	FontHandle = CharBuf.UseFontHandle == -1 ? NS_GetDefaultFontHandle() : CharBuf.UseFontHandle ;

	// フォントサイズを得る
	FontSize = NS_GetFontSizeToHandle( FontHandle ) ;

	// ３Ｄ有効フラグを得る
	Use3DFlag = NS_GetUse3DFlag() ;
	NS_SetUse3DFlag( FALSE ) ;

	// 描画可能領域を得る
	NS_GetDrawArea( &DrawRect ) ;

	// 入力文字列がない場合はここで終了
	if( CharBuf.InputString[ 0 ] == 0 )
	{
		// ３Ｄ有効フラグを元に戻す
		NS_SetUse3DFlag( Use3DFlag ) ;
		return 0 ;
	}

	// 描画文字列の長さを得る
	StrLen   = ( int )_WCSLEN( CharBuf.InputString ) ;

	// 描画位置を補正
	if( x < DrawRect.left ) x = DrawRect.left ;
	if( y < DrawRect.top  ) y = DrawRect.top ;

	// 入力文字列を描画
	{
		int i ;

		// 入力中文字列を覆う矩形を描画
		for( i = 0 ; i < StrLen ; i ++ )
		{
			Point = GetDrawStringWidthToHandle_WCHAR_T( CharBuf.InputString    , 0, i, FontHandle, FALSE ) ;
			Width = GetDrawStringWidthToHandle_WCHAR_T( CharBuf.InputString + i, 0, 1, FontHandle, FALSE ) ;

			if( i < CharBuf.CharAttrNum && CharBuf.CharAttr[ i ] == ATTR_TARGET_CONVERTED )
			{
				IsSelect = TRUE ;
			}
			else
			{
				IsSelect = FALSE ;
			}

			NS_DrawObtainsBox(
				x + Point ,
				y ,
				x + Point + Width ,
				y + FontSize ,
				FontSize + FontSize / 10 * 3 ,
				IsSelect ? CharBuf.IMESelectStrBackColor : CharBuf.IMEStrBackColor ,
				TRUE
			) ;

			if( CHECK_WCHAR_T_DOUBLE( CharBuf.InputString[ i ] ) )
			{
				i ++ ;
			}
		}

		// 入力中文字列を描画
		for( i = 0 ; i < StrLen ; i ++ )
		{
			Point = GetDrawStringWidthToHandle_WCHAR_T( CharBuf.InputString    , 0, i , FontHandle, FALSE ) ;
			Width = GetDrawStringWidthToHandle_WCHAR_T( CharBuf.InputString + i, 0, 1 , FontHandle, FALSE ) ;

			if( i < CharBuf.CharAttrNum && CharBuf.CharAttr[ i ] == ATTR_TARGET_CONVERTED )
			{
				IsSelect = TRUE ;
			}
			else
			{
				IsSelect = FALSE ;
			}

			Mult = CHECK_WCHAR_T_DOUBLE( CharBuf.InputString[ i ] ) ;

			StringBuf[ 0 ] = CharBuf.InputString[ i ] ;
			if( Mult )
			{
				StringBuf[ 1 ] = CharBuf.InputString[ i + 1 ] ;
				StringBuf[ 2 ] = L'\0' ;
			}
			else
			{
				StringBuf[ 1 ] = L'\0' ;
			}

			DrawObtainsString_WCHAR_T(
				x + Point ,
				y ,
				FontSize + FontSize / 10 * 3 ,
				StringBuf ,
				-1,
				IsSelect ? CharBuf.IMESelectStrColor : CharBuf.IMEStrColor ,
				IsSelect ? ( CharBuf.IMESelectStrEdgeColorEnable ? CharBuf.IMESelectStrEdgeColor : CharBuf.IMESelectStrBackColor ) :
				           ( CharBuf.IMEStrEdgeColorEnable       ? CharBuf.IMEStrEdgeColor       : CharBuf.IMEStrBackColor       ),
				FontHandle
			) ;

			if( Mult )
			{
				i ++ ;
			}
		}
	}

	// カーソルのある位置を算出
	{
		StrLen = GetStringPoint_WCHAR_T( CharBuf.InputString , CharBuf.InputPoint ) ;

		// 先頭文字位置までの幅を算出
		if( StrLen == 0 )
		{
			CPointX = 0 ;
		}
		else
		{
			// 文字幅を得る
			CPointX = GetDrawStringWidthToHandle_WCHAR_T( CharBuf.InputString, 0, StrLen , FontHandle, FALSE ) ;
		}
		CPointX += x ;
		CPointY = y ;
	}

	// カーソルを描画
	{
		// 選択文字列が存在する場合のみ描画
		if( CharBuf.CandidateList != NULL && CharBuf.CBrinkDrawFlag == TRUE )
		{
			// カーソルの描画
			NS_DrawObtainsBox( CPointX , CPointY , CPointX + 2 , CPointY + FontSize , FontSize + FontSize / 10 * 3  , CharBuf.IMECursorColor , TRUE ) ;
		}
	}

	// 下線を描画
	{
		int i, StrNum , StrWidth , LinePX, LinePY ;

		StrNum = 0 ;
		PointX = x ;
		PointY = y ;
		LinePX = FontSize / 20 ;
		if( LinePX < 1 )
		{
			LinePX = 1 ;
		}
		LinePY = FontSize / 10 ;
		for( i = 0 ; i < CharBuf.ClauseNum ; i ++ )
		{
			StrWidth = GetDrawStringWidthToHandle_WCHAR_T( &CharBuf.InputString[ StrNum ], 0, CharBuf.ClauseData[ i ] - StrNum, FontHandle, FALSE ) ;
			if( StrWidth > LinePX * 4 )
			{
				if( StrNum < CharBuf.CharAttrNum && CharBuf.CharAttr[ StrNum ] == ATTR_TARGET_CONVERTED )
				{
					IsSelect = TRUE ;
				}
				else
				{
					IsSelect = FALSE ;
				}

				NS_DrawObtainsBox(
					PointX + LinePX * 2				, PointY + FontSize + LinePY , 
					PointX + StrWidth - LinePX * 2	, PointY + FontSize + LinePY * ( IsSelect ? 3 : 2 ) ,
					FontSize + FontSize / 10 * 3 ,
					CharBuf.IMELineColor , TRUE
				) ;
			}
			PointX += StrWidth ;
			StrNum = CharBuf.ClauseData[ i ] ;
		}
	}

	// 候補リストが出ている場合はその描画
	if( DrawCandidateList && CharBuf.CandidateList && CharBuf.CandidateList->dwCount > 0 )
	{
		DWORD i ;
		RECT SelectRect ;
		int j ,h ;
		int MaxWidth ;
		int ValidNum ;
		int SelectionNumWidth ;

		// 選択候補の最大数を表示すると描画範囲を超えてしまう場合は描画する候補の数を制限する
		h = FontSize / 3 ;
		if( FontSize * ( SelectStringNum + 1 ) + h * 2 > DrawRect.bottom - DrawRect.top )
		{
			SelectStringNum = ( DrawRect.bottom - DrawRect.top - ( h * 2 ) ) / FontSize - 1 ;
			if( SelectStringNum <= 0 )
			{
				SelectStringNum = 1 ;
			}
		}

		// 最長選択候補を調べる
		MaxWidth = 0 ;
		ValidNum = 0 ;
		j = 0 ;
		for( i = ( CharBuf.CandidateList->dwSelection / SelectStringNum ) * SelectStringNum ; j < SelectStringNum ; i ++ , j ++ )
		{
			if( i >= CharBuf.CandidateList->dwCount ) break ;

			if( ( ( wchar_t * )( ( BYTE * )CharBuf.CandidateList + CharBuf.CandidateList->dwOffset[ i ] ) )[ 0 ] != L'\0' )
			{
				ValidNum ++ ;
			}

			_SWNPRINTF( StringBuf, sizeof( StringBuf ) / 2, L"%d:%s", j + 1, ( wchar_t * )( ( BYTE * )CharBuf.CandidateList + CharBuf.CandidateList->dwOffset[ i ] ) ) ;
			Width = GetDrawStringWidthToHandle_WCHAR_T( StringBuf, 0, ( int )_WCSLEN( StringBuf ), FontHandle, FALSE ) ;
			if( Width > MaxWidth )
			{
				MaxWidth = Width ;
			}
		}
		_SWNPRINTF( StringBuf, sizeof( StringBuf ) / 2, L"%d/%d", CharBuf.CandidateList->dwSelection + 1, CharBuf.CandidateList->dwCount ) ;
		SelectionNumWidth = GetDrawStringWidthToHandle_WCHAR_T( StringBuf, 0, ( int )_WCSLEN( StringBuf ), FontHandle, FALSE ) ;
		if( SelectionNumWidth > MaxWidth )
		{
			MaxWidth = SelectionNumWidth ;
		}
		j ++ ;

		// 有効な選択対象がある場合のみ描画
		if( ValidNum > 0 )
		{
			// 描画範囲をセット
			{
				SETRECT(
					SelectRect,
					CPointX,					CPointY + FontSize + FontSize / 2 ,
					CPointX + MaxWidth + h * 2,	CPointY + FontSize + FontSize / 2 + FontSize * j + h * 2
				) ;

				// インプット文字描画位置より下の位置に配置できるか検証
				if( SelectRect.bottom > DrawRect.bottom )
				{
					// 出来ない場合はインプット文字描画位置より上の位置に配置を試みる
					if( y - FontSize * j - FontSize / 2 - h * 2 < DrawRect.top )
					{	
						// それでも駄目な場合は無理やり下の位置に描画する
						j = SelectRect.bottom - DrawRect.bottom ;
					}
					else
					{
						// 上の位置を起点に配置
						j = SelectRect.top - ( y - FontSize * j - FontSize / 2 - h * 2 ) ;
					}

					// 高さを補正
					SelectRect.bottom -= j ;
					SelectRect.top -= j ;
				}

				// インプット文字描画幅のせいで描画可能領域右端に到達していた場合は補正
				if( SelectRect.right > DrawRect.right )
				{
					j = SelectRect.right - DrawRect.right ;	

					SelectRect.left -= j ;
					SelectRect.right -= j ;
				}
			}

			// 描画範囲を黒で塗りつぶし
			NS_DrawBox( SelectRect.left , SelectRect.top , SelectRect.right , SelectRect.bottom , CharBuf.IMEConvWinBackColor , TRUE ) ;
			NS_DrawBox( SelectRect.left , SelectRect.top , SelectRect.right , SelectRect.bottom , CharBuf.IMEConvWinEdgeColor , FALSE ) ;

			// 候補を描画
			PointX = SelectRect.left + h ;
			PointY = SelectRect.top + h ;
			j = 0 ;
			for( i = ( CharBuf.CandidateList->dwSelection / SelectStringNum ) * SelectStringNum ; j < SelectStringNum ; i ++ , j ++ )
			{
				if( i >= CharBuf.CandidateList->dwCount ) break ;

				IsSelect = ( i == CharBuf.CandidateList->dwSelection ) ? TRUE : FALSE ;
				if( IsSelect && CharBuf.IMEConvWinSelectStrBackColorEnable )
				{
					NS_DrawBox( SelectRect.left + 1, PointY + j * FontSize , SelectRect.right - 1, PointY + ( j + 1 ) * FontSize, CharBuf.IMEConvWinSelectStrBackColor, TRUE ) ;
				}

				_SWNPRINTF( StringBuf, sizeof( StringBuf ) / 2, L"%d:%s", j + 1, ( wchar_t * )( ( BYTE * )CharBuf.CandidateList + CharBuf.CandidateList->dwOffset[i] ) ) ;
				DrawStringToHandle_WCHAR_T( PointX , PointY + j * FontSize , StringBuf, _WCSLEN( StringBuf ),
					IsSelect ? CharBuf.IMEConvWinSelectStrColor     : CharBuf.IMEConvWinStrColor , FontHandle ,
					IsSelect ? CharBuf.IMEConvWinSelectStrEdgeColor : CharBuf.IMEConvWinStrEdgeColor, FALSE ) ;
			}
 
			// 候補総数の内何番目か、の情報を描画する
			_SWNPRINTF( StringBuf, sizeof( StringBuf ) / 2, L"%d/%d", CharBuf.CandidateList->dwSelection + 1, CharBuf.CandidateList->dwCount ) ;
			DrawStringToHandle_WCHAR_T( PointX + MaxWidth - SelectionNumWidth, PointY + j * FontSize , StringBuf, _WCSLEN( StringBuf ), CharBuf.IMEConvWinStrColor , FontHandle , CharBuf.IMEConvWinStrEdgeColor, FALSE ) ;

			// 候補リストの開始インデックスと候補リストのページサイズを変更
			{
				HWND DefHwnd = WinData.MainWindow;
				HIMC Imc ;

				SETUP_WIN_API

				// 入力コンテキストを取得
				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;

				// 候補リストのページサイズを変更
				WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc, NI_SETCANDIDATE_PAGESIZE, 0, ( DWORD )SelectStringNum ) ;

				// 候補リストの開始インデックスの変更メッセージを送る
				WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc, NI_SETCANDIDATE_PAGESTART, 0, ( CharBuf.CandidateList->dwSelection / SelectStringNum ) * SelectStringNum ) ;

				// 入力コンテキストの削除
				WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
			}
		}
	}

	// ３Ｄ有効フラグを元に戻す
	NS_SetUse3DFlag( Use3DFlag ) ;

	// 終了
	return 0 ;
}

// ＩＭＥを使用するかどうかの状態を更新する
extern void RefreshIMEFlag( int Always )
{
	int NewUseFlag ;

	SETUP_WIN_API

	NewUseFlag = CharBuf.IMEUseFlag || CharBuf.IMEUseFlag_System ;

	// フラグが以前と同じ場合は何もせず終了
	if( NewUseFlag == CharBuf.IMEUseFlag_OSSet && Always == FALSE ) return ;

	// 有効フラグをセット
	if( WinAPIData.Win32Func.WINNLSEnableIME_Func )
		WinAPIData.Win32Func.WINNLSEnableIME_Func( WinData.MainWindow , NewUseFlag ) ;

	// フラグ保存
	CharBuf.IMEUseFlag_OSSet = NewUseFlag ;

	// もしFALSEだった場合は各種メモリを解放する
	if( NewUseFlag == FALSE ) 
	{
		DXFREE( CharBuf.CandidateList ) ;
		CharBuf.CandidateList = NULL ;
		CharBuf.CandidateListSize = 0 ;

		_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE ) ;

		// 入力中か、フラグも倒す
		CharBuf.IMEInputFlag = FALSE ;
	}

	// 終了
	return ;
}

// ＩＭＥを使用するかどうかをセットする
extern int NS_SetUseIMEFlag( int UseFlag )
{
	// フラグ保存
	CharBuf.IMEUseFlag = UseFlag ;

	// 状態を更新
	RefreshIMEFlag() ;

	// 終了
	return 0 ;
}

// ＩＭＥを使用するかどうかを取得する
extern int NS_GetUseIMEFlag( void )
{
	// フラグを返す
	return CharBuf.IMEUseFlag ;
}

// ＩＭＥで入力できる最大文字数を MakeKeyInput の設定に合わせるかどうかをセットする( TRUE:あわせる  FALSE:あわせない(デフォルト) )
extern int NS_SetInputStringMaxLengthIMESync( int Flag )
{
	CharBuf.IMEInputStringMaxLengthIMESync = Flag ;

	// 終了
	return 0 ;
}

// ＩＭＥで一度に入力できる最大文字数を設定する( 0:制限なし  1以上:指定の文字数で制限 )
extern int NS_SetIMEInputStringMaxLength( int Length )
{
	CharBuf.IMEInputMaxLength = Length ;

	// 終了
	return 0 ;
}

// ＩＭＥの漢字変換候補表示の処理に TSF を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern int NS_SetUseTSFFlag( int UseFlag )
{
	CharBuf.TSFNotUseFlag = UseFlag != FALSE ? FALSE : TRUE ;

	// 終了
	return 0 ;
}

// ＩＭＥを使用状態を変更する
static int SetIMEOpenState( int OpenFlag ) 
{
	HIMC Imc ;
	HWND DefHwnd = WinData.MainWindow;

	SETUP_WIN_API

	// 入力コンテキストを取得
	Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;

	// 使用状態を変更する
	WinAPIData.ImmFunc.ImmSetOpenStatusFunc( Imc, OpenFlag == TRUE ? true : false ) ;

	// 入力コンテキストの削除
	WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );

	// 終了
	return 0 ;
}

// キー入力の選択範囲を削除する
static int KeyInputSelectAreaDelete( INPUTDATA * Input )
{
	int smin, smax, DelNum ;

	if( Input->SelectStart == -1 ) return 0 ;

	if( Input->SelectStart > Input->SelectEnd )
	{
		smin = Input->SelectEnd ;
		smax = Input->SelectStart ;
	}
	else
	{
		smin = Input->SelectStart ;
		smax = Input->SelectEnd ;
	}
	DelNum = smax - smin ;
	Input->Point = smin ;

	if( smax != Input->StrLength )
	{
		_MEMMOVE( &Input->Buffer[ smin ], &Input->Buffer[ smax ], ( Input->StrLength - smax ) * sizeof( wchar_t ) ) ;
	}
	Input->StrLength -= DelNum ;
	Input->Buffer[ Input->StrLength ] = _T( '\0' ) ;
	Input->SelectStart = -1 ;
	Input->SelectEnd = -1 ;
	if( Input->StrLength < Input->DrawStartPos ) Input->DrawStartPos = Input->StrLength ;

	// 終了
	return 0 ;
}


#endif

// 全角文字、半角文字入り乱れる中から指定の全半混在文字数での半角文字数を得る
extern int NS_GetStringPoint( const TCHAR *String , int Point )
{
#ifdef UNICODE
	return GetStringPoint_WCHAR_T( String, Point ) ;
#else
	int i , p ;
	DWORD CharCode ;
	int CharBytes ;

	p = 0 ;
	for( i = 0 ; i < Point ; i ++ )
	{
		CharCode = GetCharCode( String + p, _TCHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == 0 )
		{
			break ;
		}

		p += CharBytes / sizeof( char ) ;
	}

	return p ;
#endif
}

// 全角文字、半角文字入り乱れる中から指定の文字数での半角文字数を得る
extern int NS_GetStringPointWithStrLen( const TCHAR *String, size_t StringLength, int Point )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = NS_GetStringPoint( UseStringBuffer, Point ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( String )
	return Result ;
}

// 全角文字、半角文字入り乱れる中から指定の全半混在文字数での半角文字数を得る
extern int GetStringPoint_WCHAR_T( const wchar_t *String , int Point )
{
	int i , p ;

	p = 0 ;
	for( i = 0 ; i < Point ; i ++ )
	{
		if( *( String + p ) == L'\0' )
		{
			break ; 
		}

		if( CHECK_WCHAR_T_DOUBLE( String[ p ] ) == FALSE )
		{
			p ++ ;
		}
		else
		{
			p += 2 ;
		}
	}

	return p ;
}

// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern int NS_GetStringPoint2( const TCHAR *String , int Point )
{
#ifdef UNICODE
	return GetStringPoint2_WCHAR_T( String, Point ) ;
#else
	int i , p ;
	DWORD CharCode ;
	int CharBytes ;

	p = 0 ;
	i = 0 ;
	while( i < Point )
	{
		CharCode = GetCharCode( String + i, _TCHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == 0 )
		{
			break ;
		}

		i += CharBytes / sizeof( char ) ;
		p ++ ;
	}

	return p ;
#endif
}

// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern int NS_GetStringPoint2WithStrLen( const TCHAR *String, size_t StringLength, int Point )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = NS_GetStringPoint2( UseStringBuffer, Point ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = NS_GetStringPoint2( UseStringBuffer, Point ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( String )
#endif
	return Result ;
}

// 全角文字、半角文字入り乱れる中から指定の半角文字数での全角文字数を得る
extern int GetStringPoint2_WCHAR_T( const wchar_t *String , int Point )
{
	int i , p ;

	p = 0 ;
	i = 0 ;
	while( i < Point )
	{
		if( *( String + i ) == L'\0' )
		{
			break ; 
		}

		if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) == FALSE )
		{
			i ++ ;
		}
		else
		{
			i += 2 ;
		}

		p ++ ;
	}

	return p ;
}

// 全角文字、半角文字入り乱れる中から文字数を取得する
extern int NS_GetStringLength( const TCHAR *String )
{
#ifdef UNICODE
	return GetStringLength_WCHAR_T( String ) ;
#else
	int i , p ;
	DWORD CharCode ;
	int CharBytes ;

	p = 0 ;
	i = 0 ;
	for(;;)
	{
		CharCode = GetCharCode( String + i, _TCHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == 0 )
		{
			break ;
		}

		i += CharBytes / sizeof( char ) ;
		p ++ ;
	}

	return p ;
#endif
}

// 全角文字、半角文字入り乱れる中から文字数を取得する
extern int GetStringLength_WCHAR_T( const wchar_t *String )
{
	int i , p ;

	p = 0 ;
	i = 0 ;
	while( *( String + i ) != L'\0' )
	{
		if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) == FALSE )
		{
			i ++ ;
		}
		else
		{
			i += 2 ;
		}

		p ++ ;
	}

	return p ;
}

#ifndef DX_NON_FONT

// 規定領域に収めたかたちで文字列を描画
extern int NS_DrawObtainsString(
	int x , int y ,
	int AddY ,
	const TCHAR *String ,
	unsigned int StrColor ,
	unsigned int StrEdgeColor ,
	int FontHandle,
	unsigned int SelectBackColor ,
	unsigned int SelectStrColor ,
	unsigned int SelectStrEdgeColor ,
	int SelectStart ,
	int SelectEnd )
{
#ifdef UNICODE
	return DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		String ,
		-1,
		StrColor ,
		StrEdgeColor ,
		FontHandle,
		SelectBackColor ,
		SelectStrColor ,
		SelectStrEdgeColor ,
		SelectStart ,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		UseStringBuffer ,
		-1,
		StrColor ,
		StrEdgeColor ,
		FontHandle,
		SelectBackColor ,
		SelectStrColor ,
		SelectStrEdgeColor ,
		SelectStart ,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// 描画可能領域に収まるように改行しながら文字列を描画
extern int NS_DrawObtainsNString( int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor, int FontHandle, unsigned int SelectBackColor, unsigned int SelectStrColor, unsigned int SelectStrEdgeColor, int SelectStart, int SelectEnd )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		UseStringBuffer ,
		-1,
		StrColor ,
		StrEdgeColor ,
		FontHandle,
		SelectBackColor ,
		SelectStrColor ,
		SelectStrEdgeColor ,
		SelectStart ,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		UseStringBuffer ,
		-1,
		StrColor ,
		StrEdgeColor ,
		FontHandle,
		SelectBackColor ,
		SelectStrColor ,
		SelectStrEdgeColor ,
		SelectStart ,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#endif
	return Result ;
}

// 規定領域に収めたかたちで文字列を描画
extern int DrawObtainsString_WCHAR_T(
	int x , int y ,
	int AddY ,
	const wchar_t *String ,
	int StrLen,
	unsigned int StrColor ,
	unsigned int StrEdgeColor ,
	int FontHandle,
	unsigned int SelectBackColor ,
	unsigned int SelectStrColor ,
	unsigned int SelectStrEdgeColor ,
	int SelectStart ,
	int SelectEnd,
	int DrawFlag,
	int *PosX,
	int *PosY
)
{
#ifndef DX_NON_GRAPHICS
	int PointX , PointY ;
//	int Width ;
	int StrWidth ;
	int Use3DFlag = FALSE ;
	RECT DrawRect ;
//	RECT Rect ;

	// FontHandle が -1 の場合はデフォルトのフォントを使用する
	if( FontHandle == -1 ) FontHandle = NS_GetDefaultFontHandle() ;

	if( DrawFlag )
	{
		// ３Ｄ有効フラグを得る
		Use3DFlag = NS_GetUse3DFlag() ;
		NS_SetUse3DFlag( FALSE ) ;
	}

	// 描画可能領域を得る
	NS_GetDrawArea( &DrawRect ) ;

	// 初期値をセット
	if( PosX )
	{
		*PosX = 0 ;
	}
	if( PosY )
	{
		*PosY = 0 ;
	}

	// 描画位置を補正
	if( x < DrawRect.left ) x = DrawRect.left ;
	if( y < DrawRect.top  ) y = DrawRect.top ;

	while( x > DrawRect.right )
	{
		x -= DrawRect.right - DrawRect.left ;
		y += AddY ;
	}

	// 入力文字列を描画
//	if( SelectStart == -1 )
//	{
//		// 描画文字列の長さを得る
//		StrWidth = GetDrawStringWidthToHandle_WCHAR_T( String , ( int )_WCSLEN( String ) , FontHandle ) ;
//
//		Width = StrWidth ;
//		PointX = x ;
//		PointY = y ;
//		for(;;)
//		{
//			DrawStringToHandle_WCHAR_T( PointX , PointY , String , StrColor , FontHandle , StrEdgeColor ) ;
//
//			SETRECT( Rect , PointX , PointY , PointX + StrWidth , PointY + AddY ) ;
//			RectClipping_Inline( &Rect , &DrawRect ) ;
//
//			Width -= Rect.right - Rect.left ;
//
//			if( Width > 0 )
//			{
//				PointX = DrawRect.left - ( StrWidth - Width ) ;
//				PointY += AddY ;
//			}
//			else break ;
//		}
//	}
//	else
	{
		int i, Num, smin = 0, smax = 0 ;
		wchar_t TempBuf[ 3 ] ;

		if( SelectStart != -1 )
		{
			if( SelectStart < SelectEnd )
			{
				smax = SelectEnd ;
				smin = SelectStart ;
			}
			else
			{
				smax = SelectStart ;
				smin = SelectEnd ;
			}
		}

		if( StrLen < 0 )
		{
			StrLen = ( int )_WCSLEN( String ) ;
		}
		PointX = x ;
		PointY = y ;
		for( i = 0 ; i < StrLen ; )
		{
			if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) )
			{
				TempBuf[ 0 ] = String[ i ] ;
				TempBuf[ 1 ] = String[ i + 1 ] ;
				TempBuf[ 2 ] = 0 ;
				Num = 2 ;
			}
			else
			{
				if( String[ i ] == L'\n' )
				{
					PointX = x ;
					PointY += AddY ;
					i ++ ;
					continue ;
				}
				else
				{
					TempBuf[ 0 ] = String[ i ] ;
					TempBuf[ 1 ] = 0 ;
					Num = 1 ;
				}
			}

			StrWidth = GetDrawStringWidthToHandle_WCHAR_T( TempBuf, 0, ( int )_WCSLEN( TempBuf ), FontHandle, FALSE ) ;
			if( DrawFlag )
			{
				if( SelectStart != -1 && i >= smin && i < smax )
				{
					NS_DrawBox( PointX, PointY, PointX + StrWidth, PointY + NS_GetFontSizeToHandle( FontHandle ), SelectBackColor, TRUE ) ; 
					DrawStringToHandle_WCHAR_T( PointX, PointY, TempBuf, _WCSLEN( TempBuf ), SelectStrColor, FontHandle, SelectStrEdgeColor, FALSE ) ;
				}
				else
				{
					DrawStringToHandle_WCHAR_T( PointX, PointY, TempBuf, _WCSLEN( TempBuf ), StrColor, FontHandle, StrEdgeColor, FALSE ) ;
				}
			}

			if( PointX + StrWidth > DrawRect.right )
			{
				PointX = DrawRect.left - ( DrawRect.right - PointX ) ;
				PointY += AddY ;
			}
			else
			{
				PointX += StrWidth ;
				i += Num ;
			}
		}
	}

	if( DrawFlag )
	{
		// ３Ｄ描画フラグを元に戻す
		NS_SetUse3DFlag( Use3DFlag ) ;
	}

	// 座標をセット
	if( PosX )
	{
		*PosX = PointX ;
	}
	if( PosY )
	{
		*PosY = PointY ;
	}

	// 終了
	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern int NS_DrawObtainsString_CharClip(
	int x, int y,
	int AddY,
	const TCHAR *String,
	unsigned int StrColor,
	unsigned int StrEdgeColor,
	int FontHandle,
	unsigned int SelectBackColor,
	unsigned int SelectStrColor,
	unsigned int SelectStrEdgeColor,
	int SelectStart,
	int SelectEnd
)
{
#ifdef UNICODE
	return DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		String,
		-1,
		StrColor,
		StrEdgeColor,
		FontHandle,
		SelectBackColor,
		SelectStrColor,
		SelectStrEdgeColor,
		SelectStart,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		UseStringBuffer,
		-1,
		StrColor,
		StrEdgeColor,
		FontHandle,
		SelectBackColor,
		SelectStrColor,
		SelectStrEdgeColor,
		SelectStart,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern int NS_DrawObtainsNString_CharClip( int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor, int FontHandle, unsigned int SelectBackColor, unsigned int SelectStrColor, unsigned int SelectStrEdgeColor, int SelectStart, int SelectEnd )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		UseStringBuffer,
		-1,
		StrColor,
		StrEdgeColor,
		FontHandle,
		SelectBackColor,
		SelectStrColor,
		SelectStrEdgeColor,
		SelectStart,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		UseStringBuffer,
		-1,
		StrColor,
		StrEdgeColor,
		FontHandle,
		SelectBackColor,
		SelectStrColor,
		SelectStrEdgeColor,
		SelectStart,
		SelectEnd,
		TRUE,
		NULL,
		NULL
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#endif
	return Result ;
}

// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern int DrawObtainsString_CharClip_WCHAR_T(
	int x, int y,
	int AddY,
	const wchar_t *String,
	int StrLen,
	unsigned int StrColor,
	unsigned int StrEdgeColor,
	int FontHandle,
	unsigned int SelectBackColor,
	unsigned int SelectStrColor,
	unsigned int SelectStrEdgeColor,
	int SelectStart,
	int SelectEnd,
	int DrawFlag,
	int *PosX,
	int *PosY
)
{
#ifndef DX_NON_GRAPHICS
	int PointX , PointY ;
	int StrWidth ;
	int Use3DFlag = FALSE ;
	RECT DrawRect ;
	int i, Num ;
	wchar_t TempBuf[ 3 ] ;

	// FontHandle が -1 の場合はデフォルトのフォントを使用する
	if( FontHandle == -1 )
	{
		FontHandle = NS_GetDefaultFontHandle() ;
	}

	if( DrawFlag )
	{
		// ３Ｄ有効フラグを得る
		Use3DFlag = NS_GetUse3DFlag() ;
		NS_SetUse3DFlag( FALSE ) ;
	}

	// 描画可能領域を得る
	NS_GetDrawArea( &DrawRect ) ;

	// 初期値をセット
	if( PosX )
	{
		*PosX = 0 ;
	}
	if( PosY )
	{
		*PosY = 0 ;
	}

	// 描画範囲に幅が無い場合は何もせずに終了
	if( DrawRect.left == DrawRect.right ||
		DrawRect.top  == DrawRect.bottom )
	{
		return 0 ;
	}

	// 描画位置を補正
	if( x < DrawRect.left ) x = DrawRect.left ;
	if( y < DrawRect.top  ) y = DrawRect.top ;

	while( x > DrawRect.right )
	{
		x -= DrawRect.right - DrawRect.left ;
		y += AddY ;
	}

	if( StrLen < 0 )
	{
		StrLen = ( int )_WCSLEN( String ) ;
	}

	// 入力文字列を描画
//	if( SelectStart == -1 )
//	{
//		PointX = x ;
//		PointY = y ;
//		for( i = 0 ; i < StrLen ; )
//		{
//			if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) )
//			{
//				TempBuf[ 0 ] = String[ i ] ;
//				TempBuf[ 1 ] = String[ i + 1 ] ;
//				TempBuf[ 2 ] = 0 ;
//				Num = 2 ;
//			}
//			else
//			{
//				TempBuf[ 0 ] = String[ i ] ;
//				TempBuf[ 1 ] = 0 ;
//				Num = 1 ;
//			}
//
//			StrWidth = GetDrawStringWidthToHandle_WCHAR_T( TempBuf, Num, FontHandle ) ;
//			if( PointX + StrWidth > DrawRect.right )
//			{
//				PointX = x ;
//				PointY += AddY ;
//			}
//
//			DrawStringToHandle_WCHAR_T( PointX, PointY, TempBuf, StrColor, FontHandle, StrEdgeColor ) ;
//
//			PointX += StrWidth ;
//			i += Num ;
//		}
//	}
//	else
	{
		int smin = 0, smax = 0;

		if( SelectStart != -1 )
		{
			if( SelectStart < SelectEnd )
			{
				smax = SelectEnd ;
				smin = SelectStart ;
			}
			else
			{
				smax = SelectStart ;
				smin = SelectEnd ;
			}
		}

		PointX = x ;
		PointY = y ;
		for( i = 0 ; i < StrLen ; )
		{
			if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) )
			{
				TempBuf[ 0 ] = String[ i ] ;
				TempBuf[ 1 ] = String[ i + 1 ] ;
				TempBuf[ 2 ] = 0 ;
				Num = 2 ;
			}
			else
			{
				if( String[ i ] == L'\n' )
				{
					PointX = x ;
					PointY += AddY ;
					i ++ ;
					continue ;
				}
				else
				{
					TempBuf[ 0 ] = String[ i ] ;
					TempBuf[ 1 ] = 0 ;
					Num = 1 ;
				}
			}

			StrWidth = GetDrawStringWidthToHandle_WCHAR_T( TempBuf, 0, Num, FontHandle, FALSE ) ;
			if( PointX + StrWidth > DrawRect.right )
			{
				PointX = x ;
				PointY += AddY ;
			}

			if( DrawFlag )
			{
				if( SelectStart != -1 && i >= smin && i < smax )
				{
					NS_DrawBox( PointX, PointY, PointX + StrWidth, PointY + NS_GetFontSizeToHandle( FontHandle ), SelectBackColor, TRUE ) ; 
					DrawStringToHandle_WCHAR_T( PointX, PointY, TempBuf, _WCSLEN( TempBuf ), SelectStrColor, FontHandle, SelectStrEdgeColor, FALSE ) ;
				}
				else
				{
					DrawStringToHandle_WCHAR_T( PointX, PointY, TempBuf, _WCSLEN( TempBuf ), StrColor, FontHandle, StrEdgeColor, FALSE ) ;
				}
			}

			PointX += StrWidth ;
			i += Num ;
		}
	}

	if( DrawFlag )
	{
		// ３Ｄ描画フラグを元に戻す
		NS_SetUse3DFlag( Use3DFlag ) ;
	}

	// 座標をセット
	if( PosX )
	{
		*PosX = PointX ;
	}
	if( PosY )
	{
		*PosY = PointY ;
	}

	// 終了
	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}







// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する
extern int NS_GetObtainsStringCharPosition(	int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle )
{
#ifdef UNICODE
	return DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		String ,
		StrLen,
		0 ,
		0 ,
		FontHandle,
		0 ,
		0 ,
		0 ,
		0 ,
		0 ,
		FALSE,
		PosX,
		PosY
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = DrawObtainsString_WCHAR_T(
		x , y ,
		AddY ,
		UseStringBuffer ,
		StrLen,
		0 ,
		0 ,
		FontHandle,
		0 ,
		0 ,
		0 ,
		0 ,
		0 ,
		FALSE ,
		PosX,
		PosY
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する( クリップが文字単位 )
extern int NS_GetObtainsStringCharPosition_CharClip( int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle )
{
#ifdef UNICODE
	return DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		String,
		StrLen,
		0,
		0,
		FontHandle,
		0,
		0,
		0,
		0,
		0,
		FALSE,
		PosX,
		PosY
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = DrawObtainsString_CharClip_WCHAR_T(
		x, y,
		AddY,
		UseStringBuffer,
		StrLen,
		0,
		0,
		FontHandle,
		0,
		0,
		0,
		0,
		0,
		FALSE,
		PosX,
		PosY
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}





#endif // DX_NON_FONT

// 規定領域に収めたかたちで矩形を描画 
extern int NS_DrawObtainsBox( int x1 , int y1 , int x2 , int y2 , int AddY , unsigned int Color , int FillFlag )
{
#ifndef DX_NON_GRAPHICS
	int AllWidth , PointX , PointY ;
	int Width , Height ;
	int Use3DFlag ;
	RECT DrawRect , Rect ;

	// ３Ｄ有効フラグを得る
	Use3DFlag = NS_GetUse3DFlag() ;
	NS_SetUse3DFlag( FALSE ) ;

	// 描画する長さと高さを得る
	AllWidth = x2 - x1 ;
	Height = y2 - y1 ;

	// 描画可能領域を得る
	NS_GetDrawArea( &DrawRect ) ;

	// 描画位置を補正
	if( x1 < DrawRect.left ){ x2 += DrawRect.left - x1 ; x1 = DrawRect.left ; }
	if( y1 < DrawRect.top  ){ y2 += DrawRect.top  - y1 ; y1 = DrawRect.top  ; }

	while( x1 > DrawRect.right )
	{
		x1 -= DrawRect.right - DrawRect.left ;
		y1 += AddY ;
	}

	// 入力文字列を描画
	Width = AllWidth ;
	PointX = x1 ;
	PointY = y1 ;
	for(;;)
	{
		NS_DrawBox( PointX , PointY , PointX + AllWidth , PointY + Height , Color , FillFlag ) ;

		SETRECT( Rect , PointX , PointY , PointX + AllWidth , PointY + AddY ) ;
		RectClipping_Inline( &Rect , &DrawRect ) ;

		Width -= Rect.right - Rect.left ;

		if( Width > 0 )
		{
			PointX = DrawRect.left - ( AllWidth - Width ) ;
			PointY += AddY ;
		}
		else break ;
	}

	// ３Ｄ描画フラグを元に戻す
	NS_SetUse3DFlag( Use3DFlag ) ;

	// 終了
	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}



// アスキーコントロールコードか調べる
extern int NS_GetCtrlCodeCmp( TCHAR Char ) 
{
	return GetCtrlCodeCmp_WCHAR_T( ( wchar_t )Char ) ;
}

extern int GetCtrlCodeCmp_WCHAR_T( wchar_t Char ) 
{
	int i ;

	for( i = 0 ; CtrlCode[ i ][ 0 ] != 0  &&  ( wchar_t )CtrlCode[ i ][ 1 ] != Char ; i ++ ){}
	
	// 結果を返す
	return CtrlCode[ i ][ 0 ] != 0 ;
}

// ＩＭＥに変化があったか、フラグの取得
extern int GetIMEChangeFlag( void )
{
	int Flag = CharBuf.ChangeFlag ;

	CharBuf.ChangeFlag = FALSE ;
	return Flag ;
}



#ifndef DX_NON_KEYEX

// 文字列の入力取得
extern int NS_InputStringToCustom(
	int x , int y ,
	size_t BufLength ,
	TCHAR *StrBuffer ,
	int CancelValidFlag ,
	int SingleCharOnlyFlag ,
	int NumCharOnlyFlag,
	int DoubleCharOnlyFlag,
	int EnableNewLineFlag,
	int DisplayCandidateList
)
{
#ifdef UNICODE
	return InputStringToCustom_WCHAR_T(
		x , y ,
		BufLength ,
		StrBuffer ,
		CancelValidFlag ,
		SingleCharOnlyFlag ,
		NumCharOnlyFlag,
		DoubleCharOnlyFlag,
		EnableNewLineFlag,
		DisplayCandidateList
	) ;
#else
	int Result ;
	wchar_t TempBuffer[ 512 ] ;
	wchar_t *UseTempBuffer ;
	wchar_t *AllocTempBuffer = NULL ;

	if( BufLength + 1 > sizeof( TempBuffer ) / sizeof( wchar_t ) )
	{
		AllocTempBuffer = ( wchar_t * )DXALLOC( ( BufLength + 1 ) * sizeof( wchar_t ) ) ;
		if( AllocTempBuffer == NULL )
		{
			return -1 ;
		}

		UseTempBuffer = AllocTempBuffer ;
	}
	else
	{
		UseTempBuffer = TempBuffer ;
	}

	Result = InputStringToCustom_WCHAR_T(
		x , y ,
		BufLength ,
		UseTempBuffer ,
		CancelValidFlag ,
		SingleCharOnlyFlag ,
		NumCharOnlyFlag,
		DoubleCharOnlyFlag,
		EnableNewLineFlag,
		DisplayCandidateList
	) ;

	// キャンセルされていない場合のみ文字列を変換
	if( Result == 1 )
	{
		ConvString( ( const char * )UseTempBuffer, -1, WCHAR_T_CHARCODEFORMAT, StrBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;
	}

	if( AllocTempBuffer != NULL )
	{
		DXFREE( AllocTempBuffer ) ;
		AllocTempBuffer = NULL ;
	}

	return Result ;
#endif
}

// 文字列の入力取得
extern int InputStringToCustom_WCHAR_T(
	int x , int y ,
	size_t BufLength ,
	wchar_t *StrBuffer ,
	int CancelValidFlag ,
	int SingleCharOnlyFlag ,
	int NumCharOnlyFlag,
	int DoubleCharOnlyFlag,
	int EnableNewLineFlag,
	int DisplayCandidateList
)
{
	int ScreenGraphFront, ScreenGraphBack ;
	int EndFlag = 0 ;
	RECT DrawRect ;
	int FontSize ;
	int DrawWidth , DrawHeight ;
//	wchar_t *Buffer ;
	int Red , Green , Blue ;
	int InputHandle ;
	int ScreenWidth, ScreenHeight ;
	SCREENDRAWSETTINGINFO ScreenDrawSettingInfo ;

	// 各種描画設定情報を取得
	Graphics_DrawSetting_GetScreenDrawSettingInfo( &ScreenDrawSettingInfo ) ;
	GetTransColor( &Red , &Green , &Blue ) ;
	
	// 画面の大きさを得る
	GetDrawScreenSize( &ScreenWidth, &ScreenHeight ) ;

	// 描画領域を得る
	GetDrawArea( &DrawRect ) ;

	// ＶＲＡＭの使用、３Ｄ描画機能の使用変更
	SetUseSystemMemGraphCreateFlag( CheckFontChacheToTextureFlag( GetDefaultFontHandle() ) == TRUE ? FALSE : TRUE ) ;
	SetUse3DFlag( FALSE ) ;

	// 透過色を真っ黒にする
	SetTransColor( 0 , 0 , 0 ) ;

	// ＩＭＥを有効にする
	CharBuf.IMEUseFlag_System = TRUE ;
	RefreshIMEFlag() ;

	// 画面を保存するためのメモリを確保
	DrawWidth			= DrawRect.right  - DrawRect.left ;
	DrawHeight			= DrawRect.bottom - DrawRect.top ; 
	ScreenGraphFront	= MakeGraph( ScreenWidth, ScreenHeight ) ;
	ScreenGraphBack		= MakeGraph( ScreenWidth, ScreenHeight ) ;
	SetDrawScreen( DX_SCREEN_FRONT ) ;
	SetDrawArea( 0, 0, ScreenWidth, ScreenHeight ) ;
	GetDrawScreenGraph( 0, 0, ScreenWidth, ScreenHeight, ScreenGraphFront ) ;
	SetDrawScreen( DX_SCREEN_BACK ) ;
	SetDrawArea( 0, 0, ScreenWidth, ScreenHeight ) ;
	GetDrawScreenGraph( 0, 0, ScreenWidth, ScreenHeight, ScreenGraphBack ) ;

	// 文字入力用バッファの確保
//	if( ( Buffer = ( wchar_t * )DXCALLOC( BufLength + 1 ) ) == NULL ) return -1 ;

	// 入力ハンドルを得る
	InputHandle = MakeKeyInput( BufLength , CancelValidFlag , SingleCharOnlyFlag , NumCharOnlyFlag, DoubleCharOnlyFlag, EnableNewLineFlag ) ;
	if( InputHandle != -1 )
	{
		// 入力をアクティブにする
		SetActiveKeyInput( InputHandle ) ;

		// 入力文字数を０にする
		FontSize = GetFontSize() ;

		// 座標の補正
//		x -= DrawRect.left ;
//		y -= DrawRect.top ;

		// １回描画
		DrawKeyInputString( x , y , InputHandle, DisplayCandidateList ) ;

		// 描画先を裏画面に
		SetDrawScreen( DX_SCREEN_BACK ) ;
		while( ProcessMessage() == 0 )
		{
			// 入力が終了している場合は終了
			EndFlag = CheckKeyInput( InputHandle ) ;
			if( EndFlag ) break ;

			// 入力文字の更新
			{
				// もと画面の描画
				DrawGraph( 0 , 0 , ScreenGraphFront , FALSE ) ;

				// ＩＭＥ入力モードの描画
				DrawKeyInputModeString( DrawRect.right , DrawRect.bottom ) ; 

				// 入力状態の描画
				SetDrawArea( DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom ) ;
				DrawKeyInputString( x , y , InputHandle, DisplayCandidateList ) ;
				SetDrawArea( 0, 0, ScreenWidth, ScreenHeight ) ;

				// 表画面に転送
				ScreenFlip() ;
			}
		}
	}

	// ウインドウが閉じられた場合は−１を返す
	if( ProcessMessage() == -1 ) return -1;

	// キャンセルされていない場合は文字列を取得
	if( EndFlag == 1 )
	{
		GetKeyInputString_WCHAR_T( StrBuffer , InputHandle ) ;
	}

	// 画面を元にもどす
	SetDrawArea( 0, 0, ScreenWidth, ScreenHeight ) ;
	DrawGraph( 0, 0, ScreenGraphBack, FALSE ) ;
	SetDrawScreen( DX_SCREEN_FRONT ) ;
	DrawGraph( 0, 0, ScreenGraphFront, FALSE ) ;

	// グラフィックデータを消す
	DeleteKeyInput( InputHandle ) ;
	DeleteGraph( ScreenGraphFront ) ;
	DeleteGraph( ScreenGraphBack ) ;

	// 描画設定情報を元に戻す
	Graphics_DrawSetting_SetScreenDrawSettingInfo( &ScreenDrawSettingInfo ) ;
	SetTransColor( Red , Green , Blue ) ;

	// 終了
	return EndFlag ;
}


// 文字列の入力取得
extern int NS_KeyInputString( int x , int y , size_t CharMaxLength , TCHAR *StrBuffer , int CancelValidFlag )
{
	return NS_InputStringToCustom( x , y , CharMaxLength , StrBuffer , CancelValidFlag , FALSE , FALSE, FALSE ) ;
}

// 文字列の入力取得
extern int KeyInputString_WCHAR_T( int x , int y , size_t CharMaxLength , wchar_t *StrBuffer , int CancelValidFlag )
{
	return InputStringToCustom_WCHAR_T( x , y , CharMaxLength , StrBuffer , CancelValidFlag , FALSE , FALSE, FALSE ) ;
}

// 半角文字列のみの入力取得
extern int NS_KeyInputSingleCharString( int x , int y , size_t CharMaxLength , TCHAR *StrBuffer , int CancelValidFlag )
{
	return NS_InputStringToCustom( x , y, CharMaxLength , StrBuffer , CancelValidFlag , TRUE , FALSE, FALSE ) ;
}

// 半角文字列のみの入力取得
extern int KeyInputSingleCharString_WCHAR_T( int x , int y , size_t CharMaxLength , wchar_t *StrBuffer , int CancelValidFlag )
{
	return InputStringToCustom_WCHAR_T( x , y, CharMaxLength , StrBuffer , CancelValidFlag , TRUE , FALSE, FALSE ) ;
}

// 数値の入力
extern int NS_KeyInputNumber( int x , int y , int MaxNum , int MinNum , int CancelValidFlag )
{
	int Num ;
	wchar_t Buffer[ 50 ] ;
	int StrLen , hr ;

	// 最大文字列幅を取得
	{
		int MaxLen , MinLen ;

		_SWNPRINTF( Buffer, sizeof( Buffer ) / 2, L"%d", MaxNum ) ;
		MaxLen = ( int )_WCSLEN( Buffer ) ;

		_SWNPRINTF( Buffer, sizeof( Buffer ) / 2, L"%d", MinNum ) ;
		MinLen = ( int )_WCSLEN( Buffer ) ;

		StrLen = MaxLen > MinLen ? MaxLen : MinLen ;
	}

	// 数字を取得
	hr = InputStringToCustom_WCHAR_T( x , y , StrLen , Buffer , CancelValidFlag , TRUE , TRUE ) ;
	if( hr == -1 )
	{
		return MinNum - 1 ;
	}
	if( hr == 2 )
	{
		return MaxNum + 1 ;
	}

	// 数値に変換
	Num = _ATOIW( Buffer ) ;

	// 規定範囲の中に取得した数値が当てはまらない間は繰り返す
	while( ProcessMessage() == 0 && ( Num > MaxNum || Num < MinNum ) )
	{
		hr = InputStringToCustom_WCHAR_T( x , y , StrLen , Buffer , CancelValidFlag , TRUE , TRUE ) ;
		if( hr == -1 )
		{
			return MinNum - 1 ;
		}
		if( hr == 2 )
		{
			return MaxNum + 1 ;
		}

		// 数値に変換
		Num = _ATOIW( Buffer ) ;
	}

	// 終了
	return Num ;
}

// IMEの入力モード文字列の取得
extern int NS_GetIMEInputModeStr( TCHAR *GetBuffer ) 
{
#ifdef UNICODE
	return GetIMEInputModeStr_WCHAR_T( GetBuffer ) ;
#else
	wchar_t TempBuffer[ 128 ] ;
	int Result ;

	Result = GetIMEInputModeStr_WCHAR_T( TempBuffer ) ;

	ConvString( ( const char * )TempBuffer, -1, WCHAR_T_CHARCODEFORMAT, GetBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;

	return Result ;
#endif
}

// IMEの入力モード文字列の取得
extern int GetIMEInputModeStr_WCHAR_T( wchar_t *GetBuffer ) 
{
	HIMC	Imc ;
	DWORD	InputState , SentenceState , Buf ;
	wchar_t	*SelectStr ;
	static  int StringSetup = FALSE ;
	static  wchar_t ZenkakuKanaString[ 32 ] ;
	static  wchar_t HankakuKanaString[ 32 ] ;
	static  wchar_t ZenkakuHiraString[ 32 ] ;
	static  wchar_t ZenkakuEisuString[ 32 ] ;
	static  wchar_t HankakuEisuString[ 32 ] ;
	static  wchar_t FumeiString[ 32 ] ;

	SETUP_WIN_API

	// ＩＭＥが使われていないときは-1を返す
	if( CharBuf.IMESwitch == FALSE ) return -1 ;

	// 文字列の準備ができていない場合は準備を行う
	if( StringSetup == FALSE )
	{
		ConvString( "\x68\x51\xd2\x89\xab\x30\xbf\x30\xab\x30\xca\x30\x00"/*@ L"全角カタカナ" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )ZenkakuKanaString, sizeof( ZenkakuKanaString ), WCHAR_T_CHARCODEFORMAT ) ;
		ConvString( "\x4a\x53\xd2\x89\xab\x30\xbf\x30\xab\x30\xca\x30\x00"/*@ L"半角カタカナ" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )HankakuKanaString, sizeof( HankakuKanaString ), WCHAR_T_CHARCODEFORMAT ) ;
		ConvString( "\x68\x51\xd2\x89\x72\x30\x89\x30\x4c\x30\x6a\x30\x00"/*@ L"全角ひらがな" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )ZenkakuHiraString, sizeof( ZenkakuHiraString ), WCHAR_T_CHARCODEFORMAT ) ;
		ConvString( "\x68\x51\xd2\x89\xf1\x82\x70\x65\x00"/*@ L"全角英数" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )ZenkakuEisuString, sizeof( ZenkakuEisuString ), WCHAR_T_CHARCODEFORMAT ) ;
		ConvString( "\x4a\x53\xd2\x89\xf1\x82\x70\x65\x00"/*@ L"半角英数" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )HankakuEisuString, sizeof( HankakuEisuString ), WCHAR_T_CHARCODEFORMAT ) ;
		ConvString( "\x0d\x4e\x0e\x66\x6a\x30\x65\x51\x9b\x52\xe2\x30\xfc\x30\xc9\x30\x00"/*@ L"不明な入力モード" @*/, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )FumeiString, sizeof( FumeiString ), WCHAR_T_CHARCODEFORMAT ) ;

		StringSetup = TRUE ;
	}

	// 入力コンテキストを取得
	HWND DefHwnd = WinData.MainWindow;
	Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;

	// 入力モードを得る
	WinAPIData.ImmFunc.ImmGetConversionStatusFunc( Imc , &InputState , &SentenceState ) ;

	// 判断
	Buf = InputState & ( IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA ) ;
	if( Buf == ( IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA ) )
	{
		SelectStr = ZenkakuKanaString ;
	}
	else
	{
		Buf = InputState & ( IME_CMODE_NATIVE | IME_CMODE_KATAKANA ) ;
		if( Buf == ( IME_CMODE_NATIVE | IME_CMODE_KATAKANA ) )
		{
			SelectStr = HankakuKanaString ;
		}
		else
		{
			Buf = InputState & ( IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE ) ;
			if( Buf == ( IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE ) )
			{
				SelectStr = ZenkakuHiraString ;
			}
			else
			{
				Buf = InputState & IME_CMODE_FULLSHAPE ;
				if( Buf == IME_CMODE_FULLSHAPE )
				{
					SelectStr = ZenkakuEisuString ;
				}
				else
				{
					Buf = InputState & IME_CMODE_ALPHANUMERIC ;
					if( Buf == IME_CMODE_ALPHANUMERIC )
					{
						SelectStr = HankakuEisuString ;
					}
					else
					{
						SelectStr = FumeiString ;
					}
				}
			}
		}
	}

	_WCSCPY( GetBuffer , SelectStr ) ;

	// 入力コンテキストの削除
	WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );

	// 終了
	return 0 ;
}

// InputString関数使用時の文字の各色を変更する
extern int NS_SetKeyInputStringColor(
	ULONGLONG NmlStr ,				// 入力文字列の色
	ULONGLONG NmlCur ,				// ＩＭＥ非使用時のカーソルの色
	ULONGLONG IMEStrBack ,			// ＩＭＥ使用時の入力文字列の背景の色
	ULONGLONG IMECur ,				// ＩＭＥ使用時のカーソルの色
	ULONGLONG IMELine ,				// ＩＭＥ使用時の変換文字列の下線
	ULONGLONG IMESelectStr ,		// ＩＭＥ使用時の選択対象の変換候補文字列の色
	ULONGLONG IMEModeStr  ,			// ＩＭＥ使用時の入力モード文字列の色(「全角ひらがな」等)
	ULONGLONG NmlStrE ,				// 入力文字列の縁の色
	ULONGLONG IMESelectStrE ,		// ＩＭＥ使用時の選択対象の変換候補文字列の縁の色
	ULONGLONG IMEModeStrE	,		// ＩＭＥ使用時の入力モード文字列の縁の色
	ULONGLONG IMESelectWinE ,		// ＩＭＥ使用時の変換候補ウインドウの縁の色
	ULONGLONG IMESelectWinF	,		// ＩＭＥ使用時の変換候補ウインドウの下地の色
	ULONGLONG SelectStrBackColor ,	// 選択された入力文字列の背景の色
	ULONGLONG SelectStrColor ,		// 選択された入力文字列の色
	ULONGLONG SelectStrEdgeColor,	// 選択された入力文字列の縁の色
	ULONGLONG IMEStr,				// ＩＭＥ使用時の入力文字列の色
	ULONGLONG IMEStrE				// ＩＭＥ使用時の入力文字列の縁の色
	)
{
	if( NmlStr != -1 )
	{
		*((DWORD *)&CharBuf.NormalStrColor)     = *((DWORD *)&NmlStr) ;
		*((DWORD *)&CharBuf.IMEConvWinStrColor) = *((DWORD *)&NmlStr) ;
	}

	if( NmlCur != -1 )
	{
		*((DWORD *)&CharBuf.NormalCursorColor) = *((DWORD *)&NmlCur) ;
	}

	if( IMEStrBack != -1 )
	{
		*((DWORD *)&CharBuf.IMEStrBackColor) = *((DWORD *)&IMEStrBack) ;
	}

	if( IMECur != -1 )
	{
		*((DWORD *)&CharBuf.IMECursorColor) = *((DWORD *)&IMECur) ;
	}

	if( IMELine != -1 )
	{
		*((DWORD *)&CharBuf.IMELineColor) = *((DWORD *)&IMELine) ;
	}

	if( IMESelectStr != -1 )
	{
		*((DWORD *)&CharBuf.IMESelectStrBackColor)    = *((DWORD *)&IMESelectStr) ;
		*((DWORD *)&CharBuf.IMEConvWinSelectStrColor) = *((DWORD *)&IMESelectStr) ;
	}

	if( IMEModeStr != -1 )
	{
		*((DWORD *)&CharBuf.IMEModeStrColor) = *((DWORD *)&IMEModeStr) ;
	}

	if( NmlStrE != -1 )
	{
		*((DWORD *)&CharBuf.NormalStrEdgeColor)     = *((DWORD *)&NmlStrE) ;
		*((DWORD *)&CharBuf.IMEConvWinStrEdgeColor) = *((DWORD *)&NmlStrE) ;
	}

	if( IMESelectStrE != -1 )
	{
		*((DWORD *)&CharBuf.IMEConvWinSelectStrEdgeColor) = *((DWORD *)&IMESelectStrE) ;
	}

	if( IMEModeStrE != -1 )
	{
		*((DWORD *)&CharBuf.IMEModeStrEdgeColor) = *((DWORD *)&IMEModeStrE) ;
	}

	if( IMESelectWinE != -1 )
	{
		*((DWORD *)&CharBuf.IMEConvWinEdgeColor) = *((DWORD *)&IMESelectWinE) ;
	}
	else
	{
		if( IMEStrBack != -1 )
		{
			CharBuf.IMEConvWinEdgeColor = CharBuf.IMEStrBackColor;
		}
	}

	if( IMESelectWinF != -1 )
	{
		*((DWORD *)&CharBuf.IMEConvWinBackColor) = *((DWORD *)&IMESelectWinF) ;
	}
	else
	{
		CharBuf.IMEConvWinBackColor = NS_GetColor( 0, 0, 0 );
	}

	if( SelectStrBackColor != -1 )
	{
		*((DWORD *)&CharBuf.SelectStrBackColor) = *((DWORD *)&SelectStrBackColor) ;
	}
	else
	{
		CharBuf.SelectStrBackColor = NS_GetColor( 255, 255, 255 );
	}

	if( SelectStrColor != -1 )
	{
		*((DWORD *)&CharBuf.SelectStrColor) = *((DWORD *)&SelectStrColor) ;
	}
	else
	{
		CharBuf.SelectStrColor = NS_GetColor( 0, 0, 0 );
	}

	if( SelectStrEdgeColor != -1 )
	{
		*((DWORD *)&CharBuf.SelectStrEdgeColor) = *((DWORD *)&SelectStrEdgeColor) ;
	}
	else
	{
		CharBuf.SelectStrEdgeColor = NS_GetColor( 128, 128, 128 );
	}

	if( IMEStr != -1 )
	{
		*((DWORD *)&CharBuf.IMEStrColor) = *((DWORD *)&IMEStr) ;
	}

	if( IMEStrE != -1 )
	{
		CharBuf.IMEStrEdgeColorEnable = TRUE ;
		*((DWORD *)&CharBuf.IMEStrEdgeColor) = *((DWORD *)&IMEStrE) ;
	}

	// 終了
	return 0 ;
}

// InputString関数使用時の文字の各色を変更する
extern int NS_SetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */, unsigned int Color )
{
	switch( TargetColor )
	{
	default :
		return -1 ;

	case DX_KEYINPSTRCOLOR_NORMAL_STR :
		CharBuf.NormalStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_NORMAL_STR_EDGE :
		CharBuf.NormalStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_NORMAL_CURSOR :
		CharBuf.NormalCursorColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR :
		CharBuf.SelectStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR_EDGE :
		CharBuf.SelectStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR_BACK :
		CharBuf.SelectStrBackColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR :
		CharBuf.IMEStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR_EDGE :
		CharBuf.IMEStrEdgeColorEnable = TRUE ;
		CharBuf.IMEStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR_BACK :
		CharBuf.IMEStrBackColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CURSOR :
		CharBuf.IMECursorColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_LINE :
		CharBuf.IMELineColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR :
		CharBuf.IMESelectStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR_EDGE :
		CharBuf.IMESelectStrEdgeColorEnable = TRUE ;
		CharBuf.IMESelectStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR_BACK :
		CharBuf.IMESelectStrBackColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR :
		CharBuf.IMEConvWinStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR_EDGE :
		CharBuf.IMEConvWinStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR :
		CharBuf.IMEConvWinSelectStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_EDGE :
		CharBuf.IMEConvWinSelectStrEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_BACK :
		CharBuf.IMEConvWinSelectStrBackColorEnable = TRUE ;
		CharBuf.IMEConvWinSelectStrBackColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_EDGE :
		CharBuf.IMEConvWinEdgeColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_BACK :
		CharBuf.IMEConvWinBackColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_MODE_STR :
		CharBuf.IMEModeStrColor = Color ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_MODE_STR_EDGE :
		CharBuf.IMEModeStrEdgeColor = Color ;
		break ;
	}

	// 正常終了
	return 0 ;
}

// SetKeyInputStringColor2 で設定した色をデフォルトに戻す
extern int NS_ResetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */ )
{
	switch( TargetColor )
	{
	default :
		return -1 ;

	case DX_KEYINPSTRCOLOR_NORMAL_STR :
		CharBuf.NormalStrColor = NS_GetColor( 255 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_NORMAL_STR_EDGE :
		CharBuf.NormalStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_NORMAL_CURSOR :
		CharBuf.NormalCursorColor = NS_GetColor( 255 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR :
		CharBuf.SelectStrColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR_EDGE :
		CharBuf.SelectStrEdgeColor = NS_GetColor( 128 , 128 , 128 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_SELECT_STR_BACK :
		CharBuf.SelectStrBackColor = NS_GetColor( 255 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR :
		CharBuf.IMEStrColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR_EDGE :
		CharBuf.IMEStrEdgeColorEnable = FALSE ;
		CharBuf.IMEStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_STR_BACK :
		CharBuf.IMEStrBackColor = NS_GetColor( 255 , 255 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CURSOR :
		CharBuf.IMECursorColor = NS_GetColor(   0 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_LINE :
		CharBuf.IMELineColor = NS_GetColor( 255 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR :
		CharBuf.IMESelectStrColor = NS_GetColor( 0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR_EDGE :
		CharBuf.IMESelectStrEdgeColorEnable = FALSE ;
		CharBuf.IMESelectStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_SELECT_STR_BACK :
		CharBuf.IMESelectStrBackColor = NS_GetColor( 255 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR :
		CharBuf.IMEConvWinStrColor = NS_GetColor( 255 , 255 , 255 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR_EDGE :
		CharBuf.IMEConvWinStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR :
		CharBuf.IMEConvWinSelectStrColor = NS_GetColor( 255 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_EDGE :
		CharBuf.IMEConvWinSelectStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_BACK :
		CharBuf.IMEConvWinSelectStrBackColorEnable = FALSE ;
		CharBuf.IMEConvWinSelectStrBackColor = NS_GetColor( 0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_EDGE :
		CharBuf.IMEConvWinEdgeColor = NS_GetColor( 255 , 255 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_CONV_WIN_BACK :
		CharBuf.IMEConvWinBackColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_MODE_STR :
		CharBuf.IMEModeStrColor = NS_GetColor( 255 , 255 ,  20 ) ;
		break ;

	case DX_KEYINPSTRCOLOR_IME_MODE_STR_EDGE :
		CharBuf.IMEModeStrEdgeColor = NS_GetColor(   0 ,   0 ,   0 ) ;
		break ;
	}

	// 正常終了
	return 0 ;
}

// キー入力文字列描画関連で使用するフォントのハンドルを変更する(-1でデフォルトのフォントハンドル)
extern int NS_SetKeyInputStringFont( int FontHandle )
{
	CharBuf.UseFontHandle = FontHandle ;

	// 終了
	return 0 ;
}

// キー入力文字列処理の入力文字数が限界に達している状態で、文字列の末端部分で入力が行われた場合の処理モードを変更する
extern int NS_SetKeyInputStringEndCharaMode( int EndCharaMode /* DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE 等 */ )
{
	CharBuf.EndCharaMode = EndCharaMode ;

	// 終了
	return 0 ;
}

// 入力モード文字列を描画する
extern int NS_DrawKeyInputModeString( int x , int y )
{
	RECT DrawRect ;
	int FontSize, FontHandle ;

	FontHandle = CharBuf.UseFontHandle == -1 ? NS_GetDefaultFontHandle() : CharBuf.UseFontHandle ;
	FontSize = NS_GetFontSizeToHandle( FontHandle ) ;
	NS_GetDrawArea( &DrawRect ) ;

	// 入力モードを描画
	if( CharBuf.IMESwitch )
	{
		wchar_t InputModeStr[ 20 ] ;
		int Width ;

		// 文字列取得
		if( GetIMEInputModeStr_WCHAR_T( InputModeStr ) != -1 )
		{
			int StrLength = ( int )_WCSLEN( InputModeStr ) ;

			// 幅取得
			Width = GetDrawStringWidthToHandle_WCHAR_T( InputModeStr, 0, StrLength, FontHandle, FALSE ) ;

			if( Width    + x > DrawRect.right  ) x = DrawRect.right  - Width ;
			if( FontSize + y > DrawRect.bottom ) y = DrawRect.bottom - ( FontSize + 2 ) ;
			if( x < DrawRect.left ) x = DrawRect.left ;
			if( y < DrawRect.top  ) y = DrawRect.top ;

			// 描画
			DrawStringToHandle_WCHAR_T( x , y , InputModeStr , StrLength, CharBuf.IMEModeStrColor , FontHandle , CharBuf.IMEModeStrEdgeColor, FALSE )  ;
		}
	}

	// 終了
	return 0 ;
}

// キー入力データ初期化
extern int NS_InitKeyInput( void )
{
	int i ;
	INPUTDATA * Input ;

	Input = CharBuf.InputData ;
	for( i = 0 ; i < MAX_INPUT_NUM ; i ++, Input ++ )
	{
		if( Input->UseFlag == TRUE )
		{
			NS_DeleteKeyInput( i | DX_HANDLETYPE_MASK_KEYINPUT | ( Input->ID << DX_HANDLECHECK_ADDRESS ) ) ;
		}
	}

	// ＩＭＥの入力情報取得用に確保しているメモリがある場合は解放
	if( CharBuf.IMEInputData )
	{
		DXFREE( CharBuf.IMEInputData ) ;
		CharBuf.IMEInputData = NULL ;
	}

	// アクティブハンドルを-1にする
	CharBuf.ActiveInputHandle = -1 ;

	// 終了
	return 0 ;
}

// 新しいキー入力データの作成
extern int NS_MakeKeyInput( size_t MaxStrLength , int CancelValidFlag , int SingleCharOnlyFlag , int NumCharOnlyFlag, int DoubleCharOnlyFlag, int EnableNewLineFlag )
{
	int i, Result ;
	INPUTDATA * Input ;

	// MaxStrLength が 0 だった場合はエラー
	if( MaxStrLength <= 0 )
	{
		return -1 ;
	}

	// 使われていないキー入力データを探す
	for( i = 0 ; i != MAX_INPUT_NUM && CharBuf.InputData[ i ].UseFlag ; i ++ ){}
	if( i == MAX_INPUT_NUM ) return -1 ;
	Input = &CharBuf.InputData[ i ] ;

	// データの初期化
	_MEMSET( Input, 0, sizeof( *Input ) ) ;
#ifdef UNICODE
	Input->Buffer = ( wchar_t * )DXCALLOC( ( MaxStrLength + 1 ) * sizeof( wchar_t ) ) ;
#else // UNICODE
	Input->TempBufferBytes = ( MaxStrLength + 1 ) * 2 ;
	Input->Buffer = ( wchar_t * )DXCALLOC( ( MaxStrLength + 1 ) * sizeof( wchar_t ) + ( MaxStrLength + 1 ) * 2 ) ;
	Input->TempBuffer = ( char * )( Input->Buffer + MaxStrLength + 1 ) ;
#endif // UNICODE
	if( Input->Buffer == NULL )
	{
		return -1 ;
	}
	Input->MaxStrLength			= ( int )MaxStrLength ;
	Input->CancelValidFlag		= CancelValidFlag ;
	Input->SingleCharOnlyFlag	= SingleCharOnlyFlag ;
	Input->NumCharOnlyFlag		= NumCharOnlyFlag ;
	Input->DoubleCharOnlyFlag	= DoubleCharOnlyFlag ;
	Input->EnableNewLineFlag	= EnableNewLineFlag ;
	Input->UseFlag				= TRUE ;
	Input->ID					= CharBuf.HandleID ;
	Input->SelectStart			= -1 ;
	Input->SelectEnd			= -1 ;
	CharBuf.HandleID ++ ;
	if( CharBuf.HandleID >= ( DX_HANDLECHECK_MASK >> DX_HANDLECHECK_ADDRESS ) )
	{
		CharBuf.HandleID = 0 ;
	}

	// ＩＭＥを有効にする
	CharBuf.IMEUseFlag_System = TRUE ;
	RefreshIMEFlag() ;

	// 文字コードバッファをクリアする
	NS_ClearInputCharBuf() ;

	// ハンドル値の作成
	Result = i | DX_HANDLETYPE_MASK_KEYINPUT | ( Input->ID << DX_HANDLECHECK_ADDRESS ) ;

	// 終了
	return Result ;
}

// キー入力データの削除
extern int NS_DeleteKeyInput( int InputHandle )
{
	INPUTDATA * Input ;
	int i ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 確保していたメモリの解放
	if( Input->Buffer != NULL ) DXFREE( Input->Buffer ) ;

	// データの初期化
	_MEMSET( &CharBuf.InputData[ InputHandle & DX_HANDLEINDEX_MASK ], 0, sizeof( INPUTDATA ) ) ;
	
	if( CharBuf.ActiveInputHandle == InputHandle )
	{
		CharBuf.ActiveInputHandle = -1 ;
	}

	// すべてのキー入力が無効になっていればＩＭＥは無効にする
	for( i = 0 ; i != MAX_INPUT_NUM && !CharBuf.InputData[ i & DX_HANDLEINDEX_MASK ].UseFlag ; i ++ ){}
	if( i == MAX_INPUT_NUM )
	{
		CharBuf.IMEUseFlag_System = FALSE ;
		RefreshIMEFlag() ;
	}

	// 終了
	return 0 ;
}

// 指定のキー入力をアクティブにする
extern int NS_SetActiveKeyInput( int InputHandle )
{
	INPUTDATA * Input ;
	INPUTDATA * ActInput ;

	// ハンドルチェック
	Input = NULL ;
	if( InputHandle > 0 )
	{
		if( KEYHCHK( InputHandle, Input ) ) return -1 ;
	}

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;

	// 今までと同じ場合は何もしない
	if( InputHandle == CharBuf.ActiveInputHandle &&
		( ( Input != NULL && Input->EndFlag == FALSE ) || ( Input == NULL && InputHandle >= 0 ) ) ) return 0 ;

	// 今までアクティブだったハンドルで選択領域がある場合は解除する
	if( CharBuf.ActiveInputHandle != -1 )
	{
		if( KEYHCHK( CharBuf.ActiveInputHandle, ActInput ) ) return -1 ;

		ActInput->SelectStart = -1 ;
		ActInput->SelectEnd = -1 ;
	}

	if( InputHandle < 0 )
	{
		CharBuf.ActiveInputHandle = -1 ;
	}
	else
	{
		// アクティブな入力ハンドルを変更する前に入力文字列をクリアしておく
		NS_ClearInputCharBuf() ;

		CharBuf.ActiveInputHandle = InputHandle ;

		// エンドフラグの類を初期化する
		Input->EndFlag = FALSE ;
		Input->CancellFlag = FALSE ;
	}

	// 終了
	return 0 ;
}

// 現在アクティブになっているキー入力ハンドルを取得する
extern int NS_GetActiveKeyInput( void )
{
	return CharBuf.ActiveInputHandle ;
}

// 入力が終了しているか取得する
extern int NS_CheckKeyInput( int InputHandle )
{
	INPUTDATA * Input ;
	int Result ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	Result = Input->CancellFlag ? 2 : ( Input->EndFlag ? 1 : 0 ) ;

	return Result ;
}

// 入力が完了したキー入力を再度編集状態に戻す
extern int NS_ReStartKeyInput( int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	Input->EndFlag = FALSE ;

	// 終了
	return 0 ;
}

// 指定の文字列の指定の位置の文字のタイプを返す( 0:wchar_t一つで完結している文字  1:サロゲートペアのwchar_t一つめ  2:サロゲートペアのwchar_t二つ目 )
static int CheckWCharType( const wchar_t *String, int CharPosition )
{
	int i ;

	for( i = 0 ; String[ i ] != L'\0' ; )
	{
		if( CHECK_WCHAR_T_DOUBLE( String[ i ] ) == TRUE )
		{
			if( i     == CharPosition ) return 1 ;
			if( i + 1 == CharPosition ) return 2 ;
			i += 2 ;
		}
		else
		{
			if( i == CharPosition ) return 0 ;
			i ++ ;
		}
	}

	return -1 ;
}

// キー入力処理関数
extern int NS_ProcessActKeyInput( void )
{
	INPUTDATA * Input ;
	wchar_t C[ 3 ] = { 0 , 0 , 0 } ;
	wchar_t TempString[ CHARBUFFER_SIZE + 1 ] ;
#ifndef UNICODE
	char TempString2[ CHARBUFFER_SIZE * 2 + 1 ] ;
#endif // UNICODE
	int CharLen, len, DelNum ;
	HIMC Imc;

	SETUP_WIN_API

	// ＩＭＥのリフレッシュ処理
	{
		HWND DefHwnd;
		static wchar_t str[256];

		switch( CharBuf.IMERefreshStep )
		{
		case 1:
//			DefHwnd = ImmGetDefaultIMEWnd( WinData.MainWindow ) ;
			DefHwnd = WinData.MainWindow;
			if( DefHwnd )
			{
				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;
				if( Imc )
				{
					_MEMSET( CharBuf.InputString, 0, CHARBUFFER_SIZE );
					_MEMSET( CharBuf.InputTempString, 0, CHARBUFFER_SIZE );
					WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPSTR , CharBuf.InputTempString , CHARBUFFER_SIZE );
					WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc , NI_COMPOSITIONSTR ,  CPS_CANCEL , 0  );

					WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
				}
			}
			CharBuf.IMERefreshStep ++ ;
//			break;

		case 2:
			SetIMEOpenState( FALSE );
			CharBuf.IMERefreshStep ++ ;
//			break;

		case 3:
			SetIMEOpenState( TRUE );
			CharBuf.IMERefreshStep ++ ;
//			break;

		case 4:
//			DefHwnd = ImmGetDefaultIMEWnd( WinData.MainWindow ) ;
			DefHwnd = WinData.MainWindow;
			if( DefHwnd )
			{
				Imc = WinAPIData.ImmFunc.ImmGetContextFunc( DefHwnd ) ;
				if( Imc )
				{
					WinAPIData.ImmFunc.ImmSetOpenStatusFunc( Imc, TRUE );
					WinAPIData.ImmFunc.ImmSetCompositionStringFunc( Imc, SCS_SETSTR, CharBuf.InputTempString, ( DWORD )_WCSLEN( CharBuf.InputTempString ), 0, 0 );
					WinAPIData.ImmFunc.ImmReleaseContextFunc( DefHwnd , Imc );
				}
			}
			CharBuf.IMERefreshStep = 0 ;
//			break;
		}
	}

	if( CharBuf.ActiveInputHandle == -1 )
	{
		return 0 ;
	}

	if( KEYHCHK( CharBuf.ActiveInputHandle, Input ) )
	{
		return -1 ;
	}

	if( Input->EndFlag )
	{
		return 0 ;
	}

	// ＩＭＥの入力文字数制限処理
	if( CharBuf.IMEInputStringMaxLengthIMESync || CharBuf.IMEInputMaxLength )
	{
		int i, maxlen ;
		int flag ;
		int InputStrLength ;
		int StrLength ;

		// 文字数が指定最大バイト数を超えていたら強制的に文字数を減らす
		flag = FALSE ;
#ifdef UNICODE
		InputStrLength = ( int )_WCSLEN( CharBuf.InputString ) ;
		StrLength      = Input->StrLength ;
#else // UNICODE
		// マルチバイト文字列としての長さを取得する
		{
			ConvString( ( const char * )Input->Buffer, -1, WCHAR_T_CHARCODEFORMAT, Input->TempBuffer, Input->TempBufferBytes, CHAR_CHARCODEFORMAT ) ;
			StrLength = ( int )CL_strlen( CHAR_CHARCODEFORMAT, Input->TempBuffer ) ;

			ConvString( ( const char * )CharBuf.InputString, -1, WCHAR_T_CHARCODEFORMAT, TempString2, sizeof( TempString2 ), CHAR_CHARCODEFORMAT ) ;
			InputStrLength = ( int )CL_strlen( CHAR_CHARCODEFORMAT, TempString2 ) ;
		}
#endif // UNICODE
		maxlen = 65536 ;
		if( CharBuf.IMEInputStringMaxLengthIMESync && StrLength + InputStrLength > Input->MaxStrLength )
		{
			flag = TRUE ;
			maxlen = Input->MaxStrLength - StrLength ;
		}

		if( CharBuf.IMEInputMaxLength && InputStrLength > CharBuf.IMEInputMaxLength )
		{
			flag = TRUE ;
			if( maxlen > CharBuf.IMEInputMaxLength )
			{
				maxlen = CharBuf.IMEInputMaxLength ;
			}
		}

		if( flag )
		{
			// 入力コンテキストの取得
			Imc = WinAPIData.ImmFunc.ImmGetContextFunc( WinData.MainWindow ) ;
			if( Imc )
			{
				// 実バッファにある文字列もそうなっているか調べる
				_MEMSET( TempString, 0, sizeof( TempString ) );
				WinAPIData.ImmFunc.ImmGetCompositionStringFunc( Imc , GCS_COMPSTR , TempString , CHARBUFFER_SIZE );
#ifdef UNICODE
				len = ( int )_WCSLEN( TempString ) ;
#else // UNICODE
				// マルチバイト文字列に変換
				ConvString( ( const char * )TempString, -1, WCHAR_T_CHARCODEFORMAT, TempString2, sizeof( TempString2 ), CHAR_CHARCODEFORMAT ) ;

				// 文字列の長さを取得
				len = ( int )CL_strlen( CHAR_CHARCODEFORMAT, TempString2 ) ;
#endif // UNICODE
				if( len > maxlen )
				{
					// 文字列を短くする
#ifdef UNICODE
					for( i = 0; i < len ; )
					{
						if( CHECK_WCHAR_T_DOUBLE( TempString[ i ] ) )
						{
							if( i + 2 > maxlen ) break;
							i += 2 ;
						}
						else
						{
							if( i + 1 > maxlen ) break;
							i ++ ;
						}
					}
					TempString[ i ] = L'\0' ;
#else // UNICODE
					for( i = 0; i < len ; )
					{
						if( GetCharBytes_( &TempString2[ i ], CHAR_CHARCODEFORMAT ) == 2 )
						{
							if( i + 2 > maxlen ) break;
							i += 2 ;
						}
						else
						{
							if( i + 1 > maxlen ) break;
							i ++ ;
						}
					}
					TempString2[ i ] = '\0' ;

					// wchar_t文字列に変換
					ConvString( TempString2, -1, CHAR_CHARCODEFORMAT, ( char * )TempString, sizeof( TempString ), WCHAR_T_CHARCODEFORMAT ) ;
#endif // UNICODE
					// 設定を初期化
					WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc , NI_COMPOSITIONSTR ,  CPS_CANCEL , 0  );
					WinAPIData.ImmFunc.ImmSetCompositionStringFunc( Imc, SCS_SETSTR, TempString, ( DWORD )_WCSLEN( TempString ), 0, 0 );
				}

				// 入力コンテキストの削除
				WinAPIData.ImmFunc.ImmReleaseContextFunc( WinData.MainWindow , Imc );
			}
		}
	}

	// キーコードバッファの続く限り処理
	while( Input->EndFlag == FALSE )
	{
		CharLen = GetOneChar_WCHAR_T( C , TRUE ) ;
		if( CharLen == 0 )
		{
			break ;
		}

		// カーソル点滅処理のカウンタをリセット
		ResetKeyInputCursorBrinkCount() ;
		
		switch( C[0] )
		{
			// 上下タブキーは無視
		case CTRL_CODE_TAB :
			break ;

			// ホームボタン
		case CTRL_CODE_HOME :
			if( NS_CheckHitKey( KEY_INPUT_LCONTROL ) || NS_CheckHitKey( KEY_INPUT_RCONTROL ) )
			{
				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = Input->Point ;
						Input->SelectEnd   = 0 ;
					}
					else
					{
						Input->SelectEnd   = 0 ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
				Input->Point = 0 ;
			}
			else
			if( Input->Point > 0 )
			{
				int OldPos ;
				int CharNum ;

				OldPos = Input->Point ;

				// 行の先頭に移動
				do
				{
					CharNum = 1 ;
					if( Input->Point > 1 )
					{
						CharNum = CheckWCharType( Input->Buffer, Input->Point - 1 ) == 2 ? 2 : 1 ;
					}

					Input->Point -= CharNum ;
				}while( Input->Point > 0 && Input->Buffer[ Input->Point ] != L'\n' ) ;
				if( Input->Buffer[ Input->Point ] == L'\n' )
				{
					Input->Point ++ ;
				}

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = OldPos ;
						Input->SelectEnd   = Input->Point ;
					}
					else
					{
						Input->SelectEnd   = Input->Point ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
			}
			break ;

			// エンドボタン
		case CTRL_CODE_END :
			if( NS_CheckHitKey( KEY_INPUT_LCONTROL ) || NS_CheckHitKey( KEY_INPUT_RCONTROL ) )
			{
				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = Input->Point ;
						Input->SelectEnd   = Input->StrLength ;
					}
					else
					{
						Input->SelectEnd   = Input->StrLength ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
				Input->Point = Input->StrLength ;
			}
			else
			{
				int OldPos ;

				OldPos = Input->Point ;

				// 行の終端に移動
				while( Input->Point < Input->StrLength && Input->Buffer[ Input->Point ] != L'\n' )
				{
					len = 1 ;
					if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[ Input->Point ] ) == TRUE )
					{
						len = 2 ;
					}

					Input->Point += len ;
				}

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = OldPos ;
						Input->SelectEnd   = Input->Point ;
					}
					else
					{
						Input->SelectEnd   = Input->Point ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
			}
			break ;

			// カット
		case CTRL_CODE_CUT :
CUT:
			if( Input->SelectStart != -1 )
			{
				wchar_t *Buffer ;
				int smin, smax, size ;

				if( Input->SelectStart > Input->SelectEnd )
				{
					smin = Input->SelectEnd ;
					smax = Input->SelectStart ;
				}
				else
				{
					smin = Input->SelectStart ;
					smax = Input->SelectEnd ;
				}
				size = smax - smin ;
				Buffer = ( wchar_t * )DXALLOC( ( size + 1 ) * sizeof( wchar_t ) ) ;
				if( Buffer )
				{
					_MEMCPY( Buffer, &Input->Buffer[ smin ], size * sizeof( wchar_t ) ) ;
					Buffer[ size ] = L'\0' ;
					SetClipboardText_WCHAR_T( Buffer ) ;
					DXFREE( Buffer ) ;
				}
				KeyInputSelectAreaDelete( Input ) ;
			}
			break ;

			// コピー
		case CTRL_CODE_COPY :
			if( Input->SelectStart != -1 )
			{
				wchar_t *Buffer ;
				int smin, smax, size ;

				if( Input->SelectStart > Input->SelectEnd )
				{
					smin = Input->SelectEnd ;
					smax = Input->SelectStart ;
				}
				else
				{
					smin = Input->SelectStart ;
					smax = Input->SelectEnd ;
				}
				size = smax - smin ;
				Buffer = ( wchar_t * )DXALLOC( ( size + 1 ) * sizeof( wchar_t ) ) ;
				if( Buffer )
				{
					_MEMCPY( Buffer, &Input->Buffer[ smin ], size * sizeof( wchar_t ) ) ;
					Buffer[ size ] = L'\0' ;
					SetClipboardText_WCHAR_T( Buffer ) ;
					DXFREE( Buffer ) ;
				}
			}
			break ;

			// ペースト
		case CTRL_CODE_PASTE :
			if( GetClipboardText_WCHAR_T( NULL ) != -1 )
			{
				int Size ;
				wchar_t *Buffer, *p ;

				Size = GetClipboardText_WCHAR_T( NULL ) ;
				Buffer = ( wchar_t * )DXALLOC( ( size_t )Size ) ;
				if( Buffer )
				{
					GetClipboardText_WCHAR_T( Buffer ) ;
					for( p = Buffer ; *p ; p++ )
					{
						if( CHECK_WCHAR_T_DOUBLE( *p ) == TRUE )
						{
							StockInputChar_WCHAR_T( *p ) ;
							p++ ;
							StockInputChar_WCHAR_T( *p ) ;
						}
						else
						{
							if( Input->EnableNewLineFlag || ( *p != L'\r' && *p != L'\n' ) )
							{
								StockInputChar_WCHAR_T( *p ) ;
							}
						}
					}
					DXFREE( Buffer ) ;
				}
			}
			break ;

			// エンター文字だった場合
		case CTRL_CODE_CR :
			// 改行が許可されている場合は改行文字を追加する
			if( Input->EnableNewLineFlag )
			{
				C[ 0 ] = '\n' ;
				goto DEFAULTCHARADD ;
			}
			else
			{
				// そうでない場合は改行で終了
				Input->EndFlag = TRUE ;
				CharBuf.ActiveInputHandle = -1 ;
			}
			break ;

			// エスケープキーだった場合はキャンセル
		case CTRL_CODE_ESC :
			if( Input->CancelValidFlag )
			{
				Input->CancellFlag = TRUE ;
				Input->EndFlag = TRUE ;
				CharBuf.ActiveInputHandle = -1 ;
			}
			break ;

			// デリートキーだった場合は文字の削除
		case CTRL_CODE_DEL :
			if( Input->SelectStart == -1 )
			{
				if( Input->Point != Input->StrLength )
				{
					DelNum = CHECK_WCHAR_T_DOUBLE( Input->Buffer[Input->Point] ) == TRUE ? 2 : 1 ;  
					_MEMMOVE( &Input->Buffer[ Input->Point ] , &Input->Buffer[ Input->Point + DelNum ] , ( Input->StrLength - Input->Point ) * sizeof( wchar_t ) ) ;
					Input->StrLength -= DelNum ;
				}
			}
			else
			{
				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					goto CUT ;
				}
				KeyInputSelectAreaDelete( Input ) ;
			}
			break ;

			// バックスペースキーだった場合は一つ前の文字を削除
		case CTRL_CODE_BS :
			if( Input->SelectStart == -1 )
			{
				if( Input->Point > 0 )
				{
					int DelNewLine ;

					DelNum = 1 ;
					if( Input->Point > 1 )
					{
						DelNum = CheckWCharType( Input->Buffer, Input->Point - 1 ) == 2 ? 2 : 1 ;
					}

					DelNewLine = Input->Buffer[ Input->Point - DelNum ] == L'\n' ? TRUE : FALSE ;
		
					Input->Point -= DelNum ;
					_MEMMOVE( &Input->Buffer[ Input->Point ] , &Input->Buffer[ Input->Point + DelNum ] , ( Input->StrLength - Input->Point ) * sizeof( wchar_t ) ) ;
					Input->StrLength -= DelNum ;

					// もし改行文字を削除した場合は描画開始位置を変更するかチェックする
					if( DelNewLine )
					{
						if( Input->Point <= Input->DrawStartPos )
						{
							int CharNum ;

							// 行の先頭に描画開始位置を移動する
							while( Input->DrawStartPos > 0 )
							{
								CharNum = 1 ;
								if( Input->DrawStartPos > 1 )
								{
									CharNum = CheckWCharType( Input->Buffer, Input->DrawStartPos - 1 ) == 2 ? 2 : 1 ;
								}

								if( CharNum == 1 )
								{
									if( Input->Buffer[ Input->DrawStartPos - CharNum ] == L'\n' )
									{
										break ;
									}
								}

								Input->DrawStartPos -= CharNum ;
							}
						}
					}
				}
			}
			else
			{
				KeyInputSelectAreaDelete( Input ) ;
			}
			break ;

			// 左右キーの場合は一文字移動
		case CTRL_CODE_LEFT :
			if( Input->Point > 0 )
			{
				int MoveNewLine ;

				len = 1 ;
				if( Input->Point > 1 && CheckWCharType( Input->Buffer, Input->Point - 1 ) == 2 )
				{
					len = 2 ;
				}

				MoveNewLine = Input->Buffer[ Input->Point - len ] == L'\n' ? TRUE : FALSE ;

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = Input->Point ;
						Input->SelectEnd   = Input->Point - len ;
					}
					else
					{
						Input->SelectEnd   = Input->Point - len ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
				Input->Point -= len ;

				// もし改行文字を移動した場合は描画開始位置を変更するかチェックする
				if( MoveNewLine )
				{
					if( Input->Point <= Input->DrawStartPos )
					{
						int CharNum ;

						// 行の先頭に描画開始位置を移動する
						Input->DrawStartPos = Input->Point ;
						while( Input->DrawStartPos > 0 )
						{
							CharNum = 1 ;
							if( Input->DrawStartPos > 1 )
							{
								CharNum = CheckWCharType( Input->Buffer, Input->DrawStartPos - 1 ) == 2 ? 2 : 1 ;
							}

							if( CharNum == 1 )
							{
								if( Input->Buffer[ Input->DrawStartPos - CharNum ] == L'\n' )
								{
									break ;
								}
							}

							Input->DrawStartPos -= CharNum ;
						}
					}
				}
			}
			break ;

		case CTRL_CODE_RIGHT :
			if( Input->Point < Input->StrLength )
			{
				len = 1 ;
				if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[Input->Point] ) == TRUE )
				{
					len = 2 ;
				}

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = Input->Point ;
						Input->SelectEnd   = Input->Point + len ;
					}
					else
					{
						Input->SelectEnd   = Input->Point + len ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
				Input->Point += len ;
			}
			break ;

			// 上下キーは行を移動
		case CTRL_CODE_UP :
			if( Input->Point > 0 )
			{
				int CharPos ;
				int CharNum ;
				int MoveLength ;
				int i ;
				int OldPos ;

				OldPos = Input->Point ;

				// カーソルの位置が行の先頭かどうかで処理を分岐
				if( Input->Buffer[ Input->Point - 1 ] == L'\n' )
				{
					// カーソルの位置が行の先頭だった場合

					// 一番上の行だった場合とそうでない場合で処理を分岐
					if( Input->Point == 1 )
					{
						Input->Point = 0 ;
					}
					else
					// ひとつ上の行に１文字もない場合とある場合で処理を分岐
					if( Input->Buffer[ Input->Point - 2 ] == L'\n' )
					{
						Input->Point -- ;
					}
					else
					{
						// ひとつ上の行の先頭に移動
						Input->Point -- ;
						do
						{
							CharNum = 1 ;
							if( Input->Point > 1 )
							{
								CharNum = CheckWCharType( Input->Buffer, Input->Point - 1 ) == 2 ? 2 : 1 ;
							}

							Input->Point -= CharNum ;
						}while( Input->Point >= 0 && Input->Buffer[ Input->Point ] != L'\n' ) ;
						Input->Point ++ ;
					}

					if( Input->DrawStartPos > Input->Point )
					{
						Input->DrawStartPos = Input->Point ;
					}
				}
				else
				{
					// 行の先頭に移動
					CharPos = 0 ;
					MoveLength = 0 ;
					do
					{
						CharNum = 1 ;
						if( Input->Point - MoveLength > 1 )
						{
							CharNum = CheckWCharType( Input->Buffer, Input->Point - MoveLength - 1 ) == 2 ? 2 : 1 ;
						}

						MoveLength += CharNum ;
						CharPos ++ ;
					}while( Input->Point - MoveLength >= 0 && Input->Buffer[ Input->Point - MoveLength ] != L'\n' ) ;

					// 一番上の行だった場合は何もしない
					if( Input->Point - MoveLength < 0 )
					{
						// 選択していたら解除処理を行う
						if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) == 0 && NS_CheckHitKey( KEY_INPUT_RSHIFT ) == 0 )
						{
							Input->SelectStart = -1 ;
							Input->SelectEnd = -1 ;
						}

						break ;
					}

					// ひとつ上の行の先頭に移動
					Input->Point -= MoveLength ;
					do
					{
						CharNum = 1 ;
						if( Input->Point > 1 )
						{
							CharNum = CheckWCharType( Input->Buffer, Input->Point - 1 ) == 2 ? 2 : 1 ;
						}

						Input->Point -= CharNum ;
					}while( Input->Point > 0 && Input->Buffer[ Input->Point ] != L'\n' ) ;

					// 移動前の行と同じ文字数の箇所に移動
					if( Input->Point != 0 )
					{
						Input->Point ++ ;
					}
					if( Input->DrawStartPos > Input->Point )
					{
						Input->DrawStartPos = Input->Point ;
					}
					CharPos -- ;
					for( i = 0 ; i < CharPos && Input->Buffer[ Input->Point ] != L'\n' ; i ++ )
					{
						len = 1 ;
						if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[ Input->Point ] ) == TRUE )
						{
							len = 2 ;
						}

						Input->Point += len ;
					}
				}

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = OldPos ;
						Input->SelectEnd   = Input->Point ;
					}
					else
					{
						Input->SelectEnd   = Input->Point ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
			}
			break ;

		case CTRL_CODE_DOWN :
			if( Input->Point < Input->StrLength )
			{
				int CharPos ;
				int CharNum ;
				int MoveLength ;
				int i ;
				int OldPos ;

				OldPos = Input->Point ;

				// 行の先頭までの文字数を数える
				CharPos = 0 ;
				MoveLength = 0 ;
				do
				{
					CharNum = 1 ;
					if( Input->Point - MoveLength > 1 )
					{
						CharNum = CheckWCharType( Input->Buffer, Input->Point - MoveLength - 1 ) == 2 ? 2 : 1 ;
					}

					MoveLength += CharNum ;
					CharPos ++ ;
				}while( Input->Point - MoveLength > 0 && Input->Buffer[ Input->Point - MoveLength ] != L'\n' ) ;

				if( Input->Point - MoveLength == 0 )
				{
					CharPos ++ ;
				}

				// ひとつ下の行に移動
				MoveLength = 0 ;
				while( Input->Point + MoveLength < Input->StrLength && Input->Buffer[ Input->Point + MoveLength ] != L'\n' )
				{
					len = 1 ;
					if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[ Input->Point + MoveLength ] ) == TRUE )
					{
						len = 2 ;
					}

					MoveLength += len ;
				}

				// ひとつ下の行がない場合は何もせずに終了
				if( Input->Point + MoveLength == Input->StrLength )
				{
					// 選択していたら解除処理を行う
					if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) == 0 && NS_CheckHitKey( KEY_INPUT_RSHIFT ) == 0 )
					{
						Input->SelectStart = -1 ;
						Input->SelectEnd = -1 ;
					}

					break ;
				}

				// 現在の行の末端に移動
				Input->Point += MoveLength ;

				// ひとつ下の行の、移動前の行と同じ文字数の箇所に移動
				Input->Point ++ ;
				CharPos -- ;
				for( i = 0 ; i < CharPos && Input->Point < Input->StrLength && Input->Buffer[ Input->Point ] != L'\n' ; i ++ )
				{
					len = 1 ;
					if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[ Input->Point ] ) == TRUE )
					{
						len = 2 ;
					}

					Input->Point += len ;
				}

				if( NS_CheckHitKey( KEY_INPUT_LSHIFT ) || NS_CheckHitKey( KEY_INPUT_RSHIFT ) )
				{
					if( Input->SelectStart == -1 )
					{
						Input->SelectStart = OldPos ;
						Input->SelectEnd   = Input->Point ;
					}
					else
					{
						Input->SelectEnd   = Input->Point ;
					}
				}
				else
				{
					Input->SelectStart = -1 ;
					Input->SelectEnd = -1 ;
				}
			}
			break ;

		default:
DEFAULTCHARADD:
			KeyInputSelectAreaDelete( Input ) ;

			// 数値文字のみ時の処理
			if( Input->NumCharOnlyFlag )
			{
				// 数値文字でなければキャンセル
				if( C[ 0 ] < L'0' || C[ 0 ] > L'9' )
				{
					// マイナス文字でカーソル位置が０だった場合は入力可能　 
					if( C[ 0 ] != L'-' || Input->Point != 0 )
					{
						// ピリオドも一個までＯＫ
						if( ( C[ 0 ] == L'.' && _WCSCHR( Input->Buffer, L'.' ) != NULL ) || C[ 0 ] != L'.' ) break ;
					}
				}
			}

			// 文字列の追加
			{
				int CharLen2 ;
				int CharLen3 ;
				int StrLength2 ;
#ifdef UNICODE
				char TempCharBuf[ 32 ] ;

				CharLen2   = CharLen ;
				StrLength2 = Input->StrLength ;

				ConvString( ( const char * )C, -1, WCHAR_T_CHARCODEFORMAT, TempCharBuf, sizeof( TempCharBuf ), CHAR_CHARCODEFORMAT ) ;
				CharLen3   = ( int )CL_strlen( CHAR_CHARCODEFORMAT, TempCharBuf ) ;

#else // UNICODE

				ConvString( ( const char * )C, -1, WCHAR_T_CHARCODEFORMAT, TempString2, sizeof( TempString2 ), CHAR_CHARCODEFORMAT ) ;
				CharLen2   = ( int )CL_strlen( CHAR_CHARCODEFORMAT, TempString2 ) ;
				CharLen3   = CharLen2 ;

				ConvString( ( const char * )Input->Buffer, -1, WCHAR_T_CHARCODEFORMAT, Input->TempBuffer, Input->TempBufferBytes, CHAR_CHARCODEFORMAT ) ;
				StrLength2 = ( int )CL_strlen( CHAR_CHARCODEFORMAT, Input->TempBuffer ) ;
#endif // UNICODE
				// １バイト文字のみ時の処理
				if( Input->SingleCharOnlyFlag )
				{
					if( CharLen3 != 1 ) break ;
				}

				// ２バイト文字のみ時の処理
				if( Input->DoubleCharOnlyFlag )
				{
					if( CharLen3 == 1 ) break ;
				}

				// カーソルの位置が文字列の末端で、且つ末端文字処理モードが
				// DX_KEYINPSTR_ENDCHARAMODE_NOTCHANGE だったら何もせず終了
				if( CharBuf.EndCharaMode != DX_KEYINPSTR_ENDCHARAMODE_NOTCHANGE ||
					StrLength2   + CharLen2 <= Input->MaxStrLength ||
					Input->Point + CharLen  <= Input->StrLength )
				{
					// バッファに空きがない時の処理
					while( StrLength2 + CharLen2 > Input->MaxStrLength )
					{
						int Pos, CLen = 0 ;

						// 最後の文字を削る
						Pos = 0 ;
						while( Pos < Input->StrLength )
						{
							if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[Pos] ) == TRUE )
							{
								CLen = 2 ;
							}
							else
							{
								CLen = 1 ;
							}
							Pos += CLen ;
						}
#ifdef UNICODE
						StrLength2 -= CLen ;
#else // UNICODE
						{
							int i ;
							int CLen2 ;

							for( i = 0 ; i < CLen ; i ++ )
							{
								TempString[ i ] = Input->Buffer[ Pos - CLen + i ] ;
							}
							TempString[ CLen ] = L'\0' ;
							ConvString( ( const char * )TempString, -1, WCHAR_T_CHARCODEFORMAT, TempString2, sizeof( TempString2 ), CHAR_CHARCODEFORMAT ) ;
							CLen2 = ( int )CL_strlen( CHAR_CHARCODEFORMAT, TempString2 ) ;

							StrLength2 -= CLen2 ;
						}
#endif // UNICODE
						Input->Buffer[ Pos - CLen ] = L'\0' ;
						Input->StrLength -= CLen ;

						// カーソルも移動する
						if( Input->Point == Pos )
						{
							Input->Point -= CLen ;
						}
					}
					
					_MEMMOVE( &Input->Buffer[ Input->Point + CharLen ] , &Input->Buffer[ Input->Point ] , ( Input->StrLength - Input->Point + 1 ) * sizeof( wchar_t ) ) ;
					_MEMMOVE( &Input->Buffer[ Input->Point ] , C , CharLen * sizeof( wchar_t ) ) ;

					Input->Point     += CharLen ;
					Input->StrLength += CharLen ;
				}
				break ;
			}
		}
	}

	// 終了
	return 0 ;
}

// キー入力ハンドルの入力中文字列を描画する際の描画範囲を設定する
extern int NS_SetKeyInputDrawArea( int x1, int y1, int x2, int y2, int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) )
	{
		return -1 ;
	}

	Input->DrawAreaValidFlag = TRUE ;
	Input->DrawArea.left = x1 ;
	Input->DrawArea.top = y1 ;
	Input->DrawArea.right = x2 ;
	Input->DrawArea.bottom = y2 ;

	// 終了
	return 0 ;
}

// キー入力中データの描画
extern int NS_DrawKeyInputString( int x , int y , int InputHandle , int DrawCandidateList )
{
	INPUTDATA * Input ;
	int Use3DFlag , FontSize , FontHandle ;
	int PointX ;
	int PointY ;
	int AddY ;
//	int StrWidth ;
	RECT DrawRect ;
	RECT DrawRectBackup ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;
	if( Input->EndFlag ) return -1 ;

	// 各種データ保存
	Use3DFlag = NS_GetUse3DFlag() ;

	// ３Ｄ描画機能の使用変更
	NS_SetUse3DFlag( FALSE ) ;

	// 使用するフォントのハンドルをセットする
	FontHandle = CharBuf.UseFontHandle == -1 ? NS_GetDefaultFontHandle() : CharBuf.UseFontHandle ;

	// フォントサイズの取得
	FontSize = NS_GetFontSizeToHandle( FontHandle ) ;
	
	// カーソル点滅処理
	if( CharBuf.CBrinkFlag == TRUE )
	{
		if( NS_GetNowCount() - CharBuf.CBrinkCount > CharBuf.CBrinkWait )
		{
			CharBuf.CBrinkDrawFlag = CharBuf.CBrinkDrawFlag == TRUE ? FALSE : TRUE ;
			CharBuf.CBrinkCount = NS_GetNowCount() ;
		}
	}
	else
	{
		CharBuf.CBrinkDrawFlag = TRUE ;
	}

	// 描画範囲の設定
	NS_GetDrawArea( &DrawRectBackup ) ;
	if( Input->DrawAreaValidFlag )
	{
		DrawRect = Input->DrawArea ;
		NS_SetDrawArea( Input->DrawArea.left, Input->DrawArea.top, Input->DrawArea.right, Input->DrawArea.bottom ) ;
	}
	else
	{
		DrawRect = DrawRectBackup ;
	}

	// 描画開始文字位置の決定
	{
		int DrawWidth, DrawHNum ;
		int tx, ty ;
//		int w ;
//		int SizeX, SizeY, LineCount ;
		int PosX, PosY ;

		AddY = FontSize + FontSize / 10 * 3 ;

		if( DrawRect.left == DrawRect.right ||
			DrawRect.bottom == DrawRect.top )
		{
			// 描画範囲を変更していた場合は元に戻す
			if( Input->DrawAreaValidFlag )
			{
				NS_SetDrawArea( DrawRectBackup.left, DrawRectBackup.top, DrawRectBackup.right, DrawRectBackup.bottom ) ;
			}
			return -1 ;
		}

		tx = x ;
		ty = y ;
		if( tx < DrawRect.left ) tx = DrawRect.left ;
		if( ty < DrawRect.top  ) ty = DrawRect.top ;

		while( tx > DrawRect.right )
		{
			tx -= DrawRect.right - DrawRect.left ;
			ty += AddY ;
		}

		DrawHNum = ( DrawRect.bottom - ty ) / AddY ;
		if( DrawHNum == 0 )
		{
			DrawWidth = DrawRect.right - tx ;
			DrawHNum = 1 ;
		}
		else
		{
			DrawWidth = ( DrawRect.right - tx ) + ( DrawHNum - 1 ) * ( DrawRect.right - DrawRect.left ) ;
		}

		DrawObtainsString_WCHAR_T(
			x, y,
			AddY,
			Input->Buffer,
			-1,
			0, 0, FontHandle, 0, 0, 0, 0, 0, FALSE, &PosX, &PosY ) ;
		if( PosY < DrawRect.bottom - AddY )
		{
			Input->DrawStartPos = 0 ;
		}
		else
		{
			if( Input->DrawStartPos > Input->Point )
			{
				Input->DrawStartPos = Input->Point ;
			}
			else
			{
				for(;;)
				{
					DrawObtainsString_WCHAR_T(
						x, y,
						AddY,
						Input->Buffer + Input->DrawStartPos,
						Input->Point  - Input->DrawStartPos,
						0, 0, FontHandle, 0, 0, 0, 0, 0, FALSE, &PosX, &PosY ) ;

					if( Input->DrawStartPos >= Input->Point )
					{
						break ;
					}

					if( DrawHNum == 1 )
					{
						if( PosY == y )
						{
							break ;
						}
					}
					else
					{
						if( PosY <= DrawRect.bottom - AddY )
						{
							break ;
						}
					}

					if( CHECK_WCHAR_T_DOUBLE( Input->Buffer[ Input->DrawStartPos ] ) )
					{
						Input->DrawStartPos += 2 ;
					}
					else
					{
						Input->DrawStartPos ++ ;
					}
				}
			}
		}
	}

	// カーソルの描画
	{
		DrawObtainsString_WCHAR_T(
			x,
			y,
			AddY,
			Input->Buffer + Input->DrawStartPos,
			Input->Point - Input->DrawStartPos,
			0,
			0,
			FontHandle,
			0,
			0,
			0,
			0,
			0,
			FALSE,
			&PointX,
			&PointY
		) ;

		if( CharBuf.CBrinkDrawFlag == TRUE && InputHandle == CharBuf.ActiveInputHandle )
	//		|| ( InputHandle == CharBuf.ActiveInputHandle && CharBuf.IMESwitch == TRUE ) )
		{
			NS_DrawObtainsBox( PointX , PointY , PointX + 2 , PointY + FontSize , FontSize + FontSize / 10 * 3 , CharBuf.NormalCursorColor , TRUE ) ;
		}
	}

	// 文字列の描画
	DrawObtainsString_WCHAR_T(
		x,
		y,
		AddY,
		Input->Buffer + Input->DrawStartPos,
		-1,
		CharBuf.NormalStrColor,
		CharBuf.NormalStrEdgeColor,
		FontHandle,
		CharBuf.SelectStrBackColor,
		CharBuf.SelectStrColor,
		CharBuf.SelectStrEdgeColor,
		Input->SelectStart - Input->DrawStartPos,
		Input->SelectEnd   - Input->DrawStartPos
	) ;

	// 描画範囲を変更していた場合は元に戻す
	if( Input->DrawAreaValidFlag )
	{
		NS_SetDrawArea( DrawRectBackup.left, DrawRectBackup.top, DrawRectBackup.right, DrawRectBackup.bottom ) ;
	}

	// 入力中文字列の描画
	if( InputHandle == CharBuf.ActiveInputHandle )
	{
		NS_DrawIMEInputString( PointX , PointY , 5 , DrawCandidateList ) ; 
	}

	// データを元に戻す
	NS_SetUse3DFlag( Use3DFlag ) ;

	// 終了
	return 0 ;
}

// キー入力時のカーソルの点滅する早さをセットする
extern int NS_SetKeyInputCursorBrinkTime( int Time )
{
	CharBuf.CBrinkWait = Time ;

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;

	// 終了
	return 0 ;
}

// キー入力データの指定の領域を選択状態にする( SelectStart と SelectEnd に -1 を指定すると選択状態が解除されます )
extern int NS_SetKeyInputSelectArea( int SelectStart, int SelectEnd, int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	if( SelectStart == -1 && SelectEnd == -1 )
	{
		Input->SelectStart = -1 ;
		Input->SelectEnd   = -1 ;
	}
	else
	{
		if( SelectStart < 0                ) SelectStart = 0 ;
		if( SelectStart > Input->StrLength ) SelectStart = Input->StrLength ;
		if( SelectEnd   < 0                ) SelectEnd   = 0 ;
		if( SelectEnd   > Input->StrLength ) SelectEnd   = Input->StrLength ;

		Input->SelectStart = SelectStart ;
		Input->SelectEnd   = SelectEnd ;
	}

	// 終了
	return 0 ;
}

// キー入力データの選択領域を取得する
extern int NS_GetKeyInputSelectArea( int *SelectStart, int *SelectEnd, int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	if( SelectStart ) *SelectStart = Input->SelectStart ;
	if( SelectEnd   ) *SelectEnd   = Input->SelectEnd ;

	// 終了
	return 0 ;
}

// キー入力の描画開始文字位置を取得する
extern int NS_GetKeyInputDrawStartPos( int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	return Input->DrawStartPos ;
}

// キー入力の描画開始文字位置を設定する
extern int NS_SetKeyInputDrawStartPos( int DrawStartPos, int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	if( DrawStartPos < 0                ) DrawStartPos = 0 ;
	if( DrawStartPos > Input->StrLength ) DrawStartPos = Input->StrLength ;

	Input->DrawStartPos = DrawStartPos ;

	return Input->DrawStartPos ;
}

// キー入力時のカーソルを点滅させるかどうかをセットする
extern int NS_SetKeyInputCursorBrinkFlag( int Flag )
{
	CharBuf.CBrinkFlag = Flag ;

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;
	
	// 終了
	return 0 ;
}

// キー入力時のカーソル点滅処理のカウンタをリセット
static int ResetKeyInputCursorBrinkCount( void )
{
	CharBuf.CBrinkCount = NS_GetNowCount() ;
	CharBuf.CBrinkDrawFlag = TRUE ;
	
	// 終了
	return 0 ;
}
	


// キー入力データに指定の文字列をセットする
extern int NS_SetKeyInputString( const TCHAR *String , int InputHandle )
{
#ifdef UNICODE
	return SetKeyInputString_WCHAR_T( String, InputHandle ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = SetKeyInputString_WCHAR_T( UseStringBuffer, InputHandle ) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// キー入力ハンドルに指定の文字列をセットする
extern int NS_SetKeyInputStringWithStrLen( const TCHAR *String, size_t StringLength, int InputHandle )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = SetKeyInputString_WCHAR_T( UseStringBuffer, InputHandle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = SetKeyInputString_WCHAR_T( UseStringBuffer, InputHandle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#endif
	return Result ;
}

// キー入力データに指定の文字列をセットする
extern int SetKeyInputString_WCHAR_T( const wchar_t *String , int InputHandle )
{
	INPUTDATA * Input ;
	int StrLen ;

	if( KEYHCHK( InputHandle, Input ) )
	{
		return -1 ;
	}

	StrLen = ( int )_WCSLEN( String ) ;
	if( StrLen > Input->MaxStrLength )
	{
		StrLen = Input->MaxStrLength ;
	}

	_WCSNCPY_S( Input->Buffer, Input->MaxStrLength * sizeof( wchar_t ), String, StrLen ) ;
	Input->Buffer[ StrLen ]	= L'\0' ;
	Input->StrLength		= StrLen ;
	Input->Point			= StrLen ;
	Input->SelectEnd		= -1 ;
	Input->SelectStart		= -1 ;
	if( Input->StrLength < Input->DrawStartPos )
	{
		Input->DrawStartPos = Input->StrLength ;
	}

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;
	
	// 終了
	return 0 ;
}

// キー入力データに指定の数値を文字に置き換えてセットする
extern int NS_SetKeyInputNumber( int Number , int InputHandle )
{
	INPUTDATA * Input ;
	wchar_t StrBuf[ 512 ] ;
	int StrLen ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 文字に置き換える
	_ITOAW_S( Number, StrBuf, sizeof( StrBuf ), 10 ) ;

	StrLen = ( int )_WCSLEN( StrBuf ) ;
	if( StrLen > Input->MaxStrLength )
	{
		StrLen = Input->MaxStrLength ;
	}

	_WCSNCPY_S( Input->Buffer, Input->MaxStrLength * sizeof( wchar_t ), StrBuf ,StrLen ) ;
	Input->Buffer[ StrLen ] = L'\0' ;
	Input->StrLength		= StrLen ;
	Input->Point			= StrLen ;
	Input->SelectEnd		= -1 ;
	Input->SelectStart		= -1 ;
	if( Input->StrLength < Input->DrawStartPos )
	{
		Input->DrawStartPos = Input->StrLength ;
	}

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;

	// 終了
	return 0 ;
}

// キー入力データに指定の浮動小数点値を文字に置き換えてセットする
extern int NS_SetKeyInputNumberToFloat( float Number, int InputHandle )
{
	INPUTDATA * Input ;
	wchar_t StrBuf[ 256 ] ;
	int StrLen ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 文字に置き換える
	_SWNPRINTF( StrBuf, sizeof( StrBuf ) / 2, L"%f", Number ) ;

	StrLen = ( int )_WCSLEN( StrBuf ) ;
	if( StrLen > Input->MaxStrLength )
	{
		StrLen = Input->MaxStrLength ;
	}

	_WCSNCPY_S( Input->Buffer, Input->MaxStrLength * sizeof( wchar_t ), StrBuf, StrLen ) ;
	Input->Buffer[ StrLen ] = L'\0' ;
	Input->StrLength		= StrLen ;
	Input->Point			= StrLen ;
	Input->SelectEnd		= -1 ;
	Input->SelectStart		= -1 ;
	if( Input->StrLength < Input->DrawStartPos )
	{
		Input->DrawStartPos = Input->StrLength ;
	}

	// カーソル点滅処理のカウンタをリセット
	ResetKeyInputCursorBrinkCount() ;

	// 終了
	return 0 ;
}

// 入力中の文字列データを取得する
extern int NS_GetKeyInputString( TCHAR *StrBuffer , int InputHandle )
{
#ifdef UNICODE
	return GetKeyInputString_WCHAR_T( StrBuffer, InputHandle ) ;
#else
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	ConvString( ( const char * )Input->Buffer, -1, WCHAR_T_CHARCODEFORMAT, StrBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;

	// 終了
	return 0 ;
#endif
}

// 入力中の文字列データを取得する
extern int GetKeyInputString_WCHAR_T( wchar_t *StrBuffer , int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	_WCSCPY( StrBuffer , Input->Buffer ) ;

	// 終了
	return 0 ;
}

// 入力データの文字列を整数値として取得する
extern int NS_GetKeyInputNumber( int InputHandle )
{
	INPUTDATA * Input ;
	int Number ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 数値に変換
	if( _WCSCHR( Input->Buffer, L'.' ) )
	{
		Number = _FTOL( ( float )_ATOFW( Input->Buffer ) ) ;
	}
	else
	{
		Number = _ATOIW( Input->Buffer ) ;
	}

	// 終了
	return Number ;
}

// 入力データの文字列を浮動小数点値として取得する
extern float NS_GetKeyInputNumberToFloat( int InputHandle )
{
	INPUTDATA * Input ;
	float Number ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 数値に変換
	if( _WCSCHR( Input->Buffer, L'.' ) )
	{
		Number = ( float )_ATOFW( Input->Buffer ) ;
	}
	else
	{
		Number = ( float )_ATOIW( Input->Buffer ) ;
	}

	// 終了
	return Number ;
}

// キー入力の現在のカーソル位置を設定する
extern int NS_SetKeyInputCursorPosition( int Position, int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	if( Position < 0                ) Position = 0 ;
	if( Position > Input->StrLength ) Position = Input->StrLength ;

	Input->Point = Position ;

	// 終了
	return 0 ;
}

// キー入力の現在のカーソル位置を取得する
extern int NS_GetKeyInputCursorPosition( int InputHandle )
{
	INPUTDATA * Input ;

	if( KEYHCHK( InputHandle, Input ) ) return -1 ;

	// 終了
	return Input->Point ;
}

// ＩＭＥで入力中の文字列の情報を取得する
extern const IMEINPUTDATA *NS_GetIMEInputData( void )
{
	LPIMEINPUTDATA IMEInput ;
	int TotalSize, i, Len ;
	LPIMEINPUTCLAUSEDATA IMEClause ;

	// 既に確保されていたら解放
	if( CharBuf.IMEInputData )
	{
		DXFREE( CharBuf.IMEInputData ) ;
		CharBuf.IMEInputData = NULL ;
	}

	if( CharBuf.IMEUseFlag_OSSet == FALSE || CharBuf.IMESwitch == FALSE /* || CharBuf.ClauseNum == 0 */ )
	{
		return NULL ;
	}

	if( CharBuf.InputString[ 0 ] == 0 )
	{
		return NULL ;
	}

	// 確保すべきメモリ容量を計算
	Len = ( int )_WCSLEN( CharBuf.InputString ) ;
	TotalSize = sizeof( IMEINPUTDATA ) ;
	TotalSize += ( Len + 1 ) * sizeof( TCHAR ) * 8 ;
	if( CharBuf.ClauseNum > 1 )
	{
		TotalSize += ( CharBuf.ClauseNum - 1 ) * sizeof( IMEINPUTCLAUSEDATA ) ;
	}
	if( CharBuf.CandidateList != NULL && CharBuf.CandidateList->dwCount > 0 )
	{
		TotalSize += CharBuf.CandidateList->dwCount * sizeof( wchar_t ** ) + CharBuf.CandidateListSize * 8 ;
	}

	// メモリの確保
	IMEInput = ( LPIMEINPUTDATA )DXALLOC( ( size_t )TotalSize ) ;
	if( IMEInput == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x29\xff\x2d\xff\x25\xff\x65\x51\x9b\x52\xc5\x60\x31\x58\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＩＭＥ入力情報用のメモリの確保に失敗しました\n" @*/ ) ;
		return NULL ;
	}
	CharBuf.IMEInputData = IMEInput ;

	// 情報のセット
	{
		const void *NextP ;
		int TLen ;

		// 文字列情報のセット
		IMEInput->InputString = ( TCHAR * )( IMEInput + 1 ) ;
		ConvString( ( const char * )CharBuf.InputString, -1, WCHAR_T_CHARCODEFORMAT, ( char * )IMEInput->InputString, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;
		TLen = ( int )_TSTRLEN( IMEInput->InputString ) ;

		// カーソル位置のセット
		IMEInput->CursorPosition = NS_GetStringPoint( IMEInput->InputString , CharBuf.InputPoint ) ;

		// 分節情報のセット
		if( CharBuf.ClauseNum > 1 )
		{
			IMEInput->ClauseNum		= CharBuf.ClauseNum - 1 ;
			IMEClause				= ( LPIMEINPUTCLAUSEDATA )( IMEInput->InputString + TLen + 1 ) ;
			IMEInput->ClauseData	= IMEClause ;
			IMEInput->SelectClause	= -1 ;
			for( i = 0 ; i < IMEInput->ClauseNum ; i ++ , IMEClause ++ )
			{
#ifdef UNICODE
				IMEClause->Position = CharBuf.ClauseData[ i ] ;
				IMEClause->Length   = CharBuf.ClauseData[ i + 1 ] - IMEClause->Position ;
#else
				int CharPosition0 ;
				int CharPosition1 ;
				int CharBytePosition0 ;
				int CharBytePosition1 ;

				CharPosition0 = GetStringPoint2_WCHAR_T( CharBuf.InputString, CharBuf.ClauseData[ i     ] ) ;
				CharPosition1 = GetStringPoint2_WCHAR_T( CharBuf.InputString, CharBuf.ClauseData[ i + 1 ] ) ;

				CharBytePosition0 = NS_GetStringPoint( IMEInput->InputString, CharPosition0 ) * sizeof( char ) ;
				CharBytePosition1 = NS_GetStringPoint( IMEInput->InputString, CharPosition1 ) * sizeof( char ) ;

				IMEClause->Position = CharBytePosition0 ;
				IMEClause->Length   = CharBytePosition1 - IMEClause->Position ;
#endif

				if( IMEClause->Position == IMEInput->CursorPosition )
				{
					IMEInput->SelectClause = i ;
				}
			}

			NextP = IMEInput->ClauseData + IMEInput->ClauseNum ;
		}
		else
		{
			IMEInput->ClauseNum		= 0 ;
			IMEInput->ClauseData	= NULL ;
			IMEInput->SelectClause	= -1 ;

			NextP = IMEInput->InputString + TLen + 1 ;
		}

		// 変換候補情報のセット
		if( CharBuf.CandidateList != NULL && CharBuf.CandidateList->dwCount > 0 )
		{
			TCHAR **CandidateList ;

			CandidateList			= ( TCHAR ** )NextP ;
			IMEInput->CandidateList = ( const TCHAR ** )CandidateList ;
			IMEInput->CandidateNum  = ( int )CharBuf.CandidateList->dwCount ;
#ifdef UNICODE
			PCANDIDATELIST CandidateListBase ;
			CandidateListBase       = ( PCANDIDATELIST )( IMEInput->CandidateList + IMEInput->CandidateNum ) ;
			_MEMCPY( CandidateListBase, CharBuf.CandidateList, ( size_t )CharBuf.CandidateListSize ) ;
			for( i = 0 ; i < IMEInput->CandidateNum ; i ++ )
			{
				CandidateList[ i ] = ( wchar_t * )( ( BYTE * )CandidateListBase + CandidateListBase->dwOffset[ i ] ) ;
			}
#else
			wchar_t *CandidateListWCharString ;
			TCHAR   *TCharBuffer ;
			int     StringBytes ;

			TCharBuffer = ( TCHAR * )( IMEInput->CandidateList + IMEInput->CandidateNum ) ;
			for( i = 0 ; i < IMEInput->CandidateNum ; i ++ )
			{
				CandidateListWCharString = ( wchar_t * )( ( BYTE * )CharBuf.CandidateList + CharBuf.CandidateList->dwOffset[ i ] ) ;
				StringBytes = ConvString( ( const char * )CandidateListWCharString, -1, WCHAR_T_CHARCODEFORMAT, TCharBuffer, BUFFERBYTES_CANCEL, _TCHARCODEFORMAT ) ;
				CandidateList[ i ] = TCharBuffer ;
				TCharBuffer += StringBytes / sizeof( char ) ;
			}
#endif
			IMEInput->SelectCandidate = ( int )CharBuf.CandidateList->dwSelection ;
		}
		else
		{
			IMEInput->CandidateList   = NULL ;
			IMEInput->CandidateNum    = 0 ;
			IMEInput->SelectCandidate = 0 ;
		}

		// 文字列変換中かどうかをセット
		if( CharBuf.CharAttrNum == 0 )
		{
			IMEInput->ConvertFlag = FALSE ;
		}
		else
		{
			for( i = 0 ; i < CharBuf.CharAttrNum && CharBuf.CharAttr[ i ] != ATTR_INPUT ; i ++ ){}
			IMEInput->ConvertFlag = i == CharBuf.CharAttrNum ? TRUE : FALSE ;
		}
	}

	// 作成した情報を返す
	return CharBuf.IMEInputData ;
}

/*
// IMEで文字列を変換中の場合に変換候補の中で選択する候補を変更する
extern int NS_SetIMESelectCandidate( int CandidateIndex )
{
	HIMC Imc ;

	// ＩＭＥが使われていないときは -1 を返す
	if( CharBuf.IMESwitch == FALSE )
	{
		return -1 ;
	}

	// 文字列変換中では無い場合は -1 を返す
	if( CharBuf.CandidateList == NULL )
	{
		return -1 ;
	}

	// 候補番号が不正な場合は -1 を返す
	if( CandidateIndex < 0 || ( DWORD )CandidateIndex >= CharBuf.CandidateList->dwCount )
	{
		return -1 ;
	}

	// ＩＭＣの取得
	Imc = WinAPIData.ImmFunc.ImmGetContextFunc( WinData.MainWindow ) ;
	if( Imc == NULL )
	{
		return -1 ;
	}

	// 候補の変更
	WinAPIData.ImmFunc.ImmNotifyIMEFunc( Imc , NI_SELECTCANDIDATESTR, 0, CandidateIndex );

	// ＩＭＣを閉じる
	WinAPIData.ImmFunc.ImmReleaseContextFunc( WinData.MainWindow, Imc );

	// 終了
	return 0 ;
}
*/

#endif

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_INPUTSTRING

