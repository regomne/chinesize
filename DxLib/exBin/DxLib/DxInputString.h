// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		文字列入力プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXINPUTSTRING_H__
#define __DXINPUTSTRING_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_INPUTSTRING

// インクルード ------------------------------------------------------------------
#include "DxLib.h"

#ifndef DX_NON_KEYEX
#include "Windows/DxDirectX.h"
#include "Windows/DxGuid.h"
#endif // DX_NON_KEYEX

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define CHARBUFFER_SIZE				(1024)				// 文字コードバッファ容量

// 構造体定義 --------------------------------------------------------------------

#ifndef DX_NON_KEYEX

// クラス定義 --------------------------------------------------------------------

// TSF処理用クラス
//class DX_UIElementSink : public D_ITfUIElementSink, public D_ITfInputProcessorProfileActivationSink, public D_ITfCompartmentEventSink
class DX_UIElementSink : public D_ITfUIElementSink
{
public:
	DX_UIElementSink();
	~DX_UIElementSink();

	// IUnknown
	HRESULT __stdcall QueryInterface	( REFIID riid, void **ppvObj ) ;
	ULONG	__stdcall AddRef			( void ) ;
	ULONG	__stdcall Release			( void ) ;

	// ITfUIElementSink
	//   Notifications for Reading Window events. We could process candidate as well, but we'll use IMM for simplicity sake.
	HRESULT	__stdcall BeginUIElement	( DWORD dwUIElementId, BOOL *pbShow ) ;
	HRESULT	__stdcall UpdateUIElement	( DWORD dwUIElementId ) ;
	HRESULT	__stdcall EndUIElement		( DWORD dwUIElementId ) ;

//	// ITfInputProcessorProfileActivationSink
//	//   Notification for keyboard input locale change
//	HRESULT __stdcall OnActivated		( DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile, HKL hkl, DWORD dwFlags ) ;
//
//	// ITfCompartmentEventSink
//	//    Notification for open mode (toggle state) change
//	HRESULT __stdcall OnChange			( REFGUID rguid ) ;

private:
	LONG RefCount ;
} ;

// 文字列入力中データ構造体
struct INPUTDATA
{
	int						UseFlag ;							// 使用中かフラグ

	int						ID ;								// エラーチェックに使用するＩＤ

	int						EndFlag ;							// 入力が終了しているかフラグ
	int						CancellFlag ;						// 入力がキャンセルされたかのフラグ

//	RECT					DrawRect ;							// 描画領域

	int						StrLength ;							// 入力中文字列の長さ
	int						MaxStrLength ;						// 文字列の最大長( Unicode版の場合は wchar_t の数、マルチバイト文字列版の場合は char の数 )
	int						SelectStart ;						// 選択範囲開始位置
	int						SelectEnd ;							// 選択範囲終了位置
	int						DrawStartPos ;						// 描画を開始する位置
	int						Point ;								// カーソルの位置
	wchar_t *				Buffer ;							// 実行中に編集する文字列バッファ
	char *					TempBuffer ;						// Buffer をマルチバイト文字列にしたものを格納するためのバッファ
	size_t					TempBufferBytes ;					// TempBuffer のサイズ
//	char					*DestBuffer ;						// 入力終了時に転送する文字列バッファへのポインタ

	int						CancelValidFlag ;					// キャンセル有効フラグ
	int						SingleCharOnlyFlag ;				// ダブルバイト文字は扱わないかフラグ
	int						NumCharOnlyFlag ;					// 半角英数字しか使わないかフラグ
	int						DoubleCharOnlyFlag ;				// ダブルバイト文字しか扱わないかフラグ
	int						EnableNewLineFlag ;					// 改行を許可するかフラグ

	int						DrawAreaValidFlag ;					// 描画可能範囲が有効かどうかのフラグ
	RECT					DrawArea ;							// 描画可能範囲
} ;

#endif // DX_NON_KEYEX

// 文字コードバッファ構造体
struct CHARBUFFER
{
#ifndef UNICODE
	char					TempStock[ 16 ] ;					// ２バイト文字が StockInputChar に渡された場合に１バイト目だけでは wchar_t に変換できないので、２バイト目が渡されるまでの一時保存用変数
	int						TempStockNum ;						// TempStock に代入されている有効なバイト数

	char					TempGetStock[ 16 ] ;				// ２バイト文字が GetInputChar で取得されるときに wchar_t から char に変換した後、一度の GetInputChar では戻り値として渡せないので、wchar_t から char に変換した文字列を一時的に保存するための変数
	int						TempGetStockNum ;					// TempGetStock に代入されている有効なバイト数
#endif // UNICODE

	wchar_t					CharBuffer[ CHARBUFFER_SIZE + 1 ] ;	// 入力された文字列
	int						StPoint , EdPoint ;					// リングバッファ用ポインタ

