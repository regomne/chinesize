// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＤｉｒｅｃｔＩｎｐｕｔ制御プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxInput.h"

#ifndef DX_NON_INPUT

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 定数定義 ----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 入力システム用データ
INPUTSYSTEMDATA InputSysData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

static	int UpdateJoypadInputState( int padno ) ;								// パッドの入力状態の更新

// プログラム --------------------------------------------------------------------

// 入力システムの初期化、終了関数

// 入力システムを初期化する
extern int InitializeInputSystem( void )
{
	// 既に初期化されていたら何もせずに終了
	if( InputSysData.InitializeFlag )
	{
		return 0 ;
	}

	// 初期化完了フラグを立てる
	InputSysData.InitializeFlag = TRUE ;

	// 環境依存処理
	if( InitializeInputSystem_PF_Timing0() != 0 )
	{
		// 初期化完了フラグを倒す
		InputSysData.InitializeFlag = FALSE ;

		return -1 ;
	}

	// POV を入力無しの状態にしておく
	{
		int i ;
		int j ;

		for( i = 0 ; i < MAX_JOYPAD_NUM ; i ++ )
		{
			for( j = 0 ; j < 4 ; j ++ )
			{
				InputSysData.Pad[ i ].State.POV[ j ] = 0xffffffff ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

// 入力システムの使用を終了する
extern int TerminateInputSystem( void )
{
	// 既に後始末が終わっていたら何もせずに終了
	if( InputSysData.InitializeFlag == FALSE )
	{
		return 0 ;
	}

	// 環境依存処理
	if( TerminateInputSystem_PF_Timing0() != 0 )
	{
		return -1 ;
	}

	// 初期化完了フラグを倒す
	InputSysData.InitializeFlag = FALSE ;

	// 正常終了
	return 0 ;
}

// ジョイパッドのセットアップを行う
extern int SetupJoypad( void )
{
	// 初期化判定
	if( InputSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 環境依存処理
	if( SetupJoypad_PF() != 0 )
	{
		return -1 ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ジョイパッドの初期化は正常に終了しました\n" @*/ ) ;

	// 終了
	return 0 ;
}

// ジョイパッドの後始末を行う
extern int TerminateJoypad( void )
{
	// 初期化判定
	if( InputSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 環境依存処理
	if( TerminateJoypad_PF() != 0 )
	{
		return -1 ;
	}

	// 終了
	return 0 ;
}









// F10 が押されたことにするフラグを立てる
extern int SetF10Input( void )
{
	InputSysData.F10Flag = TRUE ;
//	InputSysData.F10InputTime = NS_GetNowCount() ;

	// 終了
	return 0 ;
}

// F10 が離されたことにするフラグを立てる
extern int ResetF10Input( void )
{
	InputSysData.F10Flag = FALSE ;

	// 終了
	return 0 ;
}

// F12 が押されたことにするフラグを立てる
extern int SetF12Input( void )
{
	InputSysData.F12Flag = TRUE ;
//	InputSysData.F12InputTime = NS_GetNowCount() ;

	// 終了
	return 0 ;
}

// F12 が離されたことにするフラグを立てる
extern int ResetF12Input( void )
{
	InputSysData.F12Flag = FALSE ;

	// 終了
	return 0 ;
}











// マウス関係の関数

// マウスの入力情報を保存する
extern void StockMouseInputLogData( int Button, int ClickX, int ClickY )
{
	MOUSEINPUTLOGDATA *MInput ;

	// ログの数が最大数に達していたら何もせず終了
	if( InputSysData.MouseInputLogNum >= MOUSEINPUT_LOG_NUM ) return ;

	// 情報を保存
	MInput = &InputSysData.MouseInputLog[ InputSysData.MouseInputLogNum ] ;
	MInput->ClickX = ClickX ;
	MInput->ClickY = ClickY ;
	MInput->Button = Button ;
	MInput->LogType = MOUSE_INPUT_LOG_DOWN ;

	// 情報の数を増やす
	InputSysData.MouseInputLogNum ++ ;
}

// マウスの入力情報を保存する GetMouseInputLog2 用
extern void StockMouseInputLogData2( int Button, int ClickX, int ClickY, int LogType, int IsDummy )
{
	MOUSEINPUTLOGDATA *MInput ;
	int Index = 0 ;

	// ログの数が最大数に達していたら何もせず終了
	if( InputSysData.MouseInputLog2Num >= MOUSEINPUT_LOG_NUM ) return ;

	// 直近のボタンログに記録するためのインデックスを割り出しておく
	switch( Button )
	{
	case MOUSE_INPUT_LEFT   : Index = 0 ; break ;
	case MOUSE_INPUT_RIGHT  : Index = 1 ; break ;
	case MOUSE_INPUT_MIDDLE : Index = 2 ; break ;
	}

	// ダミー情報ではない場合のみの処理を行う
	if( IsDummy == FALSE )
	{
		int i ;

		// ダミーの情報と重複する情報であった場合はダミー情報に上書きする
		for( i = InputSysData.MouseInputLog2Num - 1 ; i >= 0 ; i -- )
		{
			if( InputSysData.MouseInputLog2[ i ].Button != Button ||
				InputSysData.MouseInputLog2[ i ].LogType != LogType )
			{
				continue ;
			}

			if( InputSysData.MouseInputLog2[ i ].IsDummy == TRUE )
			{
				MInput = &InputSysData.MouseInputLog2[ InputSysData.MouseInputLog2Num ] ;
				MInput->ClickX = ClickX ;
				MInput->ClickY = ClickY ;
				MInput->Button = Button ;
				MInput->LogType = LogType ;
				MInput->IsDummy = IsDummy ;

				// 直近のマウスの入力情報も更新
				InputSysData.MouseInputLog2PrevEnable[ Index ] = TRUE ;
				InputSysData.MouseInputLog2Prev[ Index ] = *MInput ;

				return ;
			}

			break ;
		}

		// 同一のボタンで同じ LogType が連続した場合は対の操作がウィンドウ外で行われたと判断して ClickX=-1, ClickY=-1 のログを追加する
		if( InputSysData.MouseInputLog2PrevEnable[ Index ] &&
			InputSysData.MouseInputLog2Prev[ Index ].LogType == LogType )
		{
			// 情報を保存
			MInput = &InputSysData.MouseInputLog2[ InputSysData.MouseInputLog2Num ] ;
			MInput->ClickX = -100 ;
			MInput->ClickY = -100 ;
			MInput->Button = Button ;
			MInput->LogType = LogType == MOUSE_INPUT_LOG_DOWN ? MOUSE_INPUT_LOG_UP : MOUSE_INPUT_LOG_DOWN ;
			MInput->IsDummy = TRUE ;

			// 直近のマウスの入力情報も更新
			InputSysData.MouseInputLog2PrevEnable[ Index ] = TRUE ;
			InputSysData.MouseInputLog2Prev[ Index ] = *MInput ;

			// 情報の数を増やす
			InputSysData.MouseInputLog2Num ++ ;

			// ログの数が最大数に達していたらここで終了
			if( InputSysData.MouseInputLog2Num >= MOUSEINPUT_LOG_NUM )
			{
				return ;
			}
		}
	}

	// 情報を保存
	MInput = &InputSysData.MouseInputLog2[ InputSysData.MouseInputLog2Num ] ;
	MInput->ClickX = ClickX ;
	MInput->ClickY = ClickY ;
	MInput->Button = Button ;
	MInput->LogType = LogType ;
	MInput->IsDummy = IsDummy ;

	// 情報の数を増やす
	InputSysData.MouseInputLog2Num ++ ;

	// 直近のマウス入力情報を更新
	InputSysData.MouseInputLog2PrevEnable[ Index ] = TRUE ;
	InputSysData.MouseInputLog2Prev[ Index ] = *MInput ;
}

// マウス入力

// マウスのボタンを押した情報を取得する( 戻り値  -1:ログが無かった )
extern int NS_GetMouseInputLog( int *Button, int *ClickX, int *ClickY, int LogDelete )
{
	MOUSEINPUTLOGDATA *MInput ;

	// ログが無かったら -1 を返す
	if( InputSysData.MouseInputLogNum == 0 ) return -1 ;

	// 情報を返す
	MInput = &InputSysData.MouseInputLog[ 0 ] ;
	if( Button ) *Button = MInput->Button ;
	if( ClickX ) *ClickX = MInput->ClickX ;
	if( ClickY ) *ClickY = MInput->ClickY ;

	// ログを削除する指定がある場合は削除する
	if( LogDelete )
	{
		InputSysData.MouseInputLogNum -- ;
		if( InputSysData.MouseInputLogNum )
			_MEMMOVE( InputSysData.MouseInputLog, &InputSysData.MouseInputLog[ 1 ], sizeof( MOUSEINPUTLOGDATA ) * InputSysData.MouseInputLogNum ) ;
	}

	// 終了
	return 0 ;
}

// マウスのボタンを押したり離したりした情報を取得する( 戻り値  -1:ログが無かった )
extern int NS_GetMouseInputLog2( int *Button, int *ClickX, int *ClickY, int *LogType, int LogDelete )
{
	MOUSEINPUTLOGDATA *MInput ;

	// 補助処理
	{
		static int ButtonTable[ 3 ] = { MOUSE_INPUT_LEFT, MOUSE_INPUT_RIGHT, MOUSE_INPUT_MIDDLE } ;
		int NowInput = NS_GetMouseInput() ;
		int i ;

		for( i = 0 ; i < 3 ; i ++ )
		{
			// 直近のログが『ボタンを押した』で、且つ現在ボタンが離されていたらマウスのボタンを離したという情報ストックする
			if( InputSysData.MouseInputLog2PrevEnable[ i ] &&
				InputSysData.MouseInputLog2Prev[ i ].LogType == MOUSE_INPUT_LOG_DOWN &&
				( NowInput & ButtonTable[ i ] ) == 0 )
			{
				int mx, my ;
				NS_GetMousePoint( &mx, &my ) ;
				StockMouseInputLogData2( ButtonTable[ i ], mx, my, MOUSE_INPUT_LOG_UP ) ;
			}

			// 直近のログが無いか、『ボタンを離した』で、且つ現在ボタンが押されていたらマウスのボタンを押したという情報ストックする
			if( ( InputSysData.MouseInputLog2PrevEnable[ i ] == FALSE ||
				  InputSysData.MouseInputLog2Prev[ i ].LogType == MOUSE_INPUT_LOG_UP ) &&
				( NowInput & ButtonTable[ i ] ) != 0 )
			{
				int mx, my ;
				NS_GetMousePoint( &mx, &my ) ;
				StockMouseInputLogData2( ButtonTable[ i ], mx, my, MOUSE_INPUT_LOG_DOWN ) ;
			}
		}
	}

	// ログが無かったら処理を分岐
	if( InputSysData.MouseInputLog2Num == 0 )
	{
		return -1 ;
	}

	// 情報を返す
	MInput = &InputSysData.MouseInputLog2[ 0 ] ;
	if( Button  ) *Button  = MInput->Button ;
	if( ClickX  ) *ClickX  = MInput->ClickX ;
	if( ClickY  ) *ClickY  = MInput->ClickY ;
	if( LogType ) *LogType = MInput->LogType ;

	// ログを削除する指定がある場合は削除する
	if( LogDelete )
	{
		InputSysData.MouseInputLog2Num -- ;
		if( InputSysData.MouseInputLog2Num )
			_MEMMOVE( InputSysData.MouseInputLog2, &InputSysData.MouseInputLog2[ 1 ], sizeof( MOUSEINPUTLOGDATA ) * InputSysData.MouseInputLog2Num ) ;
	}

	// 終了
	return 0 ;
}

// マウスの位置を取得する
extern int NS_GetMousePoint( int *XBuf, int *YBuf )
{
//	// タッチパネルの入力がある場合はそれをマウスポインタの位置とする
//	if( InputSysData.Touch.NowData.PointNum > 0 && InputSysData.Touch.MouseInputID >= 0 )
//	{
//		if( XBuf != NULL )
//		{
//			*XBuf = InputSysData.Touch.NowData.Point[ InputSysData.Touch.MouseInputIndex ].PositionX ;
//		}
//
//		if( YBuf != NULL )
//		{
//			*YBuf = InputSysData.Touch.NowData.Point[ InputSysData.Touch.MouseInputIndex ].PositionY ;
//		}
//	}
//	else
	{
		// それ以外の場合は環境依存のマウス一取得処理を実行する
		return GetMousePoint_PF( XBuf, YBuf ) ;
	}

//	// 正常終了
//	return 0 ;
}

// マウスの位置をセットする
extern int NS_SetMousePoint( int PointX , int PointY )
{
	// 環境依存処理を実行する
	return SetMousePoint_PF( PointX, PointY ) ;
}

// マウスのボタンの状態を得る 
extern int NS_GetMouseInput( void )
{
	int MInput ;

	// 環境依存処理を実行
	MInput = GetMouseInput_PF() ;

//	// タッチパネルの入力がある場合は左クリックされていることにする
//	if( InputSysData.Touch.NowData.PointNum > 0 && InputSysData.Touch.MouseInputID >= 0 )
//	{
//		MInput |= MOUSE_INPUT_LEFT ;
//	}
	
	return MInput ;
}

// マウスホイールの回転量を得る
extern int NS_GetMouseWheelRotVol( int CounterReset )
{
	return GetMouseWheelRotVol_PF( CounterReset ) ;
}

// 水平マウスホイールの回転量を得る
extern int NS_GetMouseHWheelRotVol( int CounterReset )
{
	return GetMouseHWheelRotVol_PF( CounterReset ) ;
}

// マウスホイールの回転量を得る( 戻り値が float 型 )
extern float NS_GetMouseWheelRotVolF( int CounterReset )
{
	return GetMouseWheelRotVolF_PF( CounterReset ) ;
}

// 水平マウスホイールの回転量を得る( 戻り値が float 型 )
extern float NS_GetMouseHWheelRotVolF( int CounterReset )
{
	return GetMouseHWheelRotVolF_PF( CounterReset ) ;
}














// タッチパネル関係の関数

// 押されたタッチ情報を追加する
static void AddTouchDownInputPointLog( TOUCHINPUTPOINT *TouchPointData )
{
	int AddIndex ;

	// 情報の数が最大数を超えていたら先頭データのインデックスをずらす
	if( InputSysData.Touch.DownLogNum == INPUTTOUCH_LOG_NUM )
	{
		AddIndex = InputSysData.Touch.DownLogStartIndex ;

		InputSysData.Touch.DownLogStartIndex ++ ;
		if( InputSysData.Touch.DownLogStartIndex == INPUTTOUCH_LOG_NUM )
		{
			InputSysData.Touch.DownLogStartIndex = 0 ;
		}
	}
	else
	{
		AddIndex = InputSysData.Touch.DownLogStartIndex + InputSysData.Touch.DownLogNum ;
		if( AddIndex >= INPUTTOUCH_LOG_NUM )
		{
			AddIndex -= INPUTTOUCH_LOG_NUM ;
		}

		InputSysData.Touch.DownLogNum ++ ;
	}

	// 離されたタッチ情報をセット
	InputSysData.Touch.DownLog[ AddIndex ] = *TouchPointData ;
}

// 離されたタッチ情報を追加する
static void AddTouchUpInputPointLog( TOUCHINPUTPOINT *TouchPointData )
{
	int AddIndex ;

	// 情報の数が最大数を超えていたら先頭データのインデックスをずらす
	if( InputSysData.Touch.UpLogNum == INPUTTOUCH_LOG_NUM )
	{
		AddIndex = InputSysData.Touch.UpLogStartIndex ;

		InputSysData.Touch.UpLogStartIndex ++ ;
		if( InputSysData.Touch.UpLogStartIndex == INPUTTOUCH_LOG_NUM )
		{
			InputSysData.Touch.UpLogStartIndex = 0 ;
		}
	}
	else
	{
		AddIndex = InputSysData.Touch.UpLogStartIndex + InputSysData.Touch.UpLogNum ;
		if( AddIndex >= INPUTTOUCH_LOG_NUM )
		{
			AddIndex -= INPUTTOUCH_LOG_NUM ;
		}

		InputSysData.Touch.UpLogNum ++ ;
	}

	// 離されたタッチ情報をセット
	InputSysData.Touch.UpLog[ AddIndex ] = *TouchPointData ;
}

// タッチパネルの入力情報をログに追加する
static void AddTouchInputLog( TOUCHINPUTDATA *TouchData )
{
	int AddIndex ;

	// タッチ情報の数が最大数を超えていたら先頭データのインデックスをずらす
	if( InputSysData.Touch.LogNum == INPUTTOUCH_LOG_NUM )
	{
		AddIndex = InputSysData.Touch.LogStartIndex ;

		InputSysData.Touch.LogStartIndex ++ ;
		if( InputSysData.Touch.LogStartIndex == INPUTTOUCH_LOG_NUM )
		{
			InputSysData.Touch.LogStartIndex = 0 ;
		}
	}
	else
	{
		AddIndex = InputSysData.Touch.LogStartIndex + InputSysData.Touch.LogNum ;
		if( AddIndex >= INPUTTOUCH_LOG_NUM )
		{
			AddIndex -= INPUTTOUCH_LOG_NUM ;
		}

		InputSysData.Touch.LogNum ++ ;
	}

	// データをセット
	InputSysData.Touch.Log[ AddIndex ] = *TouchData ;
}

// タッチパネルの入力情報を追加する
extern	int AddTouchInputData( TOUCHINPUTDATA *TouchData )
{
	int i ;
	int j ;

	// 最初のタッチ情報だったら「押された」ログに追加する
	if( InputSysData.Touch.LogNum == 0 )
	{
		for( i = 0 ; i < TouchData->PointNum ; i ++ )
		{
			AddTouchUpInputPointLog( &TouchData->Point[ i ] ) ;
		}
	}
	else
	{
		// 一つ前のログでは存在しなかったタッチがあったらログに追加する
		for( i = 0 ; i < TouchData->PointNum ; i ++ )
		{
			for( j = 0 ; j < InputSysData.Touch.NowData.PointNum ; j ++ )
			{
				if( TouchData->Point[ i ].ID == InputSysData.Touch.NowData.Point[ j ].ID )
				{
					break ;
				}
			}
			if( j != InputSysData.Touch.NowData.PointNum )
			{
				continue ;
			}

			AddTouchUpInputPointLog( &TouchData->Point[ i ] ) ;
		}
	}

	// 一つ前のログ情報と比較して、離されたタッチがあったらログに追加する
	if( InputSysData.Touch.LogNum >= 1 )
	{
		for( i = 0 ; i < InputSysData.Touch.NowData.PointNum ; i ++ )
		{
			for( j = 0 ; j < TouchData->PointNum ; j ++ )
			{
				if( InputSysData.Touch.NowData.Point[ i ].ID == TouchData->Point[ j ].ID )
				{
					break ;
				}
			}
			if( j != TouchData->PointNum )
			{
				continue ;
			}

			AddTouchUpInputPointLog( &InputSysData.Touch.NowData.Point[ i ] ) ;
		}
	}

	// ログに追加する
	AddTouchInputLog( TouchData ) ;

	// 最新のタッチ情報にもセット
	InputSysData.Touch.NowData = *TouchData ;

	// マウス入力とするタッチ情報のＩＤを更新
	{
		// 一つもタッチ情報がなければ -1 を代入
		if( TouchData->PointNum == 0 )
		{
			InputSysData.Touch.MouseInputDevice = -1 ;
			InputSysData.Touch.MouseInputID = -1 ;
			InputSysData.Touch.MouseInputIndex = -1 ;
		}
		else
		{
			// 今までと同じＩＤが無かったら更新
			for( i = 0 ; i < TouchData->PointNum ; i ++ )
			{
				if( InputSysData.Touch.MouseInputDevice == ( int )TouchData->Point[ i ].Device &&
					InputSysData.Touch.MouseInputID     == ( int )TouchData->Point[ i ].ID )
				{
					InputSysData.Touch.MouseInputIndex = i ;
					break ;
				}
			}
			if( i == TouchData->PointNum )
			{
				InputSysData.Touch.MouseInputDevice = TouchData->Point[ 0 ].Device ;
				InputSysData.Touch.MouseInputID     = TouchData->Point[ 0 ].ID ;
				InputSysData.Touch.MouseInputIndex  = 0 ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

// タッチパネル入力関係関数

// タッチされている数を取得する
extern int NS_GetTouchInputNum( void )
{
	return InputSysData.Touch.NowData.PointNum ;
}

// タッチの情報を取得する
extern int NS_GetTouchInput( int InputNo, int *PositionX, int *PositionY, int *ID , int *Device )
{
	if( InputNo >= InputSysData.Touch.NowData.PointNum )
	{
		return -1 ;
	}

	if( PositionX != NULL )
	{
		*PositionX = InputSysData.Touch.NowData.Point[ InputNo ].PositionX ;
	}

	if( PositionY != NULL )
	{
		*PositionY = InputSysData.Touch.NowData.Point[ InputNo ].PositionY ;
	}

	if( ID != NULL )
	{
		*ID = InputSysData.Touch.NowData.Point[ InputNo ].ID ;
	}

	if( Device != NULL )
	{
		*Device = InputSysData.Touch.NowData.Point[ InputNo ].Device ;
	}

	return 0 ;
}

// ストックされているタッチ情報の数を取得する
extern int NS_GetTouchInputLogNum( void )
{
	return InputSysData.Touch.LogNum ;
}

// ストックされているタッチ情報をクリアする
extern int NS_ClearTouchInputLog( void )
{
	InputSysData.Touch.LogNum = 0 ;
	InputSysData.Touch.LogStartIndex = 0 ;

	return 0 ;
}

// ストックされているタッチ情報から一番古い情報をひとつ取得する
extern TOUCHINPUTDATA NS_GetTouchInputLogOne( int PeekFlag )
{
	TOUCHINPUTDATA Return ;

	if( InputSysData.Touch.LogNum == 0 )
	{
		_MEMSET( &Return, 0, sizeof( Return ) ) ;
	}
	else
	{
		Return = InputSysData.Touch.Log[ InputSysData.Touch.LogStartIndex ] ;

		if( PeekFlag == FALSE )
		{
			InputSysData.Touch.LogStartIndex ++ ;
			if( InputSysData.Touch.LogStartIndex == INPUTTOUCH_LOG_NUM )
			{
				InputSysData.Touch.LogStartIndex = 0 ;
			}

			InputSysData.Touch.LogNum -- ;
		}
	}

	return Return ;
}

// ストックされているタッチ情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )
extern int NS_GetTouchInputLog( TOUCHINPUTDATA *TouchData, int GetNum, int PeekFlag )
{
	int i ;
	int GetIndex ;

	if( InputSysData.Touch.LogNum == 0 )
	{
		return 0 ;
	}

	if( InputSysData.Touch.LogNum < GetNum )
	{
		GetNum = InputSysData.Touch.LogNum ;
	}

	GetIndex = InputSysData.Touch.LogStartIndex ;
	for( i = 0 ; i < GetNum ; i ++ )
	{
		TouchData[ i ] = InputSysData.Touch.Log[ GetIndex ] ;

		GetIndex ++ ;
		if( GetIndex == INPUTTOUCH_LOG_NUM )
		{
			GetIndex = 0 ;
		}
	}

	if( PeekFlag == FALSE )
	{
		InputSysData.Touch.LogNum -= GetNum ;
		InputSysData.Touch.LogStartIndex = GetIndex ;
	}

	return GetNum ;
}







// ストックされているタッチされ始めた情報の数を取得する
extern int NS_GetTouchInputDownLogNum( void )
{
	return InputSysData.Touch.DownLogNum ;
}

// ストックされているタッチされ始めた情報をクリアする
extern int NS_ClearTouchInputDownLog( void )
{
	InputSysData.Touch.DownLogNum = 0 ;
	InputSysData.Touch.DownLogStartIndex = 0 ;

	return 0 ;
}

// ストックされているタッチされ始めた情報から一番古い情報をひとつ取得する
extern TOUCHINPUTPOINT NS_GetTouchInputDownLogOne( int PeekFlag )
{
	TOUCHINPUTPOINT Return ;

	if( InputSysData.Touch.DownLogNum == 0 )
	{
		_MEMSET( &Return, 0, sizeof( Return ) ) ;
	}
	else
	{
		Return = InputSysData.Touch.DownLog[ InputSysData.Touch.DownLogStartIndex ] ;

		if( PeekFlag == FALSE )
		{
			InputSysData.Touch.DownLogStartIndex ++ ;
			if( InputSysData.Touch.DownLogStartIndex == INPUTTOUCH_LOG_NUM )
			{
				InputSysData.Touch.DownLogStartIndex = 0 ;
			}

			InputSysData.Touch.DownLogNum -- ;
		}
	}

	return Return ;
}

// ストックされているタッチされ始めた情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )
extern int NS_GetTouchInputDownLog( TOUCHINPUTPOINT *TouchData, int GetNum, int PeekFlag )
{
	int i ;
	int GetIndex ;

	if( InputSysData.Touch.DownLogNum == 0 )
	{
		return 0 ;
	}

	if( InputSysData.Touch.DownLogNum < GetNum )
	{
		GetNum = InputSysData.Touch.DownLogNum ;
	}

	GetIndex = InputSysData.Touch.DownLogStartIndex ;
	for( i = 0 ; i < GetNum ; i ++ )
	{
		TouchData[ i ] = InputSysData.Touch.DownLog[ GetIndex ] ;

		GetIndex ++ ;
		if( GetIndex == INPUTTOUCH_LOG_NUM )
		{
			GetIndex = 0 ;
		}
	}

	if( PeekFlag == FALSE )
	{
		InputSysData.Touch.DownLogNum -= GetNum ;
		InputSysData.Touch.DownLogStartIndex = GetIndex ;
	}

	return GetNum ;
}











// ストックされているタッチが離された情報の数を取得する
extern int NS_GetTouchInputUpLogNum( void )
{
	return InputSysData.Touch.UpLogNum ;
}

// ストックされているタッチが離された情報をクリアする
extern int NS_ClearTouchInputUpLog( void )
{
	InputSysData.Touch.UpLogNum = 0 ;
	InputSysData.Touch.UpLogStartIndex = 0 ;

	return 0 ;
}

// ストックされているタッチが離された情報から一番古い情報をひとつ取得する
extern TOUCHINPUTPOINT NS_GetTouchInputUpLogOne( int PeekFlag )
{
	TOUCHINPUTPOINT Return ;

	if( InputSysData.Touch.UpLogNum == 0 )
	{
		_MEMSET( &Return, 0, sizeof( Return ) ) ;
	}
	else
	{
		Return = InputSysData.Touch.UpLog[ InputSysData.Touch.UpLogStartIndex ] ;

		if( PeekFlag == FALSE )
		{
			InputSysData.Touch.UpLogStartIndex ++ ;
			if( InputSysData.Touch.UpLogStartIndex == INPUTTOUCH_LOG_NUM )
			{
				InputSysData.Touch.UpLogStartIndex = 0 ;
			}

			InputSysData.Touch.UpLogNum -- ;
		}
	}

	return Return ;
}

// ストックされているタッチが離された情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )
extern int NS_GetTouchInputUpLog( TOUCHINPUTPOINT *TouchData, int GetNum, int PeekFlag )
{
	int i ;
	int GetIndex ;

	if( InputSysData.Touch.UpLogNum == 0 )
	{
		return 0 ;
	}

	if( InputSysData.Touch.UpLogNum < GetNum )
	{
		GetNum = InputSysData.Touch.UpLogNum ;
	}

	GetIndex = InputSysData.Touch.UpLogStartIndex ;
	for( i = 0 ; i < GetNum ; i ++ )
	{
		TouchData[ i ] = InputSysData.Touch.UpLog[ GetIndex ] ;

		GetIndex ++ ;
		if( GetIndex == INPUTTOUCH_LOG_NUM )
		{
			GetIndex = 0 ;
		}
	}

	if( PeekFlag == FALSE )
	{
		InputSysData.Touch.UpLogNum -= GetNum ;
		InputSysData.Touch.UpLogStartIndex = GetIndex ;
	}

	return GetNum ;
}

















// 入力状態取得関数

// キーボードの状態取得
extern int NS_CheckHitKey( int KeyCode )
{
	int Result ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	// キーボードの入力状態を更新
	UpdateKeyboardInputState() ;

	// 指定のキーの状態を返す
	Result = ( InputSysData.KeyInputBuf[ KeyCode ] & 0x80 ) != 0 ;
	
	return Result ;
}

// 全キーの押下状態を取得
extern int NS_CheckHitKeyAll( int CheckType )
{
	int i ;
	unsigned char *KeyData ;

#ifdef __WINDOWS__
	if( WinData.MainWindow == NULL )
		return -1 ;
#endif // __WINDOWS__

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	// キーボードのチェック
	if( CheckType & DX_CHECKINPUT_KEY )
	{
		// キーボードの入力状態を更新
		UpdateKeyboardInputState() ;

		// どれか一つでもキーが押されているか判定
		KeyData = InputSysData.KeyInputBuf ;
		for( i = 0 ; i < 256 ; i ++ , KeyData ++ )
		{
			if( ( *KeyData & 0x80 ) && i != KEY_INPUT_KANJI && i != KEY_INPUT_CAPSLOCK && i != KEY_INPUT_KANA )
				return i ;
		}
		if( InputSysData.F10Flag )
		{
			return KEY_INPUT_F10 ;
		}
	}

	// ジョイパッドのチェック
	if( CheckType & DX_CHECKINPUT_PAD )
	{
		for( i = 0 ; i < InputSysData.PadNum ; i ++ )
		{
			if( NS_GetJoypadInputState( i + 1 ) != 0 )
				return -1 ;
		}
	}

	// マウスボタンのチェック
	if( CheckType & DX_CHECKINPUT_MOUSE )
	{
#ifdef __WINDOWS__
		int x, y ;
		RECT Rect ;

		NS_GetMousePoint( &x, &y ) ;

		NS_GetWindowCRect( &Rect ) ;
		if( x >= 0                      && y >= 0                      && NS_GetWindowActiveFlag() &&
			x <= Rect.right - Rect.left && y <= Rect.bottom - Rect.top && NS_GetMouseInput() != 0 )
			return -1 ;
#else // __WINDOWS__
		if( NS_GetMouseInput() != 0 )
		{
			return -1 ;
		}
#endif // __WINDOWS__
	}

	// 終了
	return 0 ;
}

// すべてのキーの押下状態を取得する
extern int NS_GetHitKeyStateAll( char *KeyStateBuf )
{
	int i ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	// キーボードの入力状態を更新
	UpdateKeyboardInputState() ;

	// キーの入力値を変換
	for( i = 0 ; i < 256 ; i ++ , KeyStateBuf ++ )
	{
		*KeyStateBuf = ( BYTE )( InputSysData.KeyInputBuf[i] ) >> 7 ;
	}

	// 終了
	return 0 ;
}





// ジョイパッドが接続されている数を取得する
extern int NS_GetJoypadNum( void )
{
	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	return InputSysData.PadNum ;
}

// ジョイバッドの入力状態取得
extern int NS_GetJoypadInputState( int InputType )
{
	int BackData = 0 ;
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	int iX , iY ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( ( JoypadNum < -1 || JoypadNum >= InputSysData.PadNum ) && 
		( InputSysData.PadNum != 0 && ( InputType & DX_INPUT_KEY ) == 0 ) )
	{
		return 0 ;
	}

	if( JoypadNum != -1 && InputSysData.PadNum != 0 && JoypadNum < InputSysData.PadNum )
	{
		pad = &InputSysData.Pad[ JoypadNum ] ;

		// パッドの情報を取得する
		{
			// 情報の更新
			UpdateJoypadInputState( JoypadNum ) ;

			// 入力状態を保存
			iX = pad->State.X ;
			iY = pad->State.Y ;

			// カーソルボタン判定
			     if( iX < 0 ) BackData |= PAD_INPUT_LEFT ;			// ←入力判定
			else if( iX > 0 ) BackData |= PAD_INPUT_RIGHT ;			// →入力判定

			     if( iY < 0 ) BackData |= PAD_INPUT_UP ;			// ↑入力判定
			else if( iY > 0 ) BackData |= PAD_INPUT_DOWN ;			// ↓入力判定

			// ハットスイッチの状態も方向ボタンに反映する
			switch( pad->State.POV[ 0 ] )
			{
			case 0 :		BackData |= PAD_INPUT_UP						;	break ;
			case 4500 :		BackData |= PAD_INPUT_UP    | PAD_INPUT_RIGHT	;	break ;
			case 9000 :		BackData |= PAD_INPUT_RIGHT						;	break ;
			case 13500 :	BackData |= PAD_INPUT_RIGHT | PAD_INPUT_DOWN	;	break ;
			case 18000 :	BackData |= PAD_INPUT_DOWN						;	break ;
			case 22500 :	BackData |= PAD_INPUT_DOWN  | PAD_INPUT_LEFT	;	break ;
			case 27000 :	BackData |= PAD_INPUT_LEFT						;	break ;
			case 31500 :	BackData |= PAD_INPUT_LEFT  | PAD_INPUT_UP		;	break ;
			}

			// ボタン入力判定
			{
				int i ;

				// 入力状態を保存
				for( i = 0 ; i < 24 ; i ++ )
				{
					BackData |= ( pad->State.Buttons[ i ] & 0x80 ) != 0 ? 1 << ( i + 4 ) : 0 ;
				}
			}
		}
	}

	// キー入力指定もあった場合はキーの入力状態も反映させる
	if( InputType & DX_INPUT_KEY )
	{
		BYTE *KeyBuf ;
		int *Map ;
		int i, j ;
		unsigned int pad_bit ;

		// キーボードの入力状態を更新
		UpdateKeyboardInputState() ;

		KeyBuf = InputSysData.KeyInputBuf ;
		Map = InputSysData.KeyToJoypadInput[ JoypadNum < 0 ? 0 : JoypadNum ][ 0 ] ;

		pad_bit = 1 ;
		for( i = 0 ; i < 32 ; i ++, pad_bit <<= 1, Map += 4 )
		{
			for( j = 0 ; j < 4 ; j ++ )
			{
				if( Map[ j ] > 0 && ( KeyBuf[ Map[ j ] ] & 0x80 ) )
				{
					BackData |= ( int )pad_bit ;
					break ; 
				}
			}
		}
	}

	// 終了
	return BackData ;
}

// ジョイパッドのアナログ的なスティック入力情報を得る
extern int NS_GetJoypadAnalogInput( int *XBuf , int *YBuf , int InputType )
{
	int BackData = 0 ;
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;
	
	if( XBuf != NULL ) *XBuf = 0 ;
	if( YBuf != NULL ) *YBuf = 0 ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( ( JoypadNum < -1 || JoypadNum >= InputSysData.PadNum ) && 
		( InputSysData.PadNum != 0 && ( InputType & DX_INPUT_KEY ) == 0 ) )
	{
		return 0 ;
	}

	if( JoypadNum != -1 && InputSysData.PadNum != 0 )
	{
		pad = &InputSysData.Pad[ JoypadNum ] ;

		// 入力状態の更新
		UpdateJoypadInputState( JoypadNum ) ;

		// 入力状態を保存
		if( XBuf ) *XBuf = pad->State.X ;
		if( YBuf ) *YBuf = pad->State.Y ;
	}

	// キー入力指定もあった場合はキーの入力状態も反映させる
	if( InputType & DX_INPUT_KEY )
	{
		BYTE *KeyBuf ;

		// キーボードの入力状態を更新
		UpdateKeyboardInputState() ;

		KeyBuf = InputSysData.KeyInputBuf ;
		if( XBuf )
		{
			if( KeyBuf[ KEY_INPUT_NUMPAD4 ] & 0x80 || KeyBuf[ KEY_INPUT_LEFT  ] & 0x80 ) *XBuf = -DIRINPUT_MAX ;		// ４キー←チェック
			if( KeyBuf[ KEY_INPUT_NUMPAD6 ] & 0x80 || KeyBuf[ KEY_INPUT_RIGHT ] & 0x80 ) *XBuf =  DIRINPUT_MAX	;		// ６キー→キーチェック
		}
		if( YBuf )
		{
			if( KeyBuf[ KEY_INPUT_NUMPAD2 ] & 0x80 || KeyBuf[ KEY_INPUT_DOWN  ] & 0x80 ) *YBuf =  DIRINPUT_MAX	;		// ２キー↓キーチェック
			if( KeyBuf[ KEY_INPUT_NUMPAD8 ] & 0x80 || KeyBuf[ KEY_INPUT_UP    ] & 0x80 ) *YBuf = -DIRINPUT_MAX ;		// ８キー↑キーチェック
		}
	}

	// 終了
	return BackData ;
}

// ジョイパッドのアナログ的なスティック入力情報を得る(右スティック用)
extern int NS_GetJoypadAnalogInputRight( int *XBuf, int *YBuf, int InputType )
{
	int BackData = 0 ;
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;
	
	if( XBuf != NULL ) *XBuf = 0 ;
	if( YBuf != NULL ) *YBuf = 0 ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( JoypadNum < -1 || JoypadNum >= InputSysData.PadNum )
		return 0 ;

	if( JoypadNum != -1 )
	{
		pad = &InputSysData.Pad[ JoypadNum ] ;

		// 入力状態の更新
		UpdateJoypadInputState( JoypadNum ) ;

		// 入力状態を保存( XInput かどうかで処理を分岐 )
		if( NS_CheckJoypadXInput( InputType ) )
		{
			if( XBuf ) *XBuf = pad->State.Rx ;
			if( YBuf ) *YBuf = pad->State.Ry ;
		}
		else
		{
			if( XBuf ) *XBuf = pad->State.Z ;
			if( YBuf ) *YBuf = pad->State.Rz ;
		}
	}

	// 終了
	return BackData ;
}

// ジョイパッドから DirectInput から得られる生のデータを取得する( DX_CHECKINPUT_KEY や DX_INPUT_KEY_PAD1 を引数に渡すとエラー )
extern	int	NS_GetJoypadDirectInputState( int InputType, DINPUT_JOYSTATE *DInputState )
{
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		_MEMSET( DInputState, 0, sizeof( DINPUT_JOYSTATE ) ) ;
		DInputState->POV[ 0 ] = 0xffffffff ;
		DInputState->POV[ 1 ] = 0xffffffff ;
		DInputState->POV[ 2 ] = 0xffffffff ;
		DInputState->POV[ 3 ] = 0xffffffff ;
		return -1 ;
	}

	pad = &InputSysData.Pad[ JoypadNum ] ;

	// 入力状態の更新
	UpdateJoypadInputState( JoypadNum ) ;

	// 入力状態を代入する
	if( DInputState )
	{
		*DInputState = pad->State ;
	}

	// 正常終了
	return 0 ;
}

// 指定の入力デバイスが XInput に対応しているかどうかを取得する
// ( 戻り値  TRUE:XInput対応の入力デバイス  FALSE:XInput非対応の入力デバイス   -1:エラー )
// ( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern int NS_CheckJoypadXInput( int InputType )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		return -1 ;
	}

	// 環境依存処理に任せる
	return CheckJoypadXInput_PF( InputType ) ;
}

// XInput から得られる入力デバイス( Xbox360コントローラ等 )の生のデータを取得する( XInput非対応のパッドの場合はエラーとなり -1 を返す、DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern int NS_GetJoypadXInputState(	int InputType, XINPUT_STATE *XInputState )
{
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		_MEMSET( XInputState, 0, sizeof( XINPUT_STATE ) ) ;
		return -1 ;
	}

	pad = &InputSysData.Pad[ JoypadNum ] ;

	// XInput に対応していなかったらエラー
	if( NS_CheckJoypadXInput( InputType ) == FALSE )
	{
		_MEMSET( XInputState, 0, sizeof( XINPUT_STATE ) ) ;
		return -1 ;
	}

	// 入力状態の更新
	UpdateJoypadInputState( JoypadNum ) ;

	// 入力状態を代入する
	if( XInputState )
	{
		*XInputState = pad->XInputState ;
	}

	// 正常終了
	return 0 ;
}

// ジョイパッドの入力に対応したキーボードの入力を設定する
extern	int NS_SetJoypadInputToKeyInput( int InputType, int PadInput, int KeyInput1, int KeyInput2 , int KeyInput3 , int KeyInput4 )
{
	unsigned int Bit , i ;

	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( JoypadNum < 0 || JoypadNum >= MAX_JOYPAD_NUM )
		return 0 ;

	Bit = (unsigned int)PadInput ;
	for( i = 0 ; i < 32 ; i ++ )
	{
		if( Bit & ( 1 << i ) )
		{
			InputSysData.KeyToJoypadInput[ JoypadNum ][ i ][ 0 ] = KeyInput1 ;
			InputSysData.KeyToJoypadInput[ JoypadNum ][ i ][ 1 ] = KeyInput2 ;
			InputSysData.KeyToJoypadInput[ JoypadNum ][ i ][ 2 ] = KeyInput3 ;
			InputSysData.KeyToJoypadInput[ JoypadNum ][ i ][ 3 ] = KeyInput4 ;
		}
	}

	// 正常終了
	return 0 ;
}

// ジョイパッドの無効ゾーンの設定を行う
extern	int NS_SetJoypadDeadZone( int InputType, double Zone )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	INPUTPADDATA *pad = &InputSysData.Pad[ JoypadNum ] ;
	DWORD ZoneI ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;
	
	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		return AutoInitialize_PF() ;
	}

	// エラーチェック
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		return 0 ;
	}

	// 今までと同じゾーンの場合は何もせず終了
	ZoneI = ( DWORD )_DTOL( Zone * 65536 ) ;
	if( pad->DeadZone == ZoneI )
	{
		pad->DeadZoneD = Zone ;
		return 0 ;
	}

	// ゾーンを保存
	pad->DeadZone = ZoneI ;
	pad->DeadZoneD = Zone ;

	// 環境依存処理
	SetJoypadDeadZone_PF( pad ) ;

	// 正常終了
	return 0 ;
}

// ジョイパッドの無効ゾーンの設定を取得する( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )   戻り値:無効ゾーン( 0.0 〜 1.0 )
extern	double NS_GetJoypadDeadZone( int InputType )
{
	INPUTPADDATA *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		AutoInitialize_PF() ;
	}

	// エラーチェック
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		return -1.0 ;
	}

	pad = &InputSysData.Pad[ JoypadNum ] ;

	// 値を返す
	return pad->DeadZoneD ;
}

// ジョイパッドの振動を開始する
extern	int NS_StartJoypadVibration( int InputType, int Power, int Time, int EffectIndex )
{
	int               SetPower ;
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
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		return 0 ;
	}

	// EffectIndex がマイナスの場合は処理を分岐
	if( EffectIndex < 0 )
	{
		if( Power < 0 )
		{
			NS_StartJoypadVibration( InputType, pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power, Time, DINPUTPAD_MOTOR_LEFT ) ;
		}
		else
		{
			SetPower = Power * 2 ;
			if( SetPower > DX_FFNOMINALMAX / 10 )
			{
				SetPower = DX_FFNOMINALMAX / 10 ;
			}
			NS_StartJoypadVibration( InputType, SetPower, Time, DINPUTPAD_MOTOR_LEFT ) ;
		}

		if( Power < 0 )
		{
			NS_StartJoypadVibration( InputType, pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power, Time, DINPUTPAD_MOTOR_RIGHT ) ;
		}
		else
		{
			SetPower = ( Power - ( DX_FFNOMINALMAX / 10 / 2 ) ) * 2 ;
			if( SetPower <= 0 )
			{
				SetPower = 0 ;
			}
			else
			if( SetPower > DX_FFNOMINALMAX / 10 )
			{
				SetPower = DX_FFNOMINALMAX / 10 ;
			}
			NS_StartJoypadVibration( InputType, SetPower, Time, DINPUTPAD_MOTOR_RIGHT ) ;
		}

		// 終了
		return 0 ;
	}

	if( EffectIndex >= DINPUTPAD_MOTOR_NUM )
	{
		return 0 ;
	}

	// 振動に対応していなければここで終了
	if( CheckJoypadVibrationEnable_PF( pad, EffectIndex ) == FALSE )
	{
		return 0 ;
	}

	// ゆれの大きさを DirectInput の型に合わせる
	if( Power < 0 )
	{
		Power = pad->Effect[ EffectIndex ].Power ;
	}
	else
	{
		Power *= 10 ;
		if( Power >  DX_FFNOMINALMAX )
		{
			Power =  DX_FFNOMINALMAX ;
		}
	}

	// 揺れの大きさが０の場合は振動をストップする
	if( Power == 0 )
	{
		NS_StopJoypadVibration( InputType, EffectIndex ) ;
	}
	else
	{
		// 再生開始時刻などをセット
		pad->Effect[ EffectIndex ].BackTime	= NS_GetNowCount() ;
		pad->Effect[ EffectIndex ].Time		= Time ;
		pad->Effect[ EffectIndex ].CompTime	= 0 ;

		// 既に再生中でゆれの大きさも同じ場合は何もしない
		if( pad->Effect[ EffectIndex ].PlayFlag == TRUE && pad->Effect[ EffectIndex ].Power == Power )
		{
			return 0 ;
		}

		// 再生設定を保存
		pad->Effect[ EffectIndex ].PlayFlag	= TRUE ;
		pad->Effect[ EffectIndex ].Power	= Power ;

		// 再生状態の設定を反映
		RefreshEffectPlayState() ;
	}

	// 終了
	return 0 ;
}

// ジョイパッドの振動を停止する
extern	int NS_StopJoypadVibration( int InputType, int EffectIndex )
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
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum )
	{
		return 0 ;
	}

	// EffectIndex がマイナスの場合は処理を分岐
	if( EffectIndex < 0 )
	{
		// 左右モーターの振動を止める
		NS_StopJoypadVibration( InputType, DINPUTPAD_MOTOR_LEFT ) ;
		NS_StopJoypadVibration( InputType, DINPUTPAD_MOTOR_RIGHT ) ;

		// 終了
		return 0 ;
	}

	if( EffectIndex >= DINPUTPAD_MOTOR_NUM )
	{
		return 0 ;
	}

	// 振動に対応していなければここで終了
	if( CheckJoypadVibrationEnable_PF( pad, EffectIndex ) == FALSE )
	{
		return 0 ;
	}

	// 既に再生が停止していたら何もしない
	if( pad->Effect[ EffectIndex ].PlayFlag == FALSE )
	{
		return 0 ;
	}

	// 再生フラグを倒す
	pad->Effect[ EffectIndex ].PlayFlag = FALSE ;

	// 振動の強さを初期化
	pad->Effect[ EffectIndex ].Power = DX_FFNOMINALMAX ;

	// 再生状態の設定を反映
	RefreshEffectPlayState() ;

	// 終了
	return 0 ;
}

