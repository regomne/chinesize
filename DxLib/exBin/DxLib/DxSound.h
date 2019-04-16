// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		サウンドプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSOUND_H__
#define __DXSOUND_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_SOUND

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxRingBuffer.h"
#include "DxSoundConvert.h"
#include "DxHandle.h"
#include "DxFile.h"

#ifdef __WINDOWS__
#include "Windows/DxSoundWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxSoundAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxSoundiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define MAX_SOUNDBUFFER_NUM				(10)			// DIRECTSOUNDBUFFER バッファの最大数
#define STS_SETSOUNDNUM					(4)				// ストリーム風サウンド再生に登録できるサウンドの数

#define SOUNDBUFFER_MAX_CHANNEL_NUM		(8)				// 対応するサウンドチャンネルの最大数

#define SOUND_SINTABLE_DIV				(65536)			// サインテーブルの精度

#define SOUND_FFT_BITCOUNT_MIN			(4)				// 高速フーリエ変換で使用するサンプル数の最小ビット数
#define SOUND_FFT_BITCOUNT_MAX			(16)			// 高速フーリエ変換で使用するサンプル数の最大ビット数
	
#define SOUND_BEEPSOUNDBUFFER_NUM		(3)				// BEEP音再生用のサウンドバッファの数


// サウンドハンドルエラーチェック用マクロ
#define SOUNDHCHK( HAND, SPOINT )			HANDLECHK(       DX_HANDLETYPE_SOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SOUNDHCHK_ASYNC( HAND, SPOINT )		HANDLECHK_ASYNC( DX_HANDLETYPE_SOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// ソフトサウンドハンドルエラーチェック用マクロ
#define SSND_MASKHCHK( HAND, SPOINT )		HANDLECHK(       DX_HANDLETYPE_SOFTSOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SSND_MASKHCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_SOFTSOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// ミュージックハンドルエラーチェック用マクロ
#define MIDI_MASKHCHK( HAND, SPOINT )		HANDLECHK(       DX_HANDLETYPE_MUSIC, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define MIDI_MASKHCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_MUSIC, HAND, *( ( HANDLEINFO ** )&SPOINT ) )


// 構造体定義 --------------------------------------------------------------------

// ファイルからサウンドハンドルを作成する処理に必要なグローバルデータを纏めたもの
struct LOADSOUND_GPARAM
{
	int							NotInitSoundMemDelete ;					// InitSoundMem で削除しないかどうかのフラグ( TRUE:InitSoundMemでは削除しない  FALSE:InitSoundMemで削除する )
	int							Create3DSoundFlag ;						// 3Dサウンドを作成するかどうかのフラグ( TRUE:３Ｄサウンドを作成する  FALSE:３Ｄサウンドを作成しない )
	int							CreateSoundDataType ;					// 作成するサウンドデータのデータタイプ
	int							CreateSoundPitchRateEnable ;			// 作成するサウンドデータのピッチレートが有効かどうか
	float						CreateSoundPitchRate ;					// 作成するサウンドデータのピッチレート
	int							CreateSoundTimeStretchRateEnable ;		// 作成するサウンドデータのタイムストレッチレートが有効かどうか
	float						CreateSoundTimeStretchRate ;			// 作成するサウンドデータのタイムストレッチレート
	int							CreateSoundLoopStartTimePosition ;		// 作成するサウンドデータのループ範囲の先端( ミリ秒 )
	int							CreateSoundLoopStartSamplePosition ;	// 作成するサウンドデータのループ範囲の先端( サンプル )
	int							CreateSoundLoopEndTimePosition ;		// 作成するサウンドデータのループ範囲の終端( ミリ秒 )
	int							CreateSoundLoopEndSamplePosition ;		// 作成するサウンドデータのループ範囲の終端( サンプル )
	int							CreateSoundIgnoreLoopAreaInfo ;			// 作成するサウンドデータでループ範囲情報を無視するかどうか
	int							DisableReadSoundFunctionMask ;			// 使用しない読み込み処理のマスク
#ifndef DX_NON_OGGVORBIS
	int							OggVorbisBitDepth ;						// ＯｇｇＶｏｒｂｉｓ使用時のビット深度(1:8bit 2:16bit)
	int							OggVorbisFromTheoraFile ;				// Ogg Theora ファイル中の Vorbis データを参照するかどうかのフラグ( TRUE:Theora ファイル中の Vorbis データを参照する )
#endif
} ;

// サウンドバッファをロックした際に使用するデータ型
struct SOUNDBUFFERLOCKDATA
{
	DWORD						StartOffst ;
	BYTE						*WriteP, *WriteP2 ;
	DWORD						Length, Length2 ;
	DWORD						Offset, Offset2 ;
	DWORD						Valid, Valid2 ;
} ;

// 波形データ構造体
struct WAVEDATA
{
	void						*Buffer ;				// 波形データ
	void						*DoubleSizeBuffer ;		// ２倍容量の波形データ
	int							Bytes ;					// サイズ
	int							RefCount ;				// 参照数
} ;

// ３Ｄサウンドデータの基本的な情報
struct _3DSOUNDINFO
{
	VECTOR						Position ;				// 位置
	VECTOR						FrontDirection ;		// 前方方向
	VECTOR						UpDirection ;			// 上方向
	VECTOR						Velocity ;				// 移動速度
	float						InnerAngle ;			// 可聴内角度
	float						OuterAngle ;			// 可聴外角度
	float						InnerVolume ;			// 可聴内角度での音量スケール
	float						OuterVolume ;			// 可聴外角度での音量スケール
} ;

// サウンドバッファ構造体
struct SOUNDBUFFER
{
	int							Valid ;					// データが有効か否か(TRUE:有効 FALSE:無効)

	int							Pan ;					// パン( 10000:左100%右0%  0:左右100%  -100000:右100% )
	int							Volume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;	// ボリューム( 10000:0%  0:100% )
	int							Frequency ;				// 再生周波数( -1:デフォルト )
	int							SampleNum ;				// サンプルの数

	WAVEFORMATEX				Format ;				// バッファのフォーマット

	int							UseGetCurrentPosition ;	// GetCurrentPosition を使用するかどうか
	int							IsStream ;				// ストリーム再生用のサウンドバッファかどうか