	wchar_t					SecondString[CHARBUFFER_SIZE + 1 ] ;
	int						IMEInputFlag ;						// 入力処理中か、フラグ
	int						InputPoint ;						// 入力中の編集文字列上の位置
	wchar_t					InputString[ CHARBUFFER_SIZE + 1 ] ;// ＩＭＥによる入力中の文字列
	wchar_t					InputTempString[ CHARBUFFER_SIZE + 1 ] ;// ＩＭＥによる入力中の文字列のテンポラリバッファ
	PCANDIDATELIST			CandidateList ;						// 変換候補のリストデータ
	int						CandidateListSize ;					// 変換候補のリストデータに必要なデータ容量
	int						IMEUseFlag ;						// ＩＭＥの使用状態
	int						IMEUseFlag_System ;					// ＩＭＥの使用状態( ＤＸライブラリ内部用 )
	int						IMEUseFlag_OSSet ;					// ＯＳに対して設定しているＩＭＥの使用状態
	int						IMESwitch ;							// ＩＭＥの起動状態
	int						IMEInputStringMaxLengthIMESync ;	// ＩＭＥで入力できる最大文字数を MakeKeyInput の最大数に同期させるかどうか( TRUE:同期させる  FALSE:させない )
	int						IMEInputMaxLength ;					// ＩＭＥで一度に入力できる文字数( 0:特に制限なし  1以上:文字数制限 )
	int						IMERefreshStep ;					// ＩＭＥの入力文字列リフレッシュ用ステップ変数
	IMEINPUTDATA			*IMEInputData ;						// ＩＭＥの入力中の情報( ユーザー用 )
	int						TSFNotUseFlag ;						// ＩＭＥの変換候補表示に TSF を使用しないかどうかのフラグ

	int						IMEProcessNum ;						// ＩＭＥの文字取得プロセス番号
	int						ClauseData[ 1024 ] ;				// 文節データ
	int						ClauseNum ;							// 文節数
	BYTE					CharAttr[ 1024 ] ;					// 各文字の属性情報
	int						CharAttrNum ;						// 各文字の属性情報の要素数
	int						ChangeFlag ;						// ＩＭＥ入力に変化があったかフラグ

	DWORD					NormalStrColor ;					// 入力文字列の色
	DWORD					NormalStrEdgeColor ;				// 入力文字列の縁の色
	DWORD					NormalCursorColor ;					// ＩＭＥ未使用時のカーソルの色
	DWORD					SelectStrColor ;					// 選択された入力文字列の色
	DWORD					SelectStrEdgeColor ;				// 選択された入力文字列の縁の色
	DWORD					SelectStrBackColor ;				// 選択された入力文字列の背景の色
	DWORD					IMEStrColor ;						// ＩＭＥ使用時の入力文字列の色
	DWORD					IMEStrEdgeColorEnable ;				// IMEStrEdgeColor が有効かどうか( TRUE:有効  FALSE:無効 )
	DWORD					IMEStrEdgeColor ;					// ＩＭＥ使用時の入力文字列の縁の色
	DWORD					IMEStrBackColor ;					// ＩＭＥ使用時の入力文字列の周りの色
	DWORD					IMECursorColor ;					// ＩＭＥ使用時のカーソルの色
	DWORD					IMELineColor ;						// ＩＭＥ使用時の変換文字列の下線
	DWORD					IMESelectStrColor ;					// ＩＭＥ使用時の選択対象の変換候補文字列の色
	DWORD					IMESelectStrEdgeColorEnable ;		// IMESelectStrEdgeColor が有効かどうか( TRUE:有効  FALSE:無効 )
	DWORD					IMESelectStrEdgeColor ;				// ＩＭＥ使用時の選択対象の変換候補文字列の縁の色
	DWORD					IMESelectStrBackColor ;				// ＩＭＥ使用時の選択対象の変換候補文字列の周りの色
	DWORD					IMEConvWinStrColor ;				// ＩＭＥ使用時の変換候補ウインドウ内の文字列の色
	DWORD					IMEConvWinStrEdgeColor ;			// ＩＭＥ使用時の変換候補ウインドウ内の文字列の縁の色
	DWORD					IMEConvWinSelectStrColor ;			// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の色
	DWORD					IMEConvWinSelectStrEdgeColor ;		// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の縁の色
	DWORD					IMEConvWinSelectStrBackColorEnable ;// IMEConvWinSelectStrBackColor が有効かどうか( TRUE:有効  FALSE:無効 )
	DWORD					IMEConvWinSelectStrBackColor ;		// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の周りの色
	DWORD					IMEConvWinEdgeColor ;				// ＩＭＥ使用時の変換候補ウインドウの縁の色
	DWORD					IMEConvWinBackColor ;				// ＩＭＥ使用時の変換候補ウインドウの下地の色
	DWORD					IMEModeStrColor ;					// ＩＭＥ使用時の入力モード文字列の色(「全角ひらがな」等)
	DWORD					IMEModeStrEdgeColor ;				// ＩＭＥ使用時の入力モード文字列の縁の色

