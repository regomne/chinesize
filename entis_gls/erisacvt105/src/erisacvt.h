
/*****************************************************************************
                          ERISA 画像コンバーター
 *****************************************************************************/


#include <gls.h>
#include <linkgls.h>

#include <psdfile.h>
#include <linkpsdf.h>

#include <stdio.h>

#include "resource.h"


//////////////////////////////////////////////////////////////////////////////
// XML パーサー
//////////////////////////////////////////////////////////////////////////////

class	XMLParser	: public	EDescription
{
public:
	int	m_nErrorCount ;

	// 構築関数
	XMLParser( void ) : m_nErrorCount(0) { }
	// 副タグを生成
	virtual EDescription * CreateDescription( void ) ;
	// エラー出力
	virtual ESLError OutputError( const char * pszErrMsg ) ;
	// 警告出力
	virtual ESLError OutputWarning( const char * pszErrMsg ) ;
} ;


//////////////////////////////////////////////////////////////////////////////
// 詞葉スクリプト　コンパイラ
//////////////////////////////////////////////////////////////////////////////

class	ECSXCompiler	: public	ECSCompiler
{
public:
	EString				m_strBaseDir ;
	EString				m_strExeDir ;
	EWideString			m_wstrLine ;

public:
	// 構築関数
	ECSXCompiler( void ) ;
	// ベースディレクトリを設定する
	void SetBaseDirectory( const char * pszBaseDir ) ;
	// １行コンパイルする
	virtual ESLError CompileScriptLine
		( ECSSourceStream & cssLine,
			int nLineNum, const char * pszFilePath = NULL ) ;
	// 指定のパスのファイルを開く
	virtual ESLFileObject * OpenScriptFile( const char * pszFilePath ) ;
	// エラーを出力する
	virtual ESLError OutputError
		( const char * pszErrMsg,
			const char * pszFilePath = NULL, int nLineNum = 0 ) ;
	// 警告を出力する
	virtual ESLError OutputWarning
		( const char * pszErrMsg,
			const char * pszFilePath = NULL, int nLineNum = 0 ) ;

} ;


//////////////////////////////////////////////////////////////////////////////
// 画像バッファ
//////////////////////////////////////////////////////////////////////////////

class	ECSImage	: public	ECSResource
{
public:
	// 構築関数
	ECSImage( void ) ;
	// 消滅関数
	virtual ~ECSImage( void ) ;
	// クラス情報
	DECLARE_CLASS_INFO( ECSImage, ECSResource )

public:
	EGLMediaLoader	m_imgBuf ;			// 画像データ
	ECSInteger		m_intIndex ;		// フレーム・レイヤー番号
	ECSString		m_strName ;			// レイヤー名
	ECSString		m_strFileName ;		// ファイル名

public:
	// オブジェクトの型名を取得する
	virtual const wchar_t * GetTypeName( void ) const ;
	// オブジェクトを複製
	virtual ECSObject * Duplicate( void ) ;
	// オブジェクトを代入
	virtual ESLError Move( ECSContext & context, ECSObject * obj ) ;
	// メンバ変数インデックス取得
	virtual ESLError GetVariableIndex( int & nIndex, ECSObject & obj ) ;
	// メンバ変数取得
	virtual ECSObject * GetVariableAt( int nIndex ) ;
	// メンバ変数設定
	virtual ECSObject * SetVariableAt( int nIndex, ECSObject * obj ) ;

} ;


//////////////////////////////////////////////////////////////////////////////
// 画像コンテキストバッファ
//////////////////////////////////////////////////////////////////////////////

class	ECSImageContext	: public	ECSArray
{
public:
	// 構築関数
	ECSImageContext( void ) ;
	// 消滅関数
	virtual ~ECSImageContext( void ) ;
	// クラス情報
	DECLARE_CLASS_INFO( ECSImageContext, ECSArray )

public:
	EGLMediaLoader	m_imgBuf ;			// 出力用バッファ
	ENumArray<UINT>	m_lstSequence ;		// シーケンスリスト
	ECSInteger		m_intDuration ;		// アニメーション時間
	ECSStructure	m_sizeImage ;		// 画像サイズ
	ECSStructure	m_ptHotSpot ;		// ホットスポット
	ECSString		m_strFileName ;		// ファイル名

	ECSStrTagArray	m_staFuncName ;