	// DirectSound 以外の場合に使用
	WAVEDATA					*Wave ;					// 波形データバッファ
	volatile int				State ;					// 再生状態( TRUE:再生中  FALSE:停止中 )
	volatile int				Pos ;					// 再生位置
	volatile int				CompPos ;				// 転送が完了した位置
	int							Loop ;					// ループするかどうか( TRUE:ループする  FALSE:ループしない )
	int							CalcPan ;				// キャプチャ用に計算した後のパン( 256:左100%右0%  0:左右100%  -256:右100% )
	int							CalcVolume ;			// キャプチャ用に計算した後のボリューム( 256:100%  0:0% )
	volatile int				StopTimeState ;			// StopTime の状態( 0:動作無し 1:再生終了待ち 2:時間経過待ち )
	volatile int				StopTime ;				// 再生が停止した時間

	// ３Ｄサウンド再生用データ
	int							Is3DSound ;												// ３Ｄサウンドかどうか
	_3DSOUNDINFO				EmitterInfo ;											// ３Ｄサウンド用の情報
	float						EmitterRadius ;											// ３Ｄサウンド用音源の音が届く距離
	float						EmitterInnerRadius ;									// ３Ｄサウンド用音源の音が音量１００％で聞こえる距離
	int							EmitterDataChangeFlag ;									// ３Ｄサウンド用音源情報が変更されたかどうか( TRUE:変更された  FALSE:変更されていない )
	float						DSound_Calc3DVolume ;									// DirectSound用の３Ｄ計算後のボリューム
	float						DSound_Calc3DPan ;										// DirectSound用の３Ｄ計算後のパン

	SOUNDBUFFER_PF				PF ;					// 環境依存情報
} ;

// ストリーム風サウンドプレイ用ファイルデータ
struct STREAMFILEDATA
{
	signed char					LoopNum ;					// ループ回数( -1:無限ループ )
	int							DataType ;					// データのタイプ( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
	void						*FileImage ;				// メモリ上に展開したファイルイメージ( DX_SOUNDDATATYPE_FILE 以外で使用 )
	int							FileImageSize ;				// メモリ上に展開したファイルイメージのサイズ( DX_SOUNDDATATYPE_FILE 以外で使用 )
	STREAMDATA					MemStream ;					// メモリストリーム処理用データ( DX_SOUNDDATATYPE_FILE 以外で使用 )
	int							LoopSamplePosition ;		// ループ後の再生位置(サンプル単位、OGG か 無圧縮WAV のみ有効 )
	int							LoopStartSamplePosition ;	// ループを開始する再生位置(サンプル単位、OGG か 無圧縮WAV のみ有効 )
	
	SOUNDCONV					ConvData ;					// フォーマット変換処理用データ
} ;

// ストリーム風サウンドプレイ用データ
struct STREAMPLAYDATA
{
	int							BufferBorrowSoundHandle ;								// DirectSoundBuffer を貸りているサウンドハンドル(-1:借りていない)
	int							BufferBorrowSoundHandleCount ;							// DirectSoundBuffer を貸している数(0:貸していない)
	int							BufferUseSoundHandle ;									// DirectSoundBuffer を現在使用しているサウンドハンドル(-1:誰も今は使っていない)
	int							DeleteWaitFlag ;										// DirectSoundBuffer を貸しているから削除できないで居るのか、フラグ
	
	BYTE						*SoundBuffer ;											// サウンドバッファ( XAudio2用 )
	DWORD						SoundBufferSize ;										// DirectSoundBuffer のサイズ( 単位はバイト )
	int							SoundBufferCompCopyOffset ;								// 前回のサウンドデータコピーで転送が完了したサウンドデータ領域の末端オフセット
	int							SoundBufferCopyStartOffset ;							// 『ここまできたら次のサウンドデータコピーを開始する』というデータオフセット

	STREAMFILEDATA				File[ STS_SETSOUNDNUM ] ;								// ストリームファイルデータ
	BYTE						FileLibraryLoadFlag[ STS_SETSOUNDNUM ] ;				// ライブラリがロードしたデータかどうか
	int							FileNum ;												// ストリームファイルデータの数
	int							FileActive ;											// アクティブなストリームファイルデータ番号
	int							FileLoopCount ;											// 現在アクティブなストリームデータがループした回数
	int							FileCompCopyLength ;									// 現在アクティブなストリームデータ中のサウンドバッファへの転送が完了したバイト数
	int							FileUseFile ;											// メモリイメージではなくディスクなどの補助記憶装置上にあるファイルを使用しているかどうか
	int							TotalSample ;											// 全ストリームのサンプル総数

	DWORD						CompPlayWaveLength ;									// 再生し終わった波形データの長さ
	DWORD						PrevCheckPlayPosition ;									// 前回チェックした再生位置

	BYTE						StartSetupCompFlag ;									// 再生準備完了フラグ
	BYTE						EndWaitFlag ;											// 終了待ち中フラグ
	BYTE						LoopPositionValidFlag ;									// ループ位置が有効フラグ
	BYTE						AllPlayFlag ;											// 全体を通して一度でも最後でも再生したかフラグ
	DWORD						LoopAfterCompPlayWaveLength ;							// ループ後の CompPlayWaveLength に加算する値
	
	DWORD						EndOffset ;												// 終了オフセット
	DWORD						EndStartOffset ;										// 終了処理開始オフセット
	DWORD						LoopPosition ;											// ループ位置(再生バッファ内のアドレス)

	HANDLELIST					StreamSoundList ;										// ストリームサウンドリスト処理用構造体
} ;

// ノーマルサウンドプレイ用データ
struct NORMALPLAYDATA
{
	int							WaveSize ;												// 波形データのサイズ
	void						*WaveData ;												// 波形データ
	int							*WaveDataUseCount ;										// 波形データ参照数
	int							BufferDuplicateFlag[ MAX_SOUNDBUFFER_NUM ] ;			// 複製されたサウンドバッファかどうかのフラグ
	int							BackPlayBufferNo ;										// 一つ前に再生したサウンドバッファの番号(再生用バッファが複数ある場合用)
} ;

// サウンドデータ
struct SOUND
{
	HANDLEINFO					HandleInfo ;											// ハンドル共通情報

	HANDLELIST					_3DSoundList ;											// ３Ｄサウンドリスト処理用構造体
	int							Is3DSound ;												// ３Ｄサウンドかどうか( TRUE:３Ｄサウンド  FALSE:非３Ｄサウンド )
	int							AddPlay3DSoundList ;									// Play3DSoundList がリストに追加されているかどうか( TRUE:追加されている  FALSE:追加されていない )
	HANDLELIST					Play3DSoundList ;										// 再生中の３Ｄサウンドのリスト処理用構造体

