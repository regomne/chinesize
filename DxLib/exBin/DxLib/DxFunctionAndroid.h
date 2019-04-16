// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Android専用関数プロトタイプ宣言用ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXFUNCTIONANDROID
#define __DXFUNCTIONANDROID

// インクルード ------------------------------------------------------------------
#include <android/native_activity.h>
#include <android/window.h>

// 定義---------------------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE


// アプリで使用している NativeActivity を取得する
extern const ANativeActivity *GetNativeActivity( void ) ;

// Android の APIレベルを取得する
extern int GetAndroidAPILevel( void ) ;

// ソフトのデータ保存用のディレクトリパスを取得する
extern int GetInternalDataPath( TCHAR *PathBuffer, int PathBufferBytes ) ;

// ソフトの外部データ保存用のディレクトリパスを取得する
// PathBuffer : パスを保存するバッファ
// PathBufferBytes : パスを保存するバッファのサイズ( 単位：バイト )
// PathIndex : 外部データ保存用のディレクトリパスの番号( 複数個ある場合があるので )
// IsRemovableFlag : PathIndex で指定したパスが取り外し可能なデバイスかどうかを保存する int型変数のアドレス( 格納された値　TRUE:取り外し可能( SDカード等 )  FALSE:取り外し不可能( 内蔵ストレージ等 ) )
extern int GetExternalDataPath( TCHAR *PathBuffer, int PathBufferBytes, int PathIndex = 0, int *IsRemovableFlag = NULL ) ;

// ソフトの外部データ保存用のディレクトリパスの数を取得する
extern int GetExternalDataPathNum( void ) ;

// 端末に設定されている言語を取得する( 戻り値　-1：エラー　0以上：言語名文字列の格納に必要なバイト数 )
extern int GetLanguage( TCHAR *StringBuffer, int StringBufferSize ) ;

// 端末に設定されている国を取得する( 戻り値　-1：エラー　0以上：国名文字列の格納に必要なバイト数 )
extern int GetCountry( TCHAR *StringBuffer, int StringBufferSize ) ;

// メモリ情報を取得する
// TotalMemory : 総メモリ容量を代入する変数( 単位：byte )、( 情報の取得が不要な場合は NULL を渡す )
// FreeMrmoyy : 空きメモリ容量を代入する変数( 単位：byte )、( 情報の取得が不要な場合は NULL を渡す )
// UseMemory : アプリが使用しているメモリ容量を代入する変数( 単位:byte )、( 情報の取得が不要な場合は NULL を渡す )、( 使用メモリ量の取得は非常に処理負荷が高いので、不要な場合は必ず NULL を渡すようにしてください )
extern int GetAndroidMemoryInfo( LONGLONG *TotalMemory, LONGLONG *FreeMemory, LONGLONG *UseMemory ) ;

// ディスプレイの解像度を取得する
extern int GetAndroidDisplayResolution( int *SizeX, int *SizeY ) ;

// ディスプレイを本体設定に従って時間経過でスリープ状態になることを許可するかどうかを設定する( Flag  TRUE = スリープ状態になることを許可しない  FALSE = スリープ状態を許可する( デフォルト ) )
extern int SetKeepScreenOnFlag( int Flag ) ;

// 加速度センサーのベクトル値を取得する
extern VECTOR GetAccelerometerVector( void ) ;

// センサーから得られる方角を取得する
// 縦持ち( 地面に対して端末を垂直に持った状態 )の場合に正しく値が取得できます
// 戻り値の x:方角( 単位：ラジアン  0.0f:北  π/2.0f:東  -π/2.0f:西  -π or π:南 )
// 戻り値の y:前後の傾斜( 単位：ラジアン  0.0f:端末が縦持ちで垂直( 逆さまで垂直の場合含む )の状態  π/2.0f:前方方向に90度倒した状態( 画面が空を向いている状態 )  -π/2.0f:後方方向に90度倒した状態( 画面が地面を向いている状態 ) )
// 戻り値の z:左右の傾斜( 単位：ラジアン  0.0f:端末が縦持ちで垂直の状態  π/2.0f:右方向に90度倒した状態  -π/2.0f:左方向に90度倒した状態  -π or π:端末が上下さかさまで垂直の状態 )
extern VECTOR GetOrientationVector( void ) ;