	enum	CutOperationFlag
	{
		cofKeepHotspot	= 0x01,
		cofRemoveAlpha	= 0x02,
		cofAutoAlpha	= 0x04,
	} ;

	typedef	ESLError (ECSImageContext::*PFUNC_CALL)
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	static const wchar_t *	pwszFuncName[7] ;
	static const PFUNC_CALL	m_pfnCallFunc[6] ;

public:
	// 配列長取得
	int GetLength( void ) const
		{
			return	m_varArray.GetSize() ;
		}
	// 画像データ取得
	ECSImage * GetImageAt( int nIndex ) const
		{
			return	ESLTypeCast<ECSImage>( m_varArray.GetAt( nIndex ) ) ;
		}
	// 画像コンテキスト消去
	void DeleteContext( void ) ;

public:
	// 画像の読み込み
	ESLError LoadImageFile
		( const wchar_t * pwszFileName, int fBlendMode, int fMaskMode ) ;
	// 画像の切り出し
	int CutImage
		( int nAlign, int nMargin,
			int nThreshold, int nFlags, EGL_IMAGE_RECT & irClip ) ;
	// 画像の配列
	int ArrangeImage( bool fWayVert, bool fPutRight, int nAlign ) ;
	// アニメーション作成
	int MakeAnimation( const wchar_t * pwszSeq, int nDuration ) ;
	// マージする
	void MergeContext
		( ECSImageContext & imgctx,
			int iFirst = 0, int nCount = -1,
			const wchar_t * pwszName = NULL ) ;

public:
	// オブジェクトの型名を取得する
	virtual const wchar_t * GetTypeName( void ) const ;
	// オブジェクトを複製
	virtual ECSObject * Duplicate( void ) ;
	// オブジェクトを代入
	virtual ESLError Move( ECSContext & context, ECSObject * obj ) ;
	// メンバ変数インデックス取得
	virtual ESLError GetVariableIndex( int & nIndex, ECSObject & obj ) ;
	// メンバ変数取得
	virtual ECSObject * GetVariableAt( int nIndex ) ;
	// メンバ変数設定
	virtual ECSObject * SetVariableAt( int nIndex, ECSObject * obj ) ;
	// メンバ関数インデックス取得
	virtual ESLError GetFunction
		( ECSContext & context, int & nIndex, const wchar_t * pwszName ) ;
	// メンバ関数呼び出し
	virtual ESLError CallFunction
		( ECSContext & context,
			int nIndex, EObjArray<ECSObject> & lstArg ) ;

public:
	// メンバ関数
	ESLError Call_load
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	ESLError Call_save
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	ESLError Call_cut
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	ESLError Call_arrange
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	ESLError Call_animation
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;
	ESLError Call_merge
		( ECSContext & context, EObjArray<ECSObject> & lstArg ) ;

} ;


//////////////////////////////////////////////////////////////////////////////
// アプリケーションクラス
//////////////////////////////////////////////////////////////////////////////

class	EConverterApp	: public	ECSContext
{
public:
	// 構築関数
	EConverterApp( void ) ;
	// 消滅関数
	virtual ~EConverterApp( void ) ;
	// クラス情報
	DECLARE_CLASS_INFO( EConverterApp, ECSContext )

public:
	// 引数
	enum	Options
	{
		optNologo	= 0x0001,
		optUsage	= 0x0002,
		optHelp		= 0x0004,
		optIndirect	= 0x0008,
		optInform	= 0x0010
	} ;
	DWORD			m_dwFlags ;
	EString			m_strErrMsg ;
	EString			m_strSrcFile ;
	EString			m_strDstFile ;
	EString			m_strReportFile ;
	EString			m_strOutputFile ;

	XMLParser		m_xmlCvt ;
	EDescription *	m_pxmlErisa ;

	int				m_nErrorCount ;
	XMLParser		m_xmlReport ;

	EStreamBuffer	m_bufOutput ;

	// コンバーターの環境変数
	enum	FormatType
	{
		ftInvalid = -1,
		ftERI, ftERINA, ftERISA,
		ftBMP, ftTIFF, ftPNG, ftJPEG,
		ftMax
	} ;
	int				m_nFmtType ;			// 出力フォーマット
	int				m_nCmprMode ;			// 圧縮モード
	EWideString		m_wstrDstDir ;			// 出力ディレクトリ
	EWideString		m_wstrSrcDir ;			// 入力ディレクトリ
	int				m_nPutAlign ;			// 画像を配置する際のグリッド
	int				m_nCutAlign ;			// 画像を切り出す際のグリッド
	int				m_nCutMargin ;			// 画像を切り抜く際のマージン
	int				m_nCutThreshold ;		// 切り抜き閾値
	int				m_fKeepHotspot ;		// ホットスポットを維持するか？
	int				m_fLayerBlend ;			// レイヤーを結合するか？
	int				m_fLayerMask ;			// マスク処理の方法