	int							PlayFinishDeleteFlag ;									// サウンドの再生が終了したら削除するかどうか( TRUE:削除する  FALSE:削除しない )
	HANDLELIST					PlayFinishDeleteSoundList ;								// サウンドの再生が終了したら削除するサウンドのリスト処理用構造体

	int							NotInitSoundMemDelete ;									// InitSoundMem で削除しないかどうかのフラグ( TRUE:InitSoundMemでは削除しない  FALSE:InitSoundMemで削除する )

//	char						FilePath[ 256 ] ;										// ディレクトリ
//	char						SoundName[ 256 ] ;										// サウンドファイルネーム

	int							ValidBufferNum ;										// 有効なバッファの数
	int							BufferPlayStateBackupFlagValid[ MAX_SOUNDBUFFER_NUM ] ;	// BufferPlayStateBackupFlag が有効かどうかのフラグ( TRUE:有効  FALSE:無効 )
	int							BufferPlayStateBackupFlag[ MAX_SOUNDBUFFER_NUM ] ;		// サウンドバッファを一時停止するときの再生状態フラグ
	SOUNDBUFFER					Buffer[ MAX_SOUNDBUFFER_NUM ] ;							// サウンドバッファ
	WAVEFORMATEX				BufferFormat ;											// サウンドバッファのフォーマット

	int							Type ;													// サウンドのタイプ
	int							PlayType ;												// 再生タイプ

	STREAMPLAYDATA				Stream ;												// ストリーム風サウンドプレイ用データ
	NORMALPLAYDATA				Normal ;												// ノーマルサウンドプレイ用データ

	int							PresetReverbParam ;										// ３Ｄサウンド時に設定するプリセットリバーブパラメータ番号( DX_REVERB_PRESET_DEFAULT 等 )
	SOUND3D_REVERB_PARAM		ReverbParam ;											// ３Ｄサウンド時に設定するリバーブパラメータ( PresetReverbParam が -1 の際に使用 )

	int							BaseVolume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;				// 基本ボリューム( 100分の1デシベル単位 0 〜 10000 )( -1:デフォルト )
	int							BaseVolume8bit[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;			// 基本ボリューム( 0 〜 255 )( -1:デフォルト )
	int							BasePan ;												// 基本パン( -1:デフォルト )
	int							BaseFrequency ;											// 基本再生周波数( -1:デフォルト )
	VECTOR						Base3DPosition ;										// 基本再生位置
	float						Base3DRadius ;											// 基本聞こえる距離
	VECTOR						Base3DVelocity ;										// 基本音の速度
	int							PitchRateEnable ;										// ピッチレートが有効化どうか( TRUE:有効  FALSE:無効 )
	float						PitchRate ;												// ピッチレート

	BYTE						ValidNextPlayVolume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;	// NextPlayVolume が有効かどうか( 1:有効 0:無効 )
	BYTE						ValidNextPlayPan ;										// NextPlayPan が有効かどうか( 1:有効 0:無効 )
	BYTE						ValidNextPlayFrequency ;								// NextPlayFrequency が有効かどうか( 1:有効 0:無効 )
	BYTE						ValidNextPlay3DPosition ;								// NextPlay3DPosition が有効かどうか( 1:有効 0:無効 )
	BYTE						ValidNextPlay3DRadius ;									// NextPlay3DRadius が有効かどうか( 1:有効 0:無効 )
	BYTE						ValidNextPlay3DVelocity ;								// NextPlay3DVelocity が有効かどうか( 1:有効 0:無効 )
	int							NextPlayVolume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;			// 次に鳴らす音のボリューム
	int							NextPlayPan ;											// 次に鳴らす音のパン
	int							NextPlayFrequency ;										// 次に鳴らす音の周波数
	VECTOR						NextPlay3DPosition ;									// 次に鳴らす音の位置
	float						NextPlay3DRadius ;										// 次に鳴らす音の聞こえる距離
	VECTOR						NextPlay3DVelocity ;									// 次に鳴らす音の移動速度
} ;

// ソフトウエアで扱う波形データの波形データ構造体
struct SOFTSOUND_WAVE
{
	int							BufferSampleNum ;										// バッファのサイズ( サンプル数 )
	void *						Buffer ;												// データバッファ
} ;

// ソフトウエアで扱う波形データのプレイヤー構造体
struct SOFTSOUND_PLAYER
{
	HANDLELIST					SoftSoundPlayerList ;									// ソフトウエアサウンドプレイヤーリスト処理用構造体

	int							StockSampleNum ;										// ストックしているサンプルの数
	RINGBUF						StockSample ;											// サンプルストック用リングバッファ

	int							SoundBufferPlayStateBackupFlagValid ;					// SoundBufferPlayStateBackupFlag が有効かどうかのフラグ( TRUE:有効  FALSE:無効 )
	int							SoundBufferPlayStateBackupFlag ;						// サウンドバッファを一時停止するときの再生状態フラグ
	SOUNDBUFFER					SoundBuffer ;											// 再生用サウンドバッファ
	int							SoundBufferSize ;										// 再生用サウンドバッファのサイズ( バイト単位 )
//	int							BackCheckPlayOffset ;									// 一つ前にチェックした再生位置
	int							DataSetCompOffset ;										// データセットが済んでいるサウンドバッファ上の位置( -1:まだセットしていない )

	int							NoneDataSetCompOffset ;									// 無音データのセットを開始したサウンドバッファ上の位置( -1:まだセットしていない )
	int							NoneDataPlayCheckBackPlayOffset ;						// 無音データの再生を開始したかを検出するための前回チェック時の再生位置
	int							NoneDataPlayStartFlag ;									// 無音データの再生を開始しているかどうかフラグ( TRUE:している  FALSE:していない )

	int							MaxDataSetSize ;										// 再生位置に先行してセットする最大データサイズ、バイト単位( 先行してセットしてあるデータのサイズがこのサイズ以上にならないようにする )
	int							MinDataSetSize ;										// 再生位置に先行してセットする最小データサイズ、バイト単位( 先行してセットしてあるデータのサイズがこのサイズ以下にならないようにする )