// センサーのベクトル値を取得する
extern VECTOR GetAndroidSensorVector( int SensorType /* DX_ANDROID_SENSOR_ACCELEROMETER など */ ) ;

// センサーが有効かどうかを取得する( 戻り値　TRUE：有効　FALSE：無効 )
extern int CheckAndroidSensor( int SensorType /* DX_ANDROID_SENSOR_ACCELEROMETER など */ ) ;



// 振動機能関係( 使用するには AndroidManifest.xml に <uses-permission android:name="android.permission.VIBRATE" /> を書き加える必要があります )

// 振動機能があるか取得する( 戻り値　TRUE : 振動機能あり  FALSE : 振動機能なし )
extern int Vibrator_hasVibrator( void ) ;

// 振動機能に振幅制御( 振動の強さ制御 )機能があるかどうかを取得する( 戻り値　TRUE : 振幅制御ができる  FALSE : 振幅制御はできない )
extern int Vibrator_hasAmplitudeControl( void ) ;

// 振動を開始する
// Milliseconds : 振動させる時間( 単位：ミリ秒 )
// Amplitude : 振動の強さ( 0( 振動なし ) 〜 255( 最大振幅 ) )、振動の強さの制御に対応していない場合は無視されます
extern int Vibrator_vibrate( int Milliseconds, int Amplitude ) ;

// 振動パターン付きの振動を開始する
// Timings : 振動パターンを定義する時間( 単位：ミリ秒 )の配列( Timings[0]:振動を開始するまでの待ち時間、Timings[1]:振動し続ける時間、Timings[2]:振動を停止している時間、Timings[3]:振動し続ける時間、Timings[4]:振動を停止している時間　　以下繰り返し )
// TimingsLength : Timings の配列の長さ
// Repeat : 振動パターンを繰り返す場合の配列の要素番号( 例　0=最初から繰り返し  1:配列要素の1から繰り返し )、-1 で繰り返し無し
extern int Vibrator_vibrate_with_OnOffTimings( int *Timings, int TimingsLength, int Repeat ) ;

// 振幅制御付き( 振動の強さ指定付き )振動を開始する( Vibrator_hasAmplitudeControl の戻り値が TRUE の場合のみ使用可能 )
// Timings : 振動の強さを変更するタイミング( 単位：ミリ秒 )の配列
// Amplitudes : 振動の強さ( 0( 振動なし ) 〜 255( 最大振幅 ) )の配列
// ArrayLength : Timings と Amplitudes の配列の長さ( 配列の長さはどちらも同じである必要があります )
// Repeat : Timings と Amplitudes で示される振動パターンを繰り返す場合の配列の要素番号( 例　0=最初から繰り返し  1:配列要素の1から繰り返し )、-1 で繰り返し無し
extern int Vibrator_vibrate_with_Amplitudes( int *Timings, int *Amplitudes, int ArrayLength, int Repeat ) ;

// 振動を中断する
extern int Vibrator_cancel( void ) ;



// 曜日や週に関する情報を取得する
// DayOfWeek : 曜日( 1:日曜日 2:月曜日 3:火曜日 4:水曜日 5:木曜日 6:金曜日 7:土曜日 )
// WeekOfMonth : 今日が今月の何週目なのかの値( 1:１週目  2:２週目  3:３週目  4:４週目  5:５週目 )
// DayOfWeekInMonth : 今日の曜日が今月何回目に当たるか、の値( 1:１回目  2:２回目  3:３回目  4:４回目 ... )
extern int GetAndroidWeekInfo( int *DayOfWeek, int *WeekOfMonth, int *DayOfWeekInMonth ) ;

// res/values/strings.xml の string リソースを取得する
// ValueName：string 情報名
// StringBuffer：ValueName が示す文字列を格納するバッファの先頭アドレス
// StringBufferBytes：StringBuffer のサイズ( 単位：バイト )
// 戻り値　　-1：指定の string 情報は無かった　　-1以外：StringBuffer に必要なバッファのサイズ( 単位：バイト )
extern int GetAndroidResource_Strings_String(           const TCHAR *ValueName,                         TCHAR *StringBuffer, int StringBufferBytes ) ;
extern int GetAndroidResource_Strings_StringWithStrLen( const TCHAR *ValueName, size_t ValueNameLength, TCHAR *StringBuffer, int StringBufferBytes ) ;