	static const wchar_t *	m_pwszFormatMime[ftMax] ;
	static const wchar_t *	m_pwszFormatExt[ftMax] ;

	// ファイル名列挙オブジェクト
	class	EnumFileName
	{
	public:
		enum
		{
			typeNothing,
			typeWin32,
			typeSeqNum,
			typeScript
		}						m_type ;
		EConverterApp *			m_app ;
		HANDLE					m_hFind ;
		WIN32_FIND_DATA			m_wfd ;
		EString					m_strFileName ;
		EWideString				m_wstrBaseDir ;
		int						m_iFirst ;
		int						m_iEnd ;
		int						m_nStep ;
		int						m_iCurrent ;
		EObjArray<ECSObject>	m_lstArg ;
	public:
		// 構築関数
		EnumFileName( EConverterApp * app ) ;
		EnumFileName
			( EConverterApp * app,
				const wchar_t * pwszBaseDir, const wchar_t * pwszFileName ) ;
		// 消滅関数
		~EnumFileName( void ) ;
		// ファイル名設定
		void SetEnumFiles
			( const wchar_t * pwszBaseDir, const wchar_t * pwszFileName ) ;
		// ファイル名取得
		ESLError GetNextFileName
			( EWideString & wstrFileName, EString & strErrMsg ) ;
	} ;

	// スクリプト環境
	ECSExecutionImage	m_csxi ;			// 実行イメージ

public:
	// 拡張型オブジェクトを生成する
	virtual ECSObject * CreateExtendedObject
		( CSVariableType csvtType,
			const wchar_t * pwszType, DWORD * pdwFuncAddr = NULL ) ;

public:
	// 引数の解析
	void ParseCmdLine( int argc, char * argv[] ) ;
	// 見出し・エラー・書式などの表示
	int PrintMessage( void ) ;
	// 指定されたりソースを標準出力へ出力
	void PrintResource( LPCTSTR lpName ) ;
	// 実行
	int Perform( void ) ;

public:
	// MIME から FormatType 列挙子への変換
	static int FormatTypeFromMIME( const wchar_t * pwszMIME ) ;
	// 対応するフォーマットのデフォルトの拡張子を取得
	static const wchar_t * GetDefaultFileExt( int fFmtType ) ;

public:
	// 画像ファイル情報を表示する
	int InformFiles( void ) ;
	// インダイレクトファイルを読み込む
	int LoadIndirectFile( void ) ;
	// 起動引数から処理手続きを作成する
	int MakeProcess( void ) ;
	// 処理を実行する
	int BeginProcess( void ) ;

public:
	// 環境設定を行う
	void SetEnvironment( EDescription * pdscEnv ) ;
	// ファイル処理を行う
	void ProcessFiles( EDescription * pdscFile ) ;
	// ファイル処理
	void ProcessFileTags
		( ECSImageContext & imgctx, EDescription * pdscFile ) ;
	// 画像を選択
	void ProcessSelectImage
		( ECSImageContext & imgctx, EDescription * pdscSelect ) ;
	// ファイルを読み込む
	void ProcessImages
		( ECSImageContext & imgctx, EDescription * pdscImage ) ;
	// 連番ファイル出力
	void WriteSequenceFiles
		( ECSImageContext & imgctx, const wchar_t * pwszDstFile ) ;
	// 出力ファイル名を整形する
	EWideString NormalizeDestinationFileName
		( const wchar_t * pwszDst, const wchar_t * pwszSrc ) ;

public:
	// 画像を読み込む
	ESLError LoadImageFile
		( ECSImageContext & imgctx,
			const wchar_t * pwszSrc,
			const wchar_t * pwszHotSpot, int nIndex ) ;
	// 画像をファイルに書き出す
	ESLError SaveImageContext
		( ECSImageContext & imgctx, const wchar_t * pwszDst, int nFmtType ) ;
	ESLError SaveImageFile
		( EGLMediaLoader & imgBuf, const wchar_t * pwszDst, int nFmtType ) ;

} ;