// ジョイパッドのＰＯＶ入力の状態を得る( 単位は角度の１００倍  中心位置にある場合は -1 が返る )
extern int NS_GetJoypadPOVState( int InputType, int POVNumber )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	INPUTPADDATA *pad = &InputSysData.Pad[ JoypadNum ] ;
	DWORD pov;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;
	
	// 初期化されていなかったら条件付きで初期化を行う
	if( InputSysData.InitializeFlag == FALSE )
	{
		AutoInitialize_PF() ;
		return -1 ;
	}

	// エラーチェック
	if( JoypadNum < 0 || JoypadNum >= InputSysData.PadNum || POVNumber >= 4 )
	{
		return -1 ;
	}

	// 情報の更新
	UpdateJoypadInputState( JoypadNum ) ;
	pov = pad->State.POV[ POVNumber ] ;

	// 中心チェック
	if( ( pov & 0xffff ) == 0xffff )
	{
		return -1 ;
	}

	// 何らかの角度がある場合はそのまま返す
	return ( int )pov ;
}

// ジョイパッドの再セットアップを行う( 新たに接続されたジョイパッドがある場合に検出される )
extern int NS_ReSetupJoypad( void )
{
	// ジョイパッドの再セットアップを行う
	return SetupJoypad() ;
}

// キーボードの入力状態の更新
extern int UpdateKeyboardInputState( int UseProcessMessage )
{
	static BOOL InFunction = FALSE ;
	int Result ;

	// 無限再帰防止
	if( InFunction == TRUE )
	{
		return 0 ;
	}
	InFunction = TRUE ;

	// 環境依存処理
	Result = UpdateKeyboardInputState_PF( UseProcessMessage ) ;

	InFunction = FALSE ;

	return Result ;
}