// AndroidManifest.xml に書かれた <meta-data android:name="test0" android:value="abcde" /> などのメタデータの文字列を取得する
// ValueName：メタデータ名
// StringBuffer：ValueName が示す文字列を格納するバッファの先頭アドレス
// StringBufferBytes：StringBuffer のサイズ( 単位：バイト )
// 戻り値　　-1：指定のメタデータは無かった　　-1以外：StringBuffer に必要なバッファのサイズ( 単位：バイト )
extern int GetAndroidMetaData_String(           const TCHAR *ValueName,                         TCHAR *StringBuffer, int StringBufferBytes ) ;
extern int GetAndroidMetaData_StringWithStrLen( const TCHAR *ValueName, size_t ValueNameLength, TCHAR *StringBuffer, int StringBufferBytes ) ;

// AndroidManifest.xml に書かれた <meta-data android:name="bool0" android:value="true" /> などの Boolean型のメタデータを取得する
// ValueName：メタデータ名
// 戻り値　　-1：指定のメタデータは無かった　　TRUE：指定のメタデータは true だった   FALSE：指定のメタデータは false だった
// extern int GetAndroidMetaData_Boolean(           const TCHAR *ValueName                         ) ;
// extern int GetAndroidMetaData_BooleanWithStrLen( const TCHAR *ValueName, size_t ValueNameLength ) ;

// 通知を発行する
// Title         : タイトル
// SubTitle      : サブタイトル
// Icon          : アイコン ( -1 でデフォルトのアイコン )
// ShowWhen      : 通知時刻を表示するか ( TRUE : 表示する  FALSE : 表示しない )
// AutoCancel    : 通知がタップされたら通知を削除するか ( TRUE : タップされたら通知を削除する   FALSE : Cancel されるまで通知を削除しない )
// NotifyID      : 通知ID
// Vibrate       : 振動パターン用の int型配列( 時間の単位はミリ秒 )、NULL の場合は振動なし( Vibrate[0]:振動停止時間  Vibrate[1]:振動時間  Vibrate[2]:振動停止時間  Vibrate[3]:振動時間  ... 以下繰り返し )
// VibrateLength : Vibrate 配列の要素数
// LightColor    : 通知時のライトの色( GetColor で取得 )、LightOnTime か LightOffTime が 0 の場合はライトの点灯無し
// LightOnTime   : ライトの点灯時間( 単位：ミリ秒 )
// LightOffTime  : ライトの消灯時間( 単位：ミリ秒 )
extern int AndroidNotification(
	const TCHAR *Title, const TCHAR *SubTitle, int Icon = -1, int ShowWhen = TRUE,
	int AutoCancel = TRUE, int NotifyId = 0, int *Vibrate = NULL, int VibrateLength = 0,
	unsigned int LightColor = 0, int LightOnTime = 0, int LightOffTime = 0 ) ;
extern int AndroidNotificationWithStrLen(
	const TCHAR *Title, size_t TitleLength, const TCHAR *SubTitle, size_t SubTitleLength, int Icon = -1, int ShowWhen = TRUE,
	int AutoCancel = TRUE, int NotifyId = 0, int *Vibrate = NULL, int VibrateLength = 0,
	unsigned int LightColor = 0, int LightOnTime = 0, int LightOffTime = 0 ) ;

// 通知をキャンセルする
// NotifyID : 通知ID
extern int AndroidNotificationCancel( int NotifyId ) ;

// 全ての通知をキャンセルする
extern int AndroidNotificationCancelAll( void ) ;

// 指定の URL をブラウザで開く( BrowserAppPackageName か BrowserAppClassName が NULL の場合は標準ブラウザで開く )
// URL                   : 開くURL
// BrowserAppPackageName : ブラウザのパッケージ名( NULL で標準ブラウザ )
// BrowserAppClassName   : ブラウザのクラス名( NULL で標準ブラウザ )
extern int AndroidJumpURL(           const TCHAR *URL,                   const TCHAR *BrowserAppPackageName = NULL,                                         const TCHAR *BrowserAppClassName = NULL                                       ) ;
extern int AndroidJumpURLWithStrLen( const TCHAR *URL, size_t URLLength, const TCHAR *BrowserAppPackageName = NULL, size_t BrowserAppPackageNameLength = 0, const TCHAR *BrowserAppClassName = NULL, size_t BrowserAppClassNameLength = 0 ) ;