	int							IsPlayFlag ;											// 再生中かどうかのフラグ( TRUE:再生中   FALSE:停止中 )
} ;

// ソフトウエアで扱う波形データ
struct SOFTSOUND
{
	HANDLEINFO					HandleInfo ;											// ハンドル共通情報

	int							IsPlayer ;												// プレイヤーかどうかのフラグ( TRUE:プレイヤー  FALSE:サウンドデータ )
	WAVEFORMATEX				BufferFormat ;											// サウンドのフォーマット

	union
	{
		SOFTSOUND_WAVE			Wave ;													// 波形データ
		SOFTSOUND_PLAYER		Player ;												// プレイヤー
	} ;
} ;

#ifndef DX_NON_BEEP

// BEEP音用サウンドバッファのデータ用構造体
struct SOUND_BEEP_BUFFERDATA
{
	SOUNDBUFFER					Buffer ;								// BEEP音再生用のサウンドバッファ
	DWORD						NextFillBufferPosition ;				// 次の波形データ配置開始サンプル位置
	int							NextUseOneCycleSampleBufferPosition ;	// 次の波形データ配置で使用する波形１周分のサンプルバッファのサンプル位置
	int							OneCycleSamples ;						// 波形１周分のサンプル数
	short						*OneCycleSampleBuffer ;					// 波形１周分のサンプルを格納しているバッファ
	int							OneCycleSampleBufferSize ;				// 波形１周分のサンプルを格納しているバッファのサイズ
	int							VolumeUpRequest ;						// BEEP音再生音量アップリクエストフラグ
	int							PlayTime ;								// BEEP音再生用のサウンドバッファの再生を開始した時間
	int							StopRequest ;							// BEEP音再生停止リクエストフラグ
	int							StopTime ;								// BEEP音再生用のサウンドバッファの再生状態を停止にした時間
} ;

#endif // DX_NON_BEEP


// サウンドシステム用データ構造体
struct SOUNDSYSTEMDATA
{
	int							InitializeFlag ;				// 初期化フラグ

	DWORD						OutputChannels ;				// 出力チャンネル数
	DWORD						OutputSmaplesPerSec ;			// 出力サンプリングレート

	int							_3DSoundOneMetreEnable ;		// _3DSoundOneMetre が有効かどうか( TRUE:有効  FALSE:無効 )
	float						_3DSoundOneMetre ;				// ３Ｄサウンド処理用の１メートル
	_3DSOUNDINFO				ListenerInfo ;					// ３Ｄサウンドのリスナーの情報
	VECTOR						ListenerSideDirection ;			// ３Ｄサウンドのリスナーの横方向

	HANDLELIST					_3DSoundListFirst ;				// ３Ｄサウンドハンドルリストの先頭
	HANDLELIST					_3DSoundListLast ;				// ３Ｄサウンドハンドルリストの終端
	DX_CRITICAL_SECTION			_3DSoundListCriticalSection ;	// ３Ｄサウンドリストアクセス時用クリティカルセクション

	HANDLELIST					StreamSoundListFirst ;			// ストリームサウンドハンドルリストの先頭
	HANDLELIST					StreamSoundListLast ;			// ストリームサウンドハンドルリストの終端
	DX_CRITICAL_SECTION			StreamSoundListCriticalSection ;// ストリームサウンドハンドルリストアクセス時用クリティカルセクション

	HANDLELIST					SoftSoundPlayerListFirst ;		// ソフトウエアで扱う波形データのプレイヤーのハンドルリストの先頭
	HANDLELIST					SoftSoundPlayerListLast ;		// ソフトウエアで扱う波形データのプレイヤーのハンドルリストの終端

	HANDLELIST					PlayFinishDeleteSoundListFirst ; // 再生終了時に削除するサウンドハンドルリストの先頭
	HANDLELIST					PlayFinishDeleteSoundListLast ;	 // 再生終了時に削除するサウンドハンドルリストの終端

	HANDLELIST					Play3DSoundListFirst ;			// 再生している３Ｄサウンドリストの先頭
	HANDLELIST					Play3DSoundListLast ;			// 再生している３Ｄサウンドリストの終端
	DX_CRITICAL_SECTION			Play3DSoundListCriticalSection ;// 再生している３Ｄサウンドリストアクセス時用クリティカルセクション

	int							PlayWavSoundHandle ;			// PlayWav関数で鳴らされているWAVEデータのハンドル

	int							Create3DSoundFlag ;				// 3Dサウンドを作成するかどうかのフラグ( TRUE:３Ｄサウンドを作成する  FALSE:３Ｄサウンドを作成しない )
	int							OldVolumeTypeFlag ;				// Ver3.10c以前の音量計算式を使用するかどうかのフラグ( TRUE:古い計算式を使用する  FALSE:新しい計算式を使用する )
	int							SoundMode ;						// 再生形式

	int							MaxVolume ;						// 最大音量

	int							EnableSoundCaptureFlag ;		// サウンドキャプチャを前提とした動作をする
#ifndef DX_NON_SAVEFUNCTION
	int							SoundCaptureFlag ;				// サウンドキャプチャを実行している最中かどうかのフラグ(TRUE:最中 FASLE:違う)
	int							SoundCaptureSample ;			// キャプチャしたサンプルの数(44.1KHz換算)
	DWORD_PTR					SoundCaptureFileHandle ;		// キャプチャしたサウンドを保存しているファイル
#endif // DX_NON_SAVEFUNCTION