	int						EndCharaMode ;						// 文字列入力処理の入力文字数が限界に達している状態で、文字列の末端部分で入力が行われた場合の処理モード( DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE 等 )
	
	int						CBrinkFlag ;						// カーソルを点滅させるか、フラグ
	int						CBrinkCount ;						// カーソルを点滅させる場合のカウンタ
	int						CBrinkWait ;						// カーソルの点滅の速度
	int						CBrinkDrawFlag ;					// カーソルを描画するかどうかのフラグ

	int						ActiveInputHandle ;					// 入力がアクティブになっている入力ハンドル
	int						UseFontHandle ;						// 文字列描画に使用するフォントのハンドル(-1でデフォルトのハンドル)
#ifndef DX_NON_KEYEX
	INPUTDATA				InputData[ MAX_INPUT_NUM ] ;		// インプットデータ
	int						HandleID ;							// ハンドルに割り当てるＩＤ

	D_ITfThreadMgrEx		*ITfThreadMgrEx ;
	DX_UIElementSink		*TsfSink ;
	int						TsfCandidateRefCount ;
	DWORD					TsfElementSinkCookie ;
#endif
} ;

// 入力モード構造体
struct INPUTMODE
{
	DWORD					InputState ;						// 入力モード値
	wchar_t					InputName[ 16 ] ;					// 入力モードネーム
} ;

// テーブル-----------------------------------------------------------------------

// 仮想キーコードをコントロール文字コードに変換するためのテーブル
extern char CtrlCode[ 10 ][ 2 ] ;

// 内部大域変数宣言 --------------------------------------------------------------

// 文字列入力データ
extern CHARBUFFER CharBuf ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 文字コードバッファ操作関係
extern	int			InitializeInputCharBuf( void ) ;										// 文字コードバッファ関係の初期化
extern	int			TerminateInputCharBuf( void ) ;											// 文字コードバッファ関係の後始末
#ifndef DX_NON_KEYEX
extern	LRESULT		IMEProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;			// ＩＭＥメッセージのコールバック関数
extern	void		RefreshIMEFlag( int Always = FALSE ) ;										// ＩＭＥを使用するかどうかの状態を更新する
#endif


// wchar_t版関数
#ifndef DX_NON_INPUTSTRING
extern	int			StockInputChar_WCHAR_T(		wchar_t CharCode ) ;
extern	wchar_t		GetInputChar_WCHAR_T(		int DeleteFlag ) ;
extern	wchar_t		GetInputCharWait_WCHAR_T(	int DeleteFlag ) ;
extern	int			GetOneChar_WCHAR_T(			wchar_t *CharBuffer, int DeleteFlag ) ;
extern	int			GetOneCharWait_WCHAR_T(		wchar_t *CharBuffer, int DeleteFlag ) ;
extern	int			GetCtrlCodeCmp_WCHAR_T(		wchar_t Char ) ;
#endif // DX_NON_INPUTSTRING

extern	int			GetStringPoint_WCHAR_T(				const wchar_t *String, int Point ) ;
extern	int			GetStringPoint2_WCHAR_T(			const wchar_t *String, int Point ) ;
extern	int			GetStringLength_WCHAR_T(			const wchar_t *String ) ;

#ifndef DX_NON_FONT
extern	int			DrawObtainsString_WCHAR_T(			int x, int y, int AddY, const wchar_t *String, int StrLen, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 , int DrawFlag = TRUE, int *PosX = NULL, int *PosY = NULL ) ;
extern	int			DrawObtainsString_CharClip_WCHAR_T(	int x, int y, int AddY, const wchar_t *String, int StrLen, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 , int DrawFlag = TRUE, int *PosX = NULL, int *PosY = NULL ) ;
#endif // DX_NON_FONT

#ifndef DX_NON_KEYEX
extern	int			InputStringToCustom_WCHAR_T(		int x, int y, size_t BufLength, wchar_t *StrBuffer, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag = FALSE , int EnableNewLineFlag = FALSE , int DisplayCandidateList = TRUE ) ;
extern	int			KeyInputString_WCHAR_T(				int x, int y, size_t CharMaxLength, wchar_t *StrBuffer, int CancelValidFlag ) ;
extern	int			KeyInputSingleCharString_WCHAR_T(	int x, int y, size_t CharMaxLength, wchar_t *StrBuffer, int CancelValidFlag ) ;
extern	int			GetIMEInputModeStr_WCHAR_T(			wchar_t *GetBuffer ) ;
extern	int			SetKeyInputString_WCHAR_T(			const wchar_t *String, int InputHandle ) ;
extern	int			GetKeyInputString_WCHAR_T(			wchar_t *StrBuffer,    int InputHandle ) ;
#endif // DX_NON_KEYEX



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_INPUTSTRING

#endif // __DXINPUTSTRING_H__