// パッドの入力状態の更新
static int UpdateJoypadInputState( int padno )
{
	int Result ;

	// パッド番号チェック
	if( padno < 0 || padno >= MAX_JOYPAD_NUM )
	{
		return -1 ;
	}

	// 環境依存処理
	Result = UpdateJoypadInputState_PF( padno ) ;

	// 正常終了
	return Result ;
}

// ジョイパッドの振動機能を使用するかどうかのフラグをセットする
extern int NS_SetUseJoypadVibrationFlag( int Flag )
{
	InputSysData.NoUseVibrationFlag = !Flag ;

	// 終了
	return 0 ;
}

// パッドのエフェクト処理に関するフレーム処理を行う
extern int JoypadEffectProcess( void )
{
	int        i ;
	int        j ;
	int        num ;
	int        time ;
	INPUTPADDATA *Pad ;

	// 初期化判定
	if( InputSysData.InitializeFlag == FALSE )
	{
		return 0 ;
	}

	// パッドデバイスの再取得
	num = InputSysData.PadNum ;
	Pad = InputSysData.Pad ;
	time = NS_GetNowCount() ;
	for( i = 0 ; i < num ; i ++, Pad ++ )
	{
		for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
		{
			if( CheckJoypadVibrationEnable_PF( Pad, j ) == FALSE )
			{
				continue ;
			}

			if( Pad->Effect[ j ].PlayFlag == FALSE )
			{
				continue ;
			}

			// 無限再生の場合は特に何もしない
			if( Pad->Effect[ j ].Time < 0 )
			{
				continue ;
			}

			// 前回から時刻が変わっていなかったら何もしない
			if( Pad->Effect[ j ].BackTime == time )
			{
				continue ;
			}

			// 経過時間の加算
			if( time < Pad->Effect[ j ].BackTime )
			{
				Pad->Effect[ j ].BackTime = time ;
			}
			Pad->Effect[ j ].CompTime += time - Pad->Effect[ j ].BackTime ;

			// 今回の時刻を保存
			Pad->Effect[ j ].BackTime = time ;

			// 再生停止のチェック
			if( Pad->Effect[ j ].CompTime >= Pad->Effect[ j ].Time )
			{
				NS_StopJoypadVibration( i + 1, j ) ;
			}
		}
	}

	// 再生状態の更新
	RefreshEffectPlayState() ;

	// 終了
	return 0 ;
}

// パッドエフェクトの再生状態を更新する(ソフトが非アクティブになることもあるため)
extern int RefreshEffectPlayState( void )
{
	// 初期化判定
	if( InputSysData.InitializeFlag == FALSE )
	{
		return 0 ;
	}

	// 環境依存処理
	RefreshEffectPlayState_PF() ;

	// 終了
	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_INPUT