	int							CreateSoundDataType ;					// 作成するサウンドデータのデータタイプ
	int							CreateSoundPitchRateEnable ;			// 作成するサウンドデータのピッチレートが有効かどうか
	float						CreateSoundPitchRate ;					// 作成するサウンドデータのピッチレート
	float						CreateSoundPitchRate_Cents ;			// 作成するサウンドデータのピッチレート( セント単位 )
	int							CreateSoundTimeStretchRateEnable ;		// 作成するサウンドデータのタイムストレッチレートが有効かどうか
	float						CreateSoundTimeStretchRate ;			// 作成するサウンドデータのタイムストレッチレート
	int							CreateSoundLoopStartTimePosition ;		// 作成するサウンドデータのループ範囲の先端( ミリ秒 )
	int							CreateSoundLoopStartSamplePosition ;	// 作成するサウンドデータのループ範囲の先端( サンプル )
	int							CreateSoundLoopEndTimePosition ;		// 作成するサウンドデータのループ範囲の終端( ミリ秒 )
	int							CreateSoundLoopEndSamplePosition ;		// 作成するサウンドデータのループ範囲の終端( サンプル )
	int							CreateSoundIgnoreLoopAreaInfo ;			// 作成するサウンドデータでループ範囲情報を無視するかどうか
	int							DisableReadSoundFunctionMask ;			// 使用しない読み込み処理のマスク
#ifndef DX_NON_OGGVORBIS
	int							OggVorbisBitDepth ;				// ＯｇｇＶｏｒｂｉｓ使用時のビット深度(1:8bit 2:16bit)
	int							OggVorbisFromTheoraFile ;		// Ogg Theora ファイル中の Vorbis データを参照するかどうかのフラグ( TRUE:Theora ファイル中の Vorbis データを参照する )
#endif

#ifndef DX_NON_BEEP
	int							BeepFrequency ;									// 再生するBEEP音の周波数
	int							BeepPlay ;										// BEEPを再生中かどうか
	int							BeepSoundBufferUseIndex ;						// 使用するBEEP音再生用のサウンドバッファ
	SOUND_BEEP_BUFFERDATA		BeepSoundBuffer[ SOUND_BEEPSOUNDBUFFER_NUM ] ;	// BEEP音用のサウンドバッファ関連情報
#endif

	short						SinTable[ SOUND_SINTABLE_DIV ] ;	// サインテーブル

	void						*FFTVibrationWorkBuffer[ SOUND_FFT_BITCOUNT_MAX ] ;		// 高速フーリエ変換用作業用バッファ

	SOUNDSYSTEMDATA_PF			PF ;							// 環境依存情報
} ;

// ＭＩＤＩデータ
struct MIDIHANDLEDATA
{
	HANDLEINFO					HandleInfo ;					// ハンドル共通データ

	int							PlayFlag ;						// 再生中か、フラグ
	int							PlayStartFlag ;					// 再生が開始されたか、フラグ
	int							Volume ;						// 音量

	void						*DataImage ;					// ＭＩＤＩファイルのデータが格納されているアドレス( DSOUND.SoundMode が DX_MIDIMODE_MCI の時のみ有効 )
	int							DataSize ;						// ＭＩＤＩファイルのデータサイズ( DSOUND.SoundMode が DX_MIDIMODE_MCI の時のみ有効 )

	int							StartTime ;						// 再生開始時間

	MIDIHANDLEDATA_PF			PF ;							// 環境依存情報
} ;

// ＭＩＤＩシステム用データ構造体
struct MIDISYSTEMDATA
{
	int							PlayFlag ;						// 演奏中フラグ
	int							PlayHandle ;					// 演奏中のハンドル

	wchar_t						FileName[ FILEPATH_MAX ] ;		// ファイルネーム

	int							LoopFlag ;						// ループ演奏か、フラグ

	int							StartTime ;						// 再生開始時間

	int							MemImagePlayFlag ;				// メモリ上のイメージを使用して再生しているか、フラグ( TRUE:メモリイメージ  FALSE:ファイル )

	int							DefaultHandle ;					// 標準の再生関数で使用するMIDIハンドル
	int							DefaultHandleToSoundHandleFlag ;	// 標準の再生関数で再生しているハンドルは音声ハンドルか、フラグ

	MIDISYSTEMDATA_PF			PF ;							// 環境依存情報
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// サウンドシステムデータ
extern SOUNDSYSTEMDATA SoundSysData ;

// ＭＩＤＩ管理用データ
extern MIDISYSTEMDATA MidiSystemData ;	

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int		InitializeSoundSystem( void ) ;																			// サウンドシステムを初期化する
extern	int		TerminateSoundSystem( void ) ;																			// サウンドシステムの後始末をする

extern	int		RefreshStreamSoundPlayCompLength( int SoundHandle, int CurrentPosition = -1, int ASyncThread = FALSE ) ;	// サウンドハンドルの再生完了時間を更新する
extern	int		ST_SoftSoundPlayerProcessAll( void ) ;																	// ソフトウエアで制御する全てのサウンドデータプレイヤーの定期処理を行う

extern	int		StartSoundCapture( const wchar_t *SaveFilePath ) ;															// サウンドキャプチャの開始
extern	int		SoundCaptureProcess( int CaptureSample ) ;																// サウンドキャプチャの周期的処理
extern	int		EndSoundCapture( void ) ;																				// サウンドキャプチャの終了

extern	void	InitLoadSoundGParam( LOADSOUND_GPARAM *GParam ) ;														// LOADSOUND_GPARAM のデータをセットする

extern	int		InitializeSoundHandle( HANDLEINFO *HandleInfo ) ;														// サウンドハンドルの初期化
extern	int		TerminateSoundHandle( HANDLEINFO *HandleInfo ) ;														// サウンドハンドルの後始末

extern	int		LoadSoundMemBase_UseGParam(              LOADSOUND_GPARAM *GParam, const wchar_t *WaveName, int BufferNum, int UnionHandle, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																		// LoadSoundMemBase のグローバル変数にアクセスしないバージョン
extern	int		LoadSoundMemByMemImageBase_UseGParam(    LOADSOUND_GPARAM *GParam, int CreateSoundHandle, int SoundHandle, const void *FileImageBuffer, int ImageSize, int BufferNum, int UnionHandle = -1, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;		// LoadSoundMemByMemImageBase のグローバル変数にアクセスしないバージョン
extern	int		LoadSoundMem2_UseGParam(                 LOADSOUND_GPARAM *GParam, const wchar_t *WaveName1 , const wchar_t *WaveName2, int ASyncLoadFlag = FALSE ) ;																									// LoadSoundMem2 のグローバル変数にアクセスしないバージョン
extern	int		LoadSoundMem2ByMemImage_UseGParam(       LOADSOUND_GPARAM *GParam, const void *FileImageBuffer1, int ImageSize1, const void *FileImageBuffer2, int ImageSize2, int ASyncLoadFlag = FALSE ) ;															// LoadSoundMem2ByMemImage のグローバル変数にアクセスしないバージョン

extern	int		AddStreamSoundMem_UseGParam(       LOADSOUND_GPARAM *GParam, STREAMDATA *Stream, int LoopNum,  int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle = -1, int ASyncThread = FALSE ) ;											// AddStreamSoundMem のグローバル変数にアクセスしないバージョン
extern	int		AddStreamSoundMemToFile_UseGParam( LOADSOUND_GPARAM *GParam, const wchar_t *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1, int ASyncThread = FALSE ) ;																// AddStreamSoundMemToFile のグローバル変数にアクセスしないバージョン
extern	int		AddStreamSoundMemToMem_UseGParam(  LOADSOUND_GPARAM *GParam, void *FileImageBuffer, int ImageSize, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1, int ASyncThread = FALSE ) ;													// AddStreamSoundMemToMem のグローバル変数にアクセスしないバージョン
extern	int		SetupStreamSoundMem_UseGParam(     int SoundHandle, int ASyncThread = FALSE ) ;																																											// SetupStreamSoundMem のグローバル変数にアクセスしないバージョン
extern	int		ProcessStreamSoundMem_UseGParam(   int SoundHandle, int ASyncThread = FALSE ) ;																																											// ProcessStreamSoundMem のグローバル変数にアクセスしないバージョン

extern	int		SetLoopAreaTimePosSoundMem_UseGParam(    int LoopStartTime,           int LoopEndTime,           int SoundHandle, int ASyncThread = FALSE ) ;																											// SetLoopAreaTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern	int		SetLoopAreaSamplePosSoundMem_UseGParam(  int LoopStartSamplePosition, int LoopEndSamplePosition, int SoundHandle, int ASyncThread = FALSE ) ;																											// SetLoopAreaSamplePosSoundMem のグローバル変数にアクセスしないバージョン
extern	int		SetLoopTimePosSoundMem_UseGParam(        int LoopTime,                int SoundHandle, int ASyncThread = FALSE ) ;																																		// SetLoopTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern	int		SetLoopSamplePosSoundMem_UseGParam(      int LoopSamplePosition,      int SoundHandle, int ASyncThread = FALSE ) ;																																		// SetLoopSamplePosSoundMem のグローバル変数にアクセスしないバージョン
extern	int		SetLoopStartTimePosSoundMem_UseGParam(   int LoopStartTime,           int SoundHandle, int ASyncThread = FALSE ) ;																																		// SetLoopStartTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern	int		SetLoopStartSamplePosSoundMem_UseGParam( int LoopStartSamplePosition, int SoundHandle, int ASyncThread = FALSE ) ;																																		// SetLoopStartTimePosSoundMem のグローバル変数にアクセスしないバージョン

extern	int		GetSoundTotalSample_UseGParam( int SoundHandle, int ASyncThread = FALSE ) ;																																												// GetSoundTotalSample のグローバル変数にアクセスしないバージョン
extern	int		SetStreamSoundCurrentPosition_UseGParam( int Byte, int SoundHandle, int ASyncThread = FALSE ) ;																																							// SetStreamSoundCurrentPosition のグローバル変数にアクセスしないバージョン

extern	int		Refresh3DSoundParamAll() ;																				// 再生中のすべての３Ｄサウンドのパラメータを更新する

extern	int		ProcessPlayFinishDeleteSoundMemAll( void ) ;															// 再生が終了したらサウンドハンドルを削除するサウンドの処理を行う
extern	int		ProcessPlay3DSoundMemAll( void ) ;																		// ３Ｄサウンドを再生しているサウンドハンドルに対する処理を行う

extern	int		SetupSoftSoundHandle( int SoftSoundHandle, int IsPlayer, int Channels, int BitsPerSample, int SamplesPerSec, int IsFloatType, int SampleNum ) ;	// ソフトサウンドハンドルをセットアップ

extern	int		InitializeSoftSoundHandle( HANDLEINFO *HandleInfo ) ;													// ソフトサウンドハンドルの初期化
extern	int		TerminateSoftSoundHandle( HANDLEINFO *HandleInfo ) ;													// ソフトサウンドハンドルの後始末

extern	int		DeleteCancelCheckSoftSoundFunction( HANDLEINFO *HandleInfo ) ;											// ソフトサウンドハンドルを削除するかどうかをチェックする関数
extern	int		DeleteCancelCheckSoftSoundPlayerFunction( HANDLEINFO *HandleInfo ) ;									// ソフトサウンドプレーヤーハンドルを削除するかどうかをチェックする関数

extern	int		LoadSoftSoundBase_UseGParam( LOADSOUND_GPARAM *GParam, const wchar_t *FileName, const void *FileImage, int FileImageSize, int ASyncLoadFlag = FALSE ) ;		// ソフトウエアで扱う波形データをファイルまたはメモリ上に展開されたファイルイメージから作成する
extern	int		MakeSoftSoundBase_UseGParam( int IsPlayer, int Channels, int BitsPerSample, int SamplesPerSec, int SampleNum, int IsFloatType, int UseFormat_SoftSoundHandle = -1, int ASyncLoadFlag = FALSE ) ;	// ソフトウエアで制御するサウンドデータハンドルの作成

extern	int		AddMusicData( void ) ;																					// 新しいＭＩＤＩハンドルを取得する

extern	int		InitializeMidiHandle( HANDLEINFO *HandleInfo ) ;														// ＭＩＤＩハンドルを初期化をする関数
extern	int		TerminateMidiHandle( HANDLEINFO *HandleInfo ) ;															// ＭＩＤＩハンドルの後始末を行う関数

extern	int		LoadMusicMemByMemImage_UseGParam( void *FileImage, int FileImageSize, int ASyncLoadFlag = FALSE ) ;		// LoadMusicMemByMemImage のグローバル変数にアクセスしないバージョン
extern	int		LoadMusicMem_UseGParam( const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;							// LoadMusicMem のグローバル変数にアクセスしないバージョン
extern	int		LoadMusicMemByResource_UseGParam( const wchar_t *ResourceName, const wchar_t *ResourceType, int ASyncLoadFlag = FALSE ) ;	// LoadMusicMemByResource のグローバル変数にアクセスしないバージョン
extern	int		LoadMusicMemByMemImage_Static( int MusicHandle, const void *FileImage, int FileImageSize, int ASyncThread ) ;		// LoadMusicMemByMemImage の実処理関数

extern	int		PauseSoundMemAll( int PauseFlag ) ;																		// 全ての音の一時停止状態を変更する
extern	int		PauseSoftSoundAll( int PauseFlag ) ;																	// 全てのソフトウエアサウンドの一時停止状態を変更する

extern	int		CopySoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;										// サウンドデータを別のサウンドデータにコピーする
extern	int		ConvertIntToFloatSoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;							// int型のサウンドデータから float型のサウンドデータをセットアップする
extern	int		ConvertFloatToIntSoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;							// float型のサウンドデータから int型のサウンドデータをセットアップする





// wchar_t版関数
extern	int		AddStreamSoundMemToFile_WCHAR_T(	const wchar_t *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;
extern	int		LoadSoundMem2_WCHAR_T(				const wchar_t *FileName1, const wchar_t *FileName2 ) ;

extern	int		LoadBGM_WCHAR_T(					const wchar_t *FileName ) ;

extern	int		LoadSoundMemBase_WCHAR_T(			const wchar_t *FileName, int BufferNum,      int UnionHandle = -1 ) ;
extern	int		LoadSoundMem_WCHAR_T(				const wchar_t *FileName, int BufferNum = 3 , int UnionHandle = -1 ) ;
extern	int		LoadSoundMemToBufNumSitei_WCHAR_T(	const wchar_t *FileName, int BufferNum ) ;
extern	int		LoadSoundMemByResource_WCHAR_T(		const wchar_t *ResourceName, const wchar_t *ResourceType, int BufferNum = 1 ) ;

extern	int		PlaySoundFile_WCHAR_T(				const wchar_t *FileName, int PlayType ) ;
extern	int		PlaySound_WCHAR_T(					const wchar_t *FileName, int PlayType ) ;

extern	int		LoadSoftSound_WCHAR_T(				const wchar_t *FileName ) ;

#ifndef DX_NON_SAVEFUNCTION
extern	int		SaveSoftSound_WCHAR_T(				int SoftSoundHandle, const wchar_t *FileName ) ;
#endif // DX_NON_SAVEFUNCTION

extern	int		LoadMusicMem_WCHAR_T(				const wchar_t *FileName ) ;
extern	int		LoadMusicMemByResource_WCHAR_T(		const wchar_t *ResourceName, const wchar_t *ResourceType ) ;

extern	int		PlayMusic_WCHAR_T(					const wchar_t *FileName, int PlayType ) ;

extern	int		PlayMusicByResource_WCHAR_T(		const wchar_t *ResourceName, const wchar_t *ResourceType, int PlayType ) ;

extern	int		GetMP3TagInfo_WCHAR_T(              const wchar_t *FileName, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle ) ;




//サウンドバッファ用
extern	int		SoundBuffer_Initialize(          SOUNDBUFFER *Buffer, DWORD Bytes, WAVEFORMATEX *Format, SOUNDBUFFER *Src, int UseGetCurrentPosition, int IsStream, int Is3DSound ) ;
extern	int		SoundBuffer_Duplicate(           SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound ) ;
extern	int		SoundBuffer_Terminate(           SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_CheckEnable(         SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_Play(                SOUNDBUFFER *Buffer, int Loop ) ;
extern	int		SoundBuffer_Stop(                SOUNDBUFFER *Buffer, int EffectStop = FALSE ) ;
extern	int		SoundBuffer_CheckPlay(           SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_Lock(                SOUNDBUFFER *Buffer, DWORD  WritePos, DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 ) ;
extern	int		SoundBuffer_Unlock(              SOUNDBUFFER *Buffer, void  *LockPos1, DWORD LockSize1, void  *LockPos2, DWORD LockSize2 ) ;
extern	int		SoundBuffer_SetFrequency(        SOUNDBUFFER *Buffer, DWORD Frequency ) ;
extern	int		SoundBuffer_GetFrequency(        SOUNDBUFFER *Buffer, DWORD * Frequency ) ;
extern	int		SoundBuffer_SetPan(              SOUNDBUFFER *Buffer, LONG Pan ) ;
extern	int		SoundBuffer_GetPan(              SOUNDBUFFER *Buffer, LPLONG Pan ) ;
extern	int		SoundBuffer_RefreshVolume(       SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_SetVolumeAll(        SOUNDBUFFER *Buffer, LONG Volume ) ;
extern	int		SoundBuffer_SetVolume(           SOUNDBUFFER *Buffer, int Channel, LONG Volume ) ;
extern	int		SoundBuffer_GetVolume(           SOUNDBUFFER *Buffer, int Channel, LPLONG Volume ) ;
extern	int		SoundBuffer_GetCurrentPosition(  SOUNDBUFFER *Buffer, DWORD * PlayPos, DWORD * WritePos ) ;
extern	int		SoundBuffer_SetCurrentPosition(  SOUNDBUFFER *Buffer, DWORD NewPos ) ;
extern	int		SoundBuffer_CycleProcess(        SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_FrameProcess(        SOUNDBUFFER *Buffer, int Sample, short *DestBuf ) ;
extern	int		SoundBuffer_Set3DPosition(       SOUNDBUFFER *Buffer, VECTOR *Position ) ;
extern	int		SoundBuffer_Set3DRadius(         SOUNDBUFFER *Buffer, float Radius ) ;
extern	int		SoundBuffer_Set3DInnerRadius(    SOUNDBUFFER *Buffer, float Radius ) ;
extern	int		SoundBuffer_Set3DVelocity(       SOUNDBUFFER *Buffer, VECTOR *Velocity ) ;
extern	int		SoundBuffer_Set3DFrontPosition(  SOUNDBUFFER *Buffer, VECTOR *FrontPosition, VECTOR *UpVector ) ;
extern	int		SoundBuffer_Set3DConeAngle(      SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle ) ;
extern	int		SoundBuffer_Set3DConeVolume(     SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume ) ;
extern	int		SoundBuffer_Refresh3DSoundParam( SOUNDBUFFER *Buffer, int AlwaysFlag = FALSE ) ;
extern	int		SoundBuffer_SetReverbParam(      SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param ) ;
extern	int		SoundBuffer_SetPresetReverbParam( SOUNDBUFFER *Buffer, int PresetNo ) ;


// 波形データ用
extern	WAVEDATA *	AllocWaveData( int Size, int UseDoubleSizeBuffer = FALSE ) ;
extern	int			ReleaseWaveData( WAVEDATA *Data ) ;
extern	WAVEDATA *	DuplicateWaveData( WAVEDATA *Data ) ;







// 環境依存関数
extern	int		InitializeSoundSystem_PF_Timing0( void ) ;											// サウンドシステムを初期化する関数の環境依存処理を行う関数( 実行箇所区別０ )
extern	int		InitializeSoundSystem_PF_Timing1( void ) ;											// サウンドシステムを初期化する関数の環境依存処理を行う関数( 実行箇所区別１ )

extern	int		TerminateSoundSystem_PF_Timing0( void ) ;											// サウンドシステムの後始末をする関数の環境依存処理を行う関数( 実行箇所区別０ )
extern	int		TerminateSoundSystem_PF_Timing1( void ) ;											// サウンドシステムの後始末をする関数の環境依存処理を行う関数( 実行箇所区別１ )

extern	int		CheckSoundSystem_Initialize_PF( void ) ;											// サウンドシステムの初期化チェックの環境依存処理を行う関数( TRUE:初期化されている  FALSE:初期化されていない )

extern	int		TerminateMidiHandle_PF( MIDIHANDLEDATA *MusicData ) ;								// ＭＩＤＩハンドルの後始末を行う関数の環境依存処理

extern	int		Get3DPresetReverbParamSoundMem_PF( SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ ) ;		// プリセットの３Ｄサウンド用のリバーブパラメータを取得する処理の環境依存処理を行う関数
extern	int		Set3DSoundListenerPosAndFrontPosAndUpVec_PF( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector ) ;							// ３Ｄサウンドのリスナーの位置とリスナーの前方位置とリスナーの上方向位置を設定する処理の環境依存処理を行う関数
extern	int		Set3DSoundListenerVelocity_PF( VECTOR Velocity ) ;																				// ３Ｄサウンドのリスナーの移動速度を設定する処理の環境依存処理を行う関数
extern	int		Set3DSoundListenerConeAngle_PF( float InnerAngle, float OuterAngle ) ;															// ３Ｄサウンドのリスナーの可聴角度範囲を設定する処理の環境依存処理を行う関数
extern	int		Set3DSoundListenerConeVolume_PF( float InnerAngleVolume, float OuterAngleVolume ) ;												// ３Ｄサウンドのリスナーの可聴角度範囲の音量倍率を設定する処理の環境依存処理を行う関数

extern	int		LoadMusicMemByMemImage_Static_PF( MIDIHANDLEDATA *MusicData, int ASyncThread ) ;												// LoadMusicMemByMemImage の実処理関数の環境依存処理を行う関数
extern	int		PlayMusicMem_PF( MIDIHANDLEDATA *MusicData, int PlayType ) ;																	// 読み込んだＭＩＤＩデータの演奏を開始する処理の環境依存処理を行う関数
extern	int		StopMusicMem_PF( MIDIHANDLEDATA *MusicData ) ;																					// ＭＩＤＩデータの演奏を停止する処理の環境依存処理を行う
extern	int		CheckMusicMem_PF( MIDIHANDLEDATA *MusicData ) ;																					// ＭＩＤＩデータが演奏中かどうかを取得する( TRUE:演奏中  FALSE:停止中 )処理の環境依存処理を行う関数
extern	int		ProcessMusicMem_PF( MIDIHANDLEDATA *MusicData ) ;																				// ＭＩＤＩデータの周期的処理の環境依存処理を行う関数
extern	int		GetMusicMemPosition_PF( MIDIHANDLEDATA *MusicData ) ;																			// ＭＩＤＩデータの現在の再生位置を取得する処理の環境依存処理を行う関数
extern	int		SetVolumeMusic_PF( int Volume ) ;																								// ＭＩＤＩの再生音量をセットする処理の環境依存処理を行う関数
extern	int		GetMusicPosition_PF( void ) ;																									// ＭＩＤＩの現在の再生位置を取得する処理の環境依存処理を行う関数




extern	int		SoundBuffer_Initialize_Timing0_PF(	SOUNDBUFFER *Buffer, DWORD Bytes, WAVEFORMATEX *Format, SOUNDBUFFER *Src, int Is3DSound ) ;
extern	int		SoundBuffer_Initialize_Timing1_PF(	SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound ) ;
extern	int		SoundBuffer_Terminate_PF(           SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_CheckEnable_PF(         SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_Play_PF(                SOUNDBUFFER *Buffer, int Loop ) ;
extern	int		SoundBuffer_Stop_PF(				SOUNDBUFFER *Buffer, int EffectStop ) ;
extern	int		SoundBuffer_CheckPlay_PF(           SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_Lock_PF(                SOUNDBUFFER *Buffer, DWORD WritePos , DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 ) ;
extern	int		SoundBuffer_Unlock_PF(              SOUNDBUFFER *Buffer, void *LockPos1, DWORD LockSize1, void *LockPos2, DWORD LockSize2 ) ;
extern	int		SoundBuffer_SetFrequency_PF(        SOUNDBUFFER *Buffer, DWORD Frequency ) ;
extern	int		SoundBuffer_GetFrequency_PF(        SOUNDBUFFER *Buffer, DWORD * Frequency ) ;
extern	int		SoundBuffer_RefreshVolume_PF(		SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_GetCurrentPosition_PF(	SOUNDBUFFER *Buffer, DWORD * PlayPos, DWORD * WritePos ) ;
extern	int		SoundBuffer_SetCurrentPosition_PF(	SOUNDBUFFER *Buffer, DWORD NewPos ) ;
extern	int		SoundBuffer_CycleProcess_PF(		SOUNDBUFFER *Buffer ) ;
extern	int		SoundBuffer_Set3DPosition_PF(		SOUNDBUFFER *Buffer, VECTOR *Position ) ;
extern	int		SoundBuffer_Set3DRadius_PF(			SOUNDBUFFER *Buffer, float Radius ) ;
extern	int		SoundBuffer_Set3DInnerRadius_PF(	SOUNDBUFFER *Buffer, float Radius ) ;
extern	int		SoundBuffer_Set3DVelocity_PF(		SOUNDBUFFER *Buffer, VECTOR *Velocity ) ;
extern	int		SoundBuffer_Set3DFrontPosition_PF(	SOUNDBUFFER *Buffer, VECTOR *FrontPosition, VECTOR *UpVector ) ;
extern	int		SoundBuffer_Set3DConeAngle_PF(		SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle ) ;
extern	int		SoundBuffer_Set3DConeVolume_PF(		SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume ) ;
extern	int		SoundBuffer_Refresh3DSoundParam_PF(	SOUNDBUFFER *Buffer, int AlwaysFlag ) ;
extern	int		SoundBuffer_SetReverbParam_PF(		SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param ) ;
extern	int		SoundBuffer_SetPresetReverbParam_PF( SOUNDBUFFER *Buffer, int PresetNo ) ;








#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOUND

#endif // __DXSOUND_H__