// Wifi の電波強度を取得する( 戻り値：電波の強度 0 〜 100 )
// ( 使用するには AndroidManifest.xml に <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" /> を書き加える必要があります )
extern int GetWifiSignalLevel( void ) ;

// アプリが非アクティブになった際に呼ばれるコールバック関数を登録する
extern int SetAndroidLostFocusCallbackFunction( void (* Callback )( void *Data ), void *CallbackData ) ;

// アプリがアクティブになった際に呼ばれるコールバック関数を登録する
extern int SetAndroidGainedFocusCallbackFunction( void (* Callback )( void *Data ), void *CallbackData ) ;




// OpenGL 関係の情報を取得する
extern int GetOpenGLInfo(
	TCHAR **Version = NULL,
	TCHAR **Renderer = NULL,
	TCHAR **Vendor = NULL,
	TCHAR ***ExtensionNames = NULL,
	TCHAR **Shader_Language_Version = NULL,
	int *Shader_Max_Vertex_Attribs = NULL,						// GL_MAX_VERTEX_ATTRIBS
	int *Shader_Max_Vertex_Uniform_Vectors = NULL,				// GL_MAX_VERTEX_UNIFORM_VECTORS
	int *Shader_Max_Varying_Vectors = NULL,						// GL_MAX_VARYING_VECTORS
	int *Shader_Max_Combined_Texture_Image_Units = NULL,		// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
	int *Shader_Max_Vertex_Texture_Image_Units = NULL,			// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
	int *Shader_Max_Texture_Image_Units = NULL,					// GL_MAX_TEXTURE_IMAGE_UNITS
	int *Shader_Max_Fragment_Uniform_Vectors = NULL				// GL_MAX_FRAGMENT_UNIFORM_VECTORS
) ;

// グラフィックハンドルのテクスチャオブジェクトを取得する
extern unsigned int GetGraphOpenGLESTextureObject( int GrHandle ) ;

// ＤＸライブラリが行った OpenGL ES の設定を再度行う( 特殊用途 )
extern int RefreshDxLibOpenGLESSetting( void ) ;




// イコライザー機能を使用するかどうかを設定する( DxLib_Init 呼び出し前のみ実行可能 )
// UseFlag  TRUE:使用する  FALSE:使用しない
extern int          SetUseEqualizer( int UseFlag ) ;

// イコライザー関係関数
extern int			SearchEqualizerBandIndex( int Frequency ) ;							// イコライザーの与えられた周波数に最も影響を与える周波数帯域を取得します( 戻り値　-1:エラー  0以上：周波数帯域インデックス )
extern int			GetEqualizerBandNum( void ) ;										// イコライザーに指定できる周波数帯域の数を取得する
extern int			SetEqualizerBandLevel( int BandIndex, int Level ) ;					// 与えられたイコライザーの周波数帯域を与えられたゲイン値に設定します
extern int			GetEqualizerBandLevel( int BandIndex ) ;							// イコライザーの指定された周波数帯域のゲイン値を取得します
extern int			GetEqualizerBandFreqRange( int BandIndex, int *pMin, int *pMax ) ;	// イコライザーの指定された周波数帯域の周波数範囲を取得します( pMin と pMax に最小周波数と最大周波数が格納されます( 単位：ヘルツ ) )
extern int			GetEqualizerBandCenterFreq( int BandIndex ) ;						// イコライザーの指定された周波数帯域の中心周波数を取得します( 戻り値　中心周波数( 単位：ヘルツ ) )
extern int			GetEqualizerCurrentPreset( void ) ;									// イコライザーの現在のプリセットを取得します( 戻り値　プリセット番号 )
extern int			GetEqualizerPresetNum( void ) ;										// イコライザーのプリセットの数を取得します( 戻り値　プリセットの数 )
extern const TCHAR *GetEqualizerPresetName( int PresetIndex ) ;							// イコライザーのプリセット名を取得します( 戻り値　プリセット名の文字列が格納されたメモリ領域の先頭アドレス )
extern int			SetUseEqualizerPreset( int PresetIndex ) ;							// 指定したプリセットに従ってイコライザーを設定します



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXFUNCTIONANDROID

