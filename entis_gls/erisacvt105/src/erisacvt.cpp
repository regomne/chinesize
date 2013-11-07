
/*****************************************************************************
                          ERISA 画像コンバーター
 *****************************************************************************/


#include "erisacvt.h"


//////////////////////////////////////////////////////////////////////////////
// エントリポイント
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char * argv[] )
{
	int	result = 0 ;
	::glsInitializeLibrary( ) ;
	EGLMediaLoader::Initialize( ) ;
	ECotophaScript::Initialize( ) ;
	{
		EConverterApp	app ;
		app.ParseCmdLine( argc, argv ) ;
		result = app.PrintMessage( ) ;
		if ( result == 0 )
		{
			result = app.Perform( ) ;
		}
	}
	ECotophaScript::Release( ) ;
	EGLMediaLoader::Close( ) ;
	::glsCloseLibrary( ) ;
	return	result ;
}


//////////////////////////////////////////////////////////////////////////////
// XML パーサー
//////////////////////////////////////////////////////////////////////////////

// 副タグを生成
//////////////////////////////////////////////////////////////////////////////
EDescription * XMLParser::CreateDescription( void )
{
	return	new XMLParser ;
}

// エラー出力
//////////////////////////////////////////////////////////////////////////////
ESLError XMLParser::OutputError( const char * pszErrMsg )
{
	m_nErrorCount ++ ;
	printf( "XML 構文エラー：%s\n", pszErrMsg ) ;
	return	EDescription::OutputError( pszErrMsg ) ;
}

// 警告出力
//////////////////////////////////////////////////////////////////////////////
ESLError XMLParser::OutputWarning( const char * pszErrMsg )
{
	printf( "XML 構文エラー：%s\n", pszErrMsg ) ;
	return	EDescription::OutputWarning( pszErrMsg ) ;
}


//////////////////////////////////////////////////////////////////////////////
// 詞葉スクリプト　コンパイラ
//////////////////////////////////////////////////////////////////////////////

// 構築関数
//////////////////////////////////////////////////////////////////////////////
ECSXCompiler::ECSXCompiler( void )
{
	HMODULE	hModule = ::GetModuleHandle( NULL ) ;
	EString	strModulePath ;
	::GetModuleFileName( hModule, strModulePath.GetBuffer(0x400), 0x400 ) ;
	strModulePath.ReleaseBuffer( ) ;
	m_strExeDir = strModulePath.GetFileDirectoryPart( ) ;
}

// ベースディレクトリを設定する
//////////////////////////////////////////////////////////////////////////////
void ECSXCompiler::SetBaseDirectory( const char * pszBaseDir )
{
	m_strBaseDir = pszBaseDir ;
}

// １行コンパイルする
//////////////////////////////////////////////////////////////////////////////
ESLError ECSXCompiler::CompileScriptLine
	( ECSSourceStream & cssLine, int nLineNum, const char * pszFilePath )
{
	m_wstrLine = cssLine ;
	return	ECSCompiler::CompileScriptLine( cssLine, nLineNum, pszFilePath ) ;
}

// 指定のパスのファイルを開く
//////////////////////////////////////////////////////////////////////////////
ESLFileObject * ECSXCompiler::OpenScriptFile( const char * pszFilePath )
{
	EString	strFilePath = pszFilePath ;
	ERawFile *	pfile = new ERawFile ;
	long int	nOpenFlags =
		ESLFileObject::modeRead | ESLFileObject::shareRead ;
	if ( pfile->Open( strFilePath, nOpenFlags ) )
	{
		if ( (strFilePath[0] == '\\') || (strFilePath.Find( ':' ) >= 0) )
		{
			delete	pfile ;
			return	NULL ;
		}
		if ( pfile->Open( m_strBaseDir + strFilePath, nOpenFlags ) )
		{
			if ( pfile->Open( m_strExeDir + strFilePath, nOpenFlags ) )
			{
				delete	pfile ;
				return	NULL ;
			}
		}
	}
	return	pfile ;
}

// エラーを出力する
//////////////////////////////////////////////////////////////////////////////
ESLError ECSXCompiler::OutputError
	( const char * pszErrMsg, const char * pszFilePath, int nLineNum )
{
	if ( (pszFilePath != NULL) && (nLineNum > 0) )
	{
		printf( "%s (%d) : エラー : %s\n", pszFilePath, nLineNum, pszErrMsg ) ;
	}
	else
	{
		printf( "以下の行 : エラー : %s\n", pszErrMsg ) ;
		printf( EString(m_wstrLine) + "\n" ) ;
	}
	return	ECSCompiler::OutputError( pszErrMsg, pszFilePath, nLineNum ) ;
}

// 警告を出力する
//////////////////////////////////////////////////////////////////////////////
ESLError ECSXCompiler::OutputWarning
	( const char * pszErrMsg, const char * pszFilePath, int nLineNum )
{
	if ( (pszFilePath != NULL) && (nLineNum > 0) )
	{
		printf( "%s (%d) : 警告 : %s\n", pszFilePath, nLineNum, pszErrMsg ) ;
	}
	else
	{
		printf( "以下の行 : 警告 : %s\n", pszErrMsg ) ;
		printf( EString(m_wstrLine) + "\n" ) ;
	}
	return	ECSCompiler::OutputWarning( pszErrMsg, pszFilePath, nLineNum ) ;
}


//////////////////////////////////////////////////////////////////////////////
// アプリケーションクラス
//////////////////////////////////////////////////////////////////////////////

// クラス情報
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_INFO( EConverterApp, ECSContext )

const wchar_t *	EConverterApp::m_pwszFormatMime[EConverterApp::ftMax] =
{
	L"image/x-eri", L"image/x-erina", L"image/x-erisa",
	L"image/bmp", L"image/tiff", L"image/png", L"image/jpeg"
} ;
const wchar_t *	EConverterApp::m_pwszFormatExt[EConverterApp::ftMax] =
{
	L"eri", L"eri", L"eri", L"bmp", L"tif", L"png", L"jpg"
} ;

// 構築関数
//////////////////////////////////////////////////////////////////////////////
EConverterApp::EConverterApp( void )
{
	m_dwFlags = 0 ;
	//
	m_nFmtType = ftERI ;
	m_nCmprMode = 0 ;
	m_nPutAlign = 1 ;
	m_nCutAlign = 0 ;
	m_nCutMargin = 0 ;
	m_nCutThreshold = 0 ;
	m_fKeepHotspot = 1 ;
	m_fLayerBlend = 0 ;
	m_fLayerMask = 0 ;
}

// 消滅関数
//////////////////////////////////////////////////////////////////////////////
EConverterApp::~EConverterApp( void )
{
}

// EConverterApp::EnumFileName 構築
//////////////////////////////////////////////////////////////////////////////
EConverterApp::EnumFileName::EnumFileName( EConverterApp * app )
	: m_app( app ), m_type( typeNothing ), m_hFind( INVALID_HANDLE_VALUE )
{
}

EConverterApp::EnumFileName::EnumFileName
	( EConverterApp * app,
		const wchar_t * pwszBaseDir, const wchar_t * pwszFileName )
	: m_app( app ), m_hFind( INVALID_HANDLE_VALUE )
{
	SetEnumFiles( pwszBaseDir, pwszFileName ) ;
}

// 消滅関数
//////////////////////////////////////////////////////////////////////////////
EConverterApp::EnumFileName::~EnumFileName( void )
{
	if ( m_hFind != INVALID_HANDLE_VALUE )
	{
		::FindClose( m_hFind ) ;
	}
}

// ファイル名設定
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::EnumFileName::SetEnumFiles
	( const wchar_t * pwszBaseDir, const wchar_t * pwszFileName )
{
	EStreamWideString		swsFileName = pwszFileName ;
	EStreamWideString		swsUsage =
					L"(%s)\\: (<0-9>*) [\\- (<0-9>*)] [\\, (%n)] \\" ;
	EObjArray<EWideString>	lstParam ;
	EString					strErrMsg ;
	if ( swsFileName[0] == L'$' )
	{
		m_type = typeScript ;
		m_strFileName = swsFileName.Middle(1) ;
		m_iCurrent = 0 ;
	}
	else if ( !swsFileName.IsMatchUsage( swsUsage, strErrMsg, &lstParam ) )
	{
		m_type = typeSeqNum ;
		m_strFileName = lstParam[0] ;
		m_iFirst = lstParam[1].AsInteger( ) ;
		if ( lstParam[2].IsEmpty() )
		{
			m_iEnd = -1 ;
		}
		else
		{
			m_iEnd = lstParam[2].AsInteger( ) ;
		}
		if ( lstParam[3].IsEmpty() )
		{
			m_nStep = 1 ;
			if ( (m_iEnd >= 0) && (m_iEnd < m_iFirst) )
			{
				m_nStep = -1 ;
			}
		}
		else
		{
			m_nStep = lstParam[3].AsInteger( ) ;
		}
		m_iCurrent = m_iFirst ;
	}
	else
	{
		m_type = typeWin32 ;
		m_iCurrent = 0 ;
		m_strFileName = swsFileName ;
	}
	//
	m_wstrBaseDir = pwszBaseDir ;
	if ( (m_wstrBaseDir.GetLength() > 1)
		&& (m_wstrBaseDir.Right(1) != L"\\") )
	{
		m_wstrBaseDir += L'\\' ;
	}
	if ( m_type != typeScript )
	{
		if ( (m_strFileName.GetAt(0) != '\\')
			&& (m_strFileName.Find( ':' ) < 0) )
		{
			EString	strFileName = m_wstrBaseDir ;
			strFileName += m_strFileName ;
			m_strFileName = strFileName ;
		}
	}
}

// ファイル名取得
//////////////////////////////////////////////////////////////////////////////
ESLError EConverterApp::EnumFileName::GetNextFileName
	( EWideString & wstrFileName, EString & strErrMsg )
{
	strErrMsg.FreeString( ) ;
	//
	if ( m_type == typeWin32 )
	{
		//
		// ファイル名・ワイルドカード
		//
		for ( ; ; )
		{
			if ( (m_iCurrent ++) == 0 )
			{
				m_hFind = ::FindFirstFile( m_strFileName, &m_wfd ) ;
				if ( m_hFind != INVALID_HANDLE_VALUE )
				{
					if ( !(m_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						wstrFileName = m_strFileName.GetFileDirectoryPart() ;
						wstrFileName += EWideString( m_wfd.cFileName ) ;
						return	eslErrSuccess ;
					}
				}
				else
				{
					strErrMsg = m_strFileName + " ファイルが見つかりません。" ;
					break ;
				}
			}
			else if ( m_hFind != INVALID_HANDLE_VALUE )
			{
				if ( ::FindNextFile( m_hFind, &m_wfd ) )
				{
					if ( !(m_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						wstrFileName = m_strFileName.GetFileDirectoryPart() ;
						wstrFileName += EWideString( m_wfd.cFileName ) ;
						return	eslErrSuccess ;
					}
				}
				else
				{
					break ;
				}
			}
		}
	}
	else if ( m_type == typeSeqNum )
	{
		//
		// 連番ファイル
		//
		if ( (m_iCurrent * m_nStep <= m_iEnd * m_nStep) || (m_iEnd == -1) )
		{
			int			nNum = m_iCurrent ;
			wstrFileName = m_strFileName ;
			//
			for ( int i = wstrFileName.GetLength() - 1; i >= 0; i -- )
			{
				if ( wstrFileName[i] == L'%' )
				{
					wstrFileName.SetAt( i, (wchar_t) (L'0' + (nNum % 10)) ) ;
					nNum /= 10 ;
				}
			}
			//
			m_iCurrent += m_nStep ;
			return	eslErrSuccess ;
		}
	}
	else if ( m_type == typeScript )
	{
		//
		// スクリプト
		//
		DWORD *	pdwFuncAddr =
			m_app->m_csxi.GetFunctionAddress( EWideString(m_strFileName) ) ;
		if ( pdwFuncAddr != NULL )
		{
			ESLError	err = m_app->CallFunction( *pdwFuncAddr, m_lstArg ) ;
			if ( err )
			{
				strErrMsg = GetESLErrorMsg( err ) ;
			}
			else
			{
				ECSString *	pstrResult =
					ESLTypeCast<ECSString>( m_app->GetReturnValue() ) ;
				if ( pstrResult != NULL )
				{
					if ( pstrResult->m_varStr.IsEmpty() )
					{
						return	eslErrGeneral ;
					}
					wstrFileName = L"" ;
					if ( (pstrResult->m_varStr.GetAt(0) != L'\\')
						&& (pstrResult->m_varStr.Find( L':' ) < 0) )
					{
						wstrFileName = m_wstrBaseDir ;
					}
					wstrFileName += pstrResult->m_varStr ;
					return	eslErrSuccess ;
				}
				strErrMsg = m_strFileName
					+ "関数の返り値が String ではありません。" ;
			}
		}
		else
		{
			strErrMsg = m_strFileName + " 関数が見つかりません。" ;
		}
	}
	//
	return	eslErrGeneral ;
}

// 引数の解析
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::ParseCmdLine( int argc, char * argv[] )
{
	int	i ;
	for ( i = 1; i < argc; i ++ )
	{
		if ( argv[i][0] != '/' )
		{
			break ;
		}
		EString	strArg = argv[i] + 1 ;
		if ( strArg == "l" )
		{
			if ( i + 1 >= argc )
			{
				m_strErrMsg = "変換処理ファイルが指定されていません。\n" ;
				return ;
			}
			m_strSrcFile = argv[++ i] ;
			m_dwFlags |= optIndirect ;
		}
		else if ( strArg == "r" )
		{
			if ( i + 1 >= argc )
			{
				m_strErrMsg = "レポートファイルが指定されていません。\n" ;
				return ;
			}
			m_strReportFile = argv[++ i] ;
		}
		else if ( strArg == "o" )
		{
			if ( i + 1 >= argc )
			{
				m_strErrMsg = "アウトプットファイルが指定されていません。\n" ;
				return ;
			}
			m_strOutputFile = argv[++ i] ;
		}
		else if ( strArg == "i" )
		{
			m_dwFlags |= optInform ;
		}
		else if ( strArg == "nologo" )
		{
			m_dwFlags |= optNologo ;
		}
		else if ( strArg == "?" )
		{
			m_dwFlags |= optUsage ;
		}
		else if ( strArg == "help" )
		{
			m_dwFlags |= optHelp ;
		}
		else if ( strArg == "clip" )
		{
			m_nCutAlign = 1 ;
		}
		else if ( strArg == "eri" )
		{
			m_nFmtType = ftERI ;
		}
		else if ( strArg == "erina" )
		{
			m_nFmtType = ftERINA ;
		}
		else if ( strArg == "erisa" )
		{
			m_nFmtType = ftERISA ;
		}
		else if ( strArg.Left(2) == "cp" )
		{
			bool	fError ;
			m_nCmprMode = strArg.Middle(2).AsInteger( false, &fError ) ;
			if ( (m_nCmprMode > 3) || fError )
			{
				m_nCmprMode = 0 ;
				m_strErrMsg = argv[i] ;
				m_strErrMsg += " は不正な圧縮モードの指定です。\n" ;
				return ;
			}
		}
		else if ( strArg.Left(5) == "mime:" )
		{
			m_nFmtType =
				FormatTypeFromMIME( EWideString( strArg.Middle(5) ) ) ;
			if ( m_nFmtType == ftInvalid )
			{
				m_strErrMsg = strArg.Middle(5) +
					" は対応していない出力フォーマットです。\n" ;
			}
		}
		else
		{
			m_strErrMsg = argv[i] ;
			m_strErrMsg += " は定義されていないオプションです。\n" ;
			m_strErrMsg += "/? オプションで書式を確認してください。\n" ;
			return ;
		}
	}
	if ( i >= argc )
	{
		if ( m_strSrcFile.IsEmpty() && !(m_dwFlags & (optUsage | optHelp)) )
		{
			m_strErrMsg = "入力ファイルが指定されていません。\n" ;
			m_strErrMsg += "/? オプションで書式を確認してください。\n" ;
		}
		return ;
	}
	if ( (m_dwFlags & optIndirect) || !m_strSrcFile.IsEmpty() )
	{
		m_strErrMsg = "書式に矛盾があります。\n" ;
		return ;
	}
	m_strSrcFile = argv[i] ;
	m_fLayerBlend = 1 ;
	//
	if ( i + 1 < argc )
	{
		m_strDstFile = argv[i + 1] ;
	}
	//
	return ;
}

// 見出し・エラー・書式などの表示
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::PrintMessage( void )
{
	static const char	szTitle[] =
		"ERISA image converter version 1.05\n"
		"Copyright (C) 2004-2005 Leshade Entis. All rights reserved.\n\n" ;
	//
	if ( !(m_dwFlags & optNologo) )
	{
		printf( szTitle ) ;
	}
	if ( !m_strErrMsg.IsEmpty() )
	{
		printf( m_strErrMsg ) ;
		return	1 ;
	}
	if ( m_dwFlags & optUsage )
	{
		PrintResource( MAKEINTRESOURCE(IDR_USAGE) ) ;
		return	0 ;
	}
	if ( m_dwFlags & optHelp )
	{
		PrintResource( MAKEINTRESOURCE(IDR_HELP) ) ;
		return	0 ;
	}
	return	0 ;
}

// 指定されたりソースを標準出力へ出力
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::PrintResource( LPCTSTR lpName )
{
	HMODULE	hModule = ::GetModuleHandle( NULL ) ;
	HRSRC	hRsrc = ::FindResource( hModule, lpName, "RT_RCDATA" ) ;
	HGLOBAL	hData = ::LoadResource( hModule, hRsrc ) ;
	void *	pData = ::LockResource( hData ) ;
	//
	if ( pData != NULL )
	{
		DWORD	dwWrittenBytes ;
		DWORD	dwSize = ::SizeofResource( hModule, hRsrc ) ;
		::WriteFile
			( ::GetStdHandle( STD_OUTPUT_HANDLE ),
				pData, dwSize, &dwWrittenBytes, NULL ) ;
	}
}

// 実行
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::Perform( void )
{
	//
	// 処理
	//
	int	nResult = 0 ;
	if ( m_dwFlags & (optUsage | optHelp) )
	{
		return	0 ;
	}
	if ( m_dwFlags & optInform )
	{
		nResult = InformFiles( ) ;
	}
	else
	{
		if ( m_dwFlags & optIndirect )
		{
			nResult = LoadIndirectFile( ) ;
		}
		else
		{
			nResult = MakeProcess( ) ;
		}
		if ( nResult == 0 )
		{
			nResult = BeginProcess( ) ;
		}
	}
	//
	// レポート出力
	//
	if ( !m_strReportFile.IsEmpty() )
	{
		ERawFile	file ;
		if ( file.Open( m_strReportFile, file.modeCreate ) )
		{
			printf( "レポートファイルを開けませんでした。\n" ) ;
			return	1 ;
		}
		m_xmlReport.WriteDescription( file ) ;
		file.Close( ) ;
	}
	//
	// 出力結果
	//
	if ( !m_strOutputFile.IsEmpty() )
	{
		ERawFile	file ;
		if ( file.Open( m_strOutputFile, file.modeCreate ) )
		{
			printf( "アウトプットファイルを開けませんでした。\n" ) ;
			return	1 ;
		}
		m_bufOutput.WriteToFile( file ) ;
		file.Close( ) ;
	}
	//
	return	nResult ;
}

// MIME から FormatType 列挙子への変換
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::FormatTypeFromMIME( const wchar_t * pwszMIME )
{
	EWideString	wstrMIME = pwszMIME ;
	for ( int i = 0; i < ftMax; i ++ )
	{
		if ( wstrMIME.CompareNoCase( m_pwszFormatMime[i] ) == 0 )
		{
			return	i ;
		}
	}
	return	ftInvalid ;
}

// 対応するフォーマットのデフォルトの拡張子を取得
//////////////////////////////////////////////////////////////////////////////
const wchar_t * EConverterApp::GetDefaultFileExt( int fFmtType )
{
	if ( (fFmtType >= 0) && (fFmtType < ftMax) )
	{
		return	m_pwszFormatExt[fFmtType] ;
	}
	return	NULL ;
}

// 画像ファイル情報を表示する
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::InformFiles( void )
{
	//
	// ファイルを列挙する
	//
	EnumFileName	efnFiles( this, L"", EWideString(m_strSrcFile) ) ;
	ESLError		err	;
	EWideString		wstrFileName ;
	EString			strErrMsg ;
	//
	m_xmlReport.SetTag( L"inform" ) ;
	//
	for ( ; ; )
	{
		//
		// ファイル名を取得
		//
		err = efnFiles.GetNextFileName( wstrFileName, strErrMsg ) ;
		if ( err )
		{
			if ( !strErrMsg.IsEmpty() )
			{
				printf( strErrMsg ) ;
			}
			break ;
		}
		//
		// ファイルを開く
		//
		ERawFile	file ;
		ERIFile		erif ;
		EString		strFileName = wstrFileName ;
		printf( "%s : ", strFileName.CharPtr() ) ;
		if ( file.Open( strFileName, file.modeRead | file.shareRead ) )
		{
			printf( "ファイルを開けませんでした。\n" ) ;
			return	2 ;
		}
		if ( erif.Open( &file ) )
		{
			printf( "これは ERI 画像ファイルではありません。\n" ) ;
			continue ;
		}
		//
		// 画像情報取得
		//
		ERI_INFO_HEADER &	eih = erif.m_InfoHeader ;
		EString		strTransformation, strArchitexture, strFormatType ;
		EGLSize		sizeImage ;
		EGLPoint	ptHotSpot( 0, 0 ) ;
		int			nBitsPerPixel, nResolution = 0 ;
		//
		switch ( eih.fdwTransformation )
		{
		case	CVTYPE_LOSSLESS_ERI:
			strTransformation = "可逆圧縮" ;
			break ;
		case	CVTYPE_DCT_ERI:
			strTransformation = "非可逆 DCT 変換" ;
			break ;
		case	CVTYPE_LOT_ERI:
			strTransformation = "非可逆 LOT 変換" ;
			break ;
		default:
			strTransformation = "不明な画像変換" ;
			break ;
		}
		switch ( eih.dwArchitecture )
		{
		case	ERI_RUNLENGTH_GAMMA:
			strArchitexture = "ERI 符号" ;
			break ;
		case	ERI_RUNLENGTH_HUFFMAN:
			strArchitexture = "ERINA 符号" ;
			break ;
		case	ERISA_NEMESIS_CODE:
			strArchitexture = "ERISA 符号" ;
			break ;
		case	ERI_ARITHMETIC_CODE:
			strArchitexture = "廃止された算術符号" ;
			break ;
		default:
			strArchitexture = "不明な符号" ;
			break ;
		}
		switch ( eih.fdwFormatType )
		{
		case	ERI_RGB_IMAGE:
			strFormatType = "RGB 形式" ;
			break ;
		case	ERI_GRAY_IMAGE:
			strFormatType = "グレイスケール" ;
			break ;
		case	ERI_RGBA_IMAGE:
			strFormatType = "RGBA 形式" ;
			break ;
		default:
			strFormatType = "不明な形式" ;
			break ;
		}
		sizeImage.w = eih.nImageWidth ;
		sizeImage.h = eih.nImageHeight ;
		if ( sizeImage.h < 0 )
		{
			sizeImage.h = - sizeImage.h ;
		}
		nBitsPerPixel = eih.dwBitsPerPixel ;
		//
		if ( erif.m_fdwReadMask & ERIFile::rmDescription )
		{
			ERIFile::ETagInfo	taginf ;
			taginf.CreateTagInfo( erif.m_wstrDescription ) ;
			ptHotSpot = taginf.GetHotSpot( ) ;
			nResolution = taginf.GetResolution( ) ;
		}
		//
		// 画像情報の表示
		//
		printf( "\n\t圧縮方式：%s／%s",
			strTransformation.CharPtr(), strArchitexture.CharPtr() ) ;
		printf( "\n\tフォーマット：%s", strFormatType.CharPtr() ) ;
		printf( "\n\t画像サイズ：%d × %d", sizeImage.w, sizeImage.h ) ;
		if ( nResolution > 0 )
		{
			printf( "(%d dpi)", nResolution ) ;
		}
		printf( "\n\tホットスポット：%d, %d\n\n", ptHotSpot.x, ptHotSpot.y ) ;
		//
		// レポート出力
		//
		EDescription *	pdscTag = new EDescription ;
		pdscTag->SetTag( L"file" ) ;
		m_xmlReport.AddContentTag( pdscTag ) ;
		//
		pdscTag->SetContentString
			( L"filename", EWideString(strFileName) ) ;
		pdscTag->SetContentString
			( L"transformation", EWideString(strTransformation) ) ;
		pdscTag->SetContentString
			( L"architecture", EWideString(strArchitexture) ) ;
		pdscTag->SetContentString
			( L"format", EWideString(strFormatType) ) ;
		//
		pdscTag->SetAttrInteger( L"width", sizeImage.w ) ;
		pdscTag->SetAttrInteger( L"height", sizeImage.h ) ;
		if ( nResolution > 0 )
		{
			pdscTag->SetAttrInteger( L"resolution", nResolution ) ;
		}
		pdscTag->SetIntegerAt( L"hotspot", L"x", ptHotSpot.x ) ;
		pdscTag->SetIntegerAt( L"hotspot", L"y", ptHotSpot.y ) ;
		//
		if ( erif.m_fdwReadMask & ERIFile::rmDescription )
		{
			pdscTag->SetContentString
				( L"description", erif.m_wstrDescription ) ;
		}
		if ( erif.m_fdwReadMask & ERIFile::rmCopyright )
		{
			pdscTag->SetContentString
				( L"copyright", erif.m_wstrCopyright ) ;
		}
	}
	//
	return	0 ;
}

// インダイレクトファイルを読み込む
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::LoadIndirectFile( void )
{
	//
	// ファイルを読み込む
	//
	ERawFile		file ;
	EStreamBuffer	buf ;
	if ( file.Open( m_strSrcFile, file.modeRead | file.shareRead ) )
	{
		printf
			( "%s ファイルを開けませんでした。\n", m_strSrcFile.CharPtr() ) ;
		return	1 ;
	}
	buf.ReadFromFile( file ) ;
	file.Close( ) ;
	//
	if ( m_xmlCvt.ReadDescription( buf ) )
	{
		printf( "変換処理ファイルの解析中にエラーが発生しました。\n" ) ;
		return	0 ;
	}
	//
	// 詞葉スクリプトを構築
	//
	m_pxmlErisa = m_xmlCvt.GetContentTagAs( 0, L"erisa" ) ;
	if ( m_pxmlErisa == NULL )
	{
		printf( "<erisa> タグが見つかりません。\n" ) ;
	}
	ECSSourceStream	cssSource ;
	int				iNextTag = 0 ;
	for ( ; ; )
	{
		EDescription *	pdscScript =
			m_pxmlErisa->GetContentTagAs( iNextTag, L"cotopha", &iNextTag ) ;
		if ( pdscScript == NULL )
		{
			break ;
		}
		EDescription *	pdscComment = pdscScript->GetContentTagAt( 0 ) ;
		if ( (pdscComment == NULL) || pdscComment->IsValid() )
		{
			continue ;
		}
		EStreamWideString	swsScript = pdscComment->Contents( ) ;
		if ( !swsScript.MoveToNextLine( ) )
		{
			int	iFirstLine = swsScript.GetIndex( ) ;
			int	iEndLine = iFirstLine ;
			while ( !swsScript.MoveToNextLine() )
			{
				iEndLine = swsScript.GetIndex( ) ;
			}
			cssSource +=
				swsScript.Middle( iFirstLine, iEndLine - iFirstLine ) ;
		}
	}
	if ( !cssSource.IsEmpty() )
	{
		//
		// コンパイル実行
		//
		ECSXCompiler	compiler ;
		ESLError		err ;
		//
		compiler.Initialize( &m_csxi ) ;
		compiler.SetBaseDirectory
			( m_strSrcFile.GetFileDirectoryPart() ) ;
		//
		compiler.CompileScriptLine
			( ECSSourceStream( L"DeclareType Image, ImageContext" ), 0, NULL ) ;
		compiler.CompileScriptLine
			( ECSSourceStream( L"Include \"cotopha.ch\"" ), 0, NULL ) ;
		compiler.CompileScript( cssSource, NULL ) ;
		err = compiler.FinishCompile( ) ;
		if ( err )
		{
			printf( "スクリプト : エラー : %s\n", GetESLErrorMsg(err) ) ;
			return	1 ;
		}
		if ( compiler.GetErrorCount() > 0 )
		{
			return	1 ;
		}
		//
		m_csxi.SolveLinkInfo( ) ;
		if ( m_csxi.GetUnsolvedLinkCount() > 0 )
		{
			unsigned int	i, nCount ;
			nCount = m_csxi.GetUnsolvedLinkCount() ;
			for ( i = 0; i < nCount; i ++ )
			{
				ECSWideString	wstrName = m_csxi.GetUnsolveLinkName( i ) ;
				printf( "%s は未解決の参照です。\n",
							EString(wstrName).CharPtr() ) ;
			}
			return	1 ;
		}
		//
		// スクリプト環境初期化
		//
		InitializeContext( &m_csxi ) ;
	}
	//
	return	0 ;
}

// 起動引数から処理手続きを作成する
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::MakeProcess( void )
{
	EDescription *	pdscFile ;
	m_pxmlErisa = m_xmlCvt.CreateContentTagAs( 0, L"erisa" ) ;
	pdscFile = m_pxmlErisa->CreateContentTagAs( 0, L"file" ) ;
	pdscFile->SetAttrString( L"src", EWideString(m_strSrcFile) ) ;
	pdscFile->SetAttrString( L"dst", EWideString(m_strDstFile) ) ;
	//
	if ( m_nCutAlign != 0 )
	{
		EDescription *	pdscCut ;
		pdscCut = pdscFile->CreateContentTagAs( 0, L"cut" ) ;
	}
	return	0 ;
}

// 処理を実行する
//////////////////////////////////////////////////////////////////////////////
int EConverterApp::BeginProcess( void )
{
	m_nErrorCount = 0 ;
	m_xmlReport.SetTag( L"process" ) ;
	//
	for ( int i = 0; i < m_pxmlErisa->GetContentTagCount(); i ++ )
	{
		EDescription *	pdscTag = m_pxmlErisa->GetContentTagAt( i ) ;
		if ( pdscTag == NULL )
		{
			continue ;
		}
		if ( pdscTag->Tag() == L"env" )
		{
			SetEnvironment( pdscTag ) ;
		}
		else if ( pdscTag->Tag() == L"file" )
		{
			ProcessFiles( pdscTag ) ;
		}
	}
	return	m_nErrorCount ;
}

// 環境設定を行う
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::SetEnvironment( EDescription * pdscEnv )
{
	if ( pdscEnv->GetAttributeIndexAs( L"mime" ) >= 0 )
	{
		EWideString	wstrMime = pdscEnv->GetAttrString( L"mime", NULL ) ;
		int	nFmtType = FormatTypeFromMIME( wstrMime ) ;
		if ( nFmtType == ftInvalid )
		{
			printf( "<env> : mime = \"%s\" は"
				"対応していない出力形式です\n", EString(wstrMime).CharPtr() ) ;
			m_nErrorCount ++ ;
		}
	}
	if ( pdscEnv->GetAttributeIndexAs( L"dstdir" ) >= 0 )
	{
		m_wstrDstDir = pdscEnv->GetAttrString( L"dstdir", NULL ) ;
		if ( (m_wstrDstDir.GetLength() > 1)
			&& (m_wstrDstDir.Right(1) != L"\\") )
		{
			m_wstrDstDir += L'\\' ;
		}
		if ( (m_wstrDstDir.GetAt(0) != L'\\')
				&& (m_wstrDstDir.Find( L':' ) < 0) )
		{
			EWideString	wstrDir = m_strSrcFile.GetFileDirectoryPart( ) ;
			wstrDir += m_wstrDstDir ;
			m_wstrDstDir = wstrDir ;
		}
	}
	if ( pdscEnv->GetAttributeIndexAs( L"srcdir" ) >= 0 )
	{
		m_wstrSrcDir = pdscEnv->GetAttrString( L"srcdir", NULL ) ;
		if ( (m_wstrSrcDir.GetLength() > 1)
			&& (m_wstrSrcDir.Right(1) != L"\\") )
		{
			m_wstrSrcDir += L'\\' ;
		}
		if ( (m_wstrSrcDir.GetAt(0) != L'\\')
				&& (m_wstrSrcDir.Find( L':' ) < 0) )
		{
			EWideString	wstrDir = m_strSrcFile.GetFileDirectoryPart( ) ;
			wstrDir += m_wstrSrcDir ;
			m_wstrSrcDir = wstrDir ;
		}
	}
	m_nPutAlign = pdscEnv->GetAttrInteger( L"put_align", m_nPutAlign ) ;
	m_nCutAlign = pdscEnv->GetAttrInteger( L"cut_align", m_nCutAlign ) ;
	m_nCutMargin = pdscEnv->GetAttrInteger( L"cut_margin", m_nCutMargin ) ;
	m_nCutThreshold =
		pdscEnv->GetAttrInteger( L"cut_threshold", m_nCutThreshold ) ;
	m_fKeepHotspot =
		pdscEnv->GetAttrInteger( L"keep_hotspot", m_fKeepHotspot ) ;
	m_fLayerBlend =
		pdscEnv->GetAttrInteger( L"layer_blend", m_fLayerBlend ) ;
	m_fLayerMask =
		pdscEnv->GetAttrInteger( L"layer_mask", m_fLayerMask ) ;
}

// ファイル処理を行う
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::ProcessFiles( EDescription * pdscFile )
{
	if ( pdscFile->GetAttributeIndexAs( L"src" ) >= 0 )
	{
		//
		// 入出力ファイル名列挙の準備
		//
		EnumFileName	efnSrc( this ), efnDst( this ) ;
		EWideString		wstrDstDir ;
		EWideString		wstrDstFile =
				pdscFile->GetAttrString( L"dst", NULL ) ;
		efnSrc.SetEnumFiles
			( m_wstrSrcDir, pdscFile->GetAttrString( L"src", NULL ) ) ;
		if ( (wstrDstFile.Right(1) != L"\\") && !wstrDstFile.IsEmpty() )
		{
			efnDst.SetEnumFiles( m_wstrDstDir, wstrDstFile ) ;
			if ( efnDst.m_type == EnumFileName::typeWin32 )
			{
				if ( (wstrDstFile.GetAt(0) != L'\\')
					&& (wstrDstFile.Find( L':' ) < 0) )
				{
					wstrDstFile = m_wstrDstDir + wstrDstFile ;
				}
			}
		}
		else
		{
			if ( (wstrDstFile.GetAt(0) != L'\\')
				&& (wstrDstFile.Find( L':' ) < 0) )
			{
				wstrDstDir = m_wstrDstDir + wstrDstFile ;
			}
			else
			{
				wstrDstDir = wstrDstFile ;
			}
		}
		long int	nIndex = 0 ;
		EString		strErrMsg ;
		EWideString	wstrSrcFile ;
		for ( ; ; )
		{
			//
			// 入力ファイル名取得
			//
			efnSrc.m_lstArg.SetAt( 0, new ECSInteger( nIndex ) ) ;
			if ( efnSrc.GetNextFileName( wstrSrcFile, strErrMsg ) )
			{
				if ( !strErrMsg.IsEmpty() )
				{
					printf( strErrMsg + "\n" ) ;
					m_nErrorCount ++ ;
				}
				break ;
			}
			//
			// 出力ファイル名決定
			//
			if ( efnDst.m_type == EnumFileName::typeWin32 )
			{
			}
			else if ( efnDst.m_type )
			{
				efnDst.m_lstArg.SetAt
					( 0, new ECSInteger( nIndex ) ) ;
				efnDst.m_lstArg.SetAt
					( 0, new ECSString( wstrSrcFile.GetFileNamePart() ) ) ;
				if ( efnDst.GetNextFileName( wstrDstFile, strErrMsg ) )
				{
					if ( !strErrMsg.IsEmpty() )
					{
						printf( strErrMsg + "\n" ) ;
						m_nErrorCount ++ ;
					}
					else
					{
						printf
							( "%s に対応する出力ファイル名を"
								"決定できませんでした。\n",
								EString(wstrSrcFile).CharPtr() ) ;
						m_nErrorCount ++ ;
					}
					break ;
				}
				wstrDstFile =
					NormalizeDestinationFileName( wstrDstFile, wstrSrcFile ) ;
			}
			else if ( !wstrDstFile.IsEmpty() )
			{
				wstrDstFile = wstrDstDir
					+ wstrSrcFile.GetFileTitlePart()
					+ L"." + GetDefaultFileExt( m_nFmtType ) ;
			}
			if ( wstrSrcFile.CompareNoCase( wstrDstFile ) == 0 )
			{
				printf
					( "%s は入出力ファイル名が重なっています。\n",
						EString(wstrSrcFile).CharPtr() ) ;
				m_nErrorCount ++ ;
				continue ;
			}
			//
			// 入力ファイルを読み込む
			//
			ECSImageContext	imgctx ;
			EString			strFileName = wstrSrcFile ;
			EWideString		wstrHotSpot =
					pdscFile->GetAttrString( L"hotspot", NULL ) ;
			//
			printf( "%s : \n", strFileName.CharPtr() ) ;
			if ( LoadImageFile
				( imgctx, wstrSrcFile, wstrHotSpot, nIndex ) )
			{
				printf( "読み込みに失敗しました。\n\n" ) ;
				m_nErrorCount ++ ;
				return ;
			}
			//
			// 処理開始
			//
			ProcessFileTags( imgctx, pdscFile ) ;
			//
			// 保存
			//
			if ( !wstrDstFile.IsEmpty() )
			{
				printf( " --> %s\n", EString(wstrDstFile).CharPtr() ) ;
				if ( SaveImageContext( imgctx, wstrDstFile, m_nFmtType ) )
				{
					printf( "    書き出しに失敗しました。\n" ) ;
					m_nErrorCount ++ ;
					return ;
				}
			}
			else
			{
				if ( imgctx.m_varArray.GetSize() != 0 )
				{
					printf( "（画像コンテキストに残っている"
								"画像データは破棄されます）\n" ) ;
				}
			}
			printf( "\n" ) ;
			//
			nIndex ++ ;
		}
	}
	else
	{
		//
		// 単一ファイルの処理
		//
		ECSImageContext	imgctx ;
		EWideString		wstrDst = pdscFile->GetAttrString( L"dst", NULL ) ;
		printf( "file : \n" ) ;
		ProcessFileTags( imgctx, pdscFile ) ;
		//
		// 保存
		//
		if ( !wstrDst.IsEmpty() )
		{
			if ( (wstrDst.GetAt(0) != L'\\')
				&& (wstrDst.Find( L':' ) < 0) )
			{
				wstrDst = m_wstrDstDir + wstrDst ;
			}
			printf( " --> %s\n", EString(wstrDst).CharPtr() ) ;
			if ( SaveImageContext( imgctx, wstrDst, m_nFmtType ) )
			{
				m_nErrorCount ++ ;
				return ;
			}
		}
		else
		{
			if ( imgctx.m_varArray.GetSize() != 0 )
			{
				printf( "（画像コンテキストに残っている"
							"画像データは破棄されます）\n" ) ;
			}
		}
		printf( "\n" ) ;
	}
}

// ファイル処理
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::ProcessFileTags
	( ECSImageContext & imgctx, EDescription * pdscFile )
{
	//
	// スクリプト呼び出し
	//
	EWideString	wstrScript = pdscFile->GetAttrString( L"script", NULL ) ;
	if ( !wstrScript.IsEmpty() )
	{
		DWORD *	pdwFuncAddr = m_csxi.GetFunctionAddress( wstrScript ) ;
		if ( pdwFuncAddr != NULL )
		{
			EObjArray<ECSObject>	lstArg ;
			lstArg.SetAt( 0, new ECSReference( &imgctx ) ) ;
			ESLError	err = CallFunction( *pdwFuncAddr, lstArg ) ;
			if ( err )
			{
				printf( "\nエラー：%s\n", GetESLErrorMsg(err) ) ;
			}
		}
		else
		{
			printf( "%s 関数が見つかりません。\n",
						EString(wstrScript).CharPtr() ) ;
		}
	}
	//
	// 各タグの処理
	//
	for ( int iTag = 0; iTag < pdscFile->GetContentTagCount(); iTag ++ )
	{
		EDescription *	pdscTag = pdscFile->GetContentTagAt( iTag ) ;
		if ( pdscTag == NULL )
		{
			continue ;
		}
		if ( pdscTag->Tag() == L"cut" )
		{
			//
			// 画像の切り出し・サイズ揃え
			//
			EGL_IMAGE_RECT	irCut ;
			int				nAlign, nMargin, nThreshold, fKeepHotspot ;
			int				nOpFlags = 0 ;
			EWideString		wstrAlphaOp ;
			nAlign = pdscTag->GetAttrInteger( L"align", m_nCutAlign ) ;
			nMargin = pdscTag->GetAttrInteger( L"margin", m_nCutMargin ) ;
			nThreshold =
				pdscTag->GetAttrInteger( L"threshold", m_nCutThreshold ) ;
			fKeepHotspot =
				pdscTag->GetAttrInteger( L"keep_hotspot", m_fKeepHotspot ) ;
			if ( fKeepHotspot )
			{
				nOpFlags |= ECSImageContext::cofKeepHotspot ;
			}
			wstrAlphaOp = pdscTag->GetAttrString( L"op_alpha", L"auto" ) ;
			if ( wstrAlphaOp == L"remove" )
			{
				nOpFlags |= ECSImageContext::cofRemoveAlpha ;
			}
			else if ( wstrAlphaOp != L"keep" )
			{
				nOpFlags |= ECSImageContext::cofAutoAlpha ;
			}
			if ( !imgctx.CutImage
				( nAlign, nMargin, nThreshold, nOpFlags, irCut ) )
			{
				printf( "cut : (%d, %d) - (%d x %d)\n",
						irCut.x, irCut.y, irCut.w, irCut.h ) ;
				//
				EDescription *	pdscComment = new EDescription ;
				pdscComment->SetCommentTag
					( imgctx.m_strFileName.m_varStr + L" の切り出し" ) ;
				m_xmlReport.AddContentTag( pdscComment ) ;
				//
				EDescription *	pdscCut = new EDescription ;
				pdscCut->SetTag( L"cut" ) ;
				pdscCut->SetAttrInteger( L"x", irCut.x ) ;
				pdscCut->SetAttrInteger( L"y", irCut.y ) ;
				pdscCut->SetAttrInteger( L"width", irCut.w ) ;
				pdscCut->SetAttrInteger( L"height", irCut.h ) ;
				pdscCut->SetAttrString
					( L"file", imgctx.m_strFileName.m_varStr ) ;
				m_xmlReport.AddContentTag( pdscCut ) ;
			}
			else
			{
				m_nErrorCount ++ ;
			}
		}
		else if ( pdscTag->Tag() == L"arrange" )
		{
			//
			// 画像の整列配列
			//
			bool	fWayVert, fPutRight ;
			fWayVert = (pdscTag->GetAttrString( L"way", L"horz" ) == L"vert") ;
			fPutRight = fWayVert ;
			if ( pdscTag->GetAttributeIndexAs( L"put" ) >= 0 )
			{
				fPutRight =
					(pdscTag->GetAttrString( L"put", L"under" ) == L"right") ;
			}
			int		nAlign = pdscTag->GetAttrInteger( L"align", m_nPutAlign ) ;
			//
			m_nErrorCount +=
				imgctx.ArrangeImage( fWayVert, fPutRight, nAlign ) ;
		}
		else if ( pdscTag->Tag() == L"animation" )
		{
			//
			// アニメーションの生成
			//
			EWideString	wstrSeq = pdscTag->GetAttrString( L"seq", NULL ) ;
			int			nDuration =
				pdscTag->GetAttrInteger
					( L"duration", imgctx.m_intDuration.m_varInt ) ;
			//
			m_nErrorCount +=
				imgctx.MakeAnimation( wstrSeq, nDuration ) ;
		}
		else if ( pdscTag->Tag() == L"write" )
		{
			//
			// 連番ファイルに出力
			//
			WriteSequenceFiles
				( imgctx, pdscTag->GetAttrString( L"dst", NULL ) ) ;
		}
		else if ( pdscTag->Tag() == L"image" )
		{
			//
			// 画像ファイル読み込み
			//
			ProcessImages( imgctx, pdscTag ) ;
		}
		else if ( pdscTag->Tag() == L"remove" )
		{
			//
			// 画像コンテキストの削除
			//
			imgctx.DeleteContext( ) ;
		}
		else if ( pdscTag->Tag() == L"select" )
		{
			//
			// 画像コンテキストを生成
			//
			ProcessSelectImage( imgctx, pdscTag ) ;
		}
	}
}

// 画像を選択
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::ProcessSelectImage
	( ECSImageContext & imgctx, EDescription * pdscSelect )
{
	//
	// パラメータ取得
	//
	EWideString	wstrDst = pdscSelect->GetAttrString( L"dst", NULL ) ;
	EStreamWideString	swsLayer =
				pdscSelect->GetAttrString( L"layer", NULL ) ;
	EStreamWideString	swsFrame =
				pdscSelect->GetAttrString( L"frame", NULL ) ;
	//
	EObjArray<EWideString>	lstParam ;
	EStreamWideString	swsUsage = L"(<0-9>*) [\\- (<0-9>*)] [\\, (%n)] \\" ;
	int			iFirst, iEnd, nStep = 0 ;
	EString		strErrMsg ;
	if ( !swsFrame.IsMatchUsage( swsUsage, strErrMsg, &lstParam ) )
	{
		iFirst = lstParam[0].AsInteger( ) ;
		if ( !lstParam[1].IsEmpty() )
		{
			iEnd = lstParam[1].AsInteger( ) ;
		}
		else
		{
			iEnd = iFirst ;
		}
		if ( !lstParam[2].IsEmpty() )
		{
			nStep = lstParam[3].AsInteger( ) ;
		}
		else if ( iFirst > iEnd )
		{
			nStep = -1 ;
		}
		else
		{
			nStep = 1 ;
		}
	}
	//
	// 画像情報を複製
	//
	ECSImageContext	ictxSel ;
	ictxSel.m_sizeImage.SetMemberAsInt
			( L"w", imgctx.m_sizeImage.GetMemberAsInt( L"w", 0 ) ) ;
	ictxSel.m_sizeImage.SetMemberAsInt
			( L"h", imgctx.m_sizeImage.GetMemberAsInt( L"h", 0 ) ) ;
	ictxSel.m_ptHotSpot.SetMemberAsInt
			( L"x", imgctx.m_ptHotSpot.GetMemberAsInt( L"x", 0 ) ) ;
	ictxSel.m_ptHotSpot.SetMemberAsInt
			( L"y", imgctx.m_ptHotSpot.GetMemberAsInt( L"y", 0 ) ) ;
	ictxSel.m_strFileName.m_varStr = imgctx.m_strFileName.m_varStr ;
	//
	// 画像を選択
	//
	if ( !swsLayer.IsEmpty() )
	{
		if ( nStep == 0 )
		{
			iFirst = 0 ;
			iEnd = imgctx.GetLength() - 1 ;
			nStep = 1 ;
		}
	}
	if ( nStep != 0 )
	{
		for ( int i = iFirst; i * nStep <= iEnd * nStep; i += nStep )
		{
			ECSImage *	pImage = imgctx.GetImageAt( i ) ;
			if ( pImage != NULL )
			{
				bool	fSelect = true ;
				if ( !swsLayer.IsEmpty() )
				{
					EStreamWideString	swsName ;
					EString				strErrMsg ;
					swsLayer.MoveIndex( 0 ) ;
					swsName = pImage->m_strName.m_varStr ;
					if ( swsName.IsMatchUsage( swsLayer, strErrMsg ) )
					{
						fSelect = false ;
					}
				}
				if ( fSelect )
				{
					ECSImage *	pDupImage = (ECSImage*) pImage->Duplicate( ) ;
					if ( wstrDst.IsEmpty() )
					{
						imgctx.m_varArray.RemoveAt( i ) ;
						iEnd -= nStep ;
						i -= nStep ;
					}
					ictxSel.m_varArray.Add( pDupImage ) ;
				}
			}
		}
	}
	//
	// 処理実行
	//
	printf( "select : \n" ) ;
	ProcessFileTags( ictxSel, pdscSelect ) ;
	//
	// 結果出力
	//
	if ( !wstrDst.IsEmpty() )
	{
		wstrDst = NormalizeDestinationFileName
					( wstrDst, ictxSel.m_strFileName.m_varStr ) ;
		printf( " --> %s\n", EString(wstrDst).CharPtr() ) ;
		//
		if ( (wstrDst.GetAt(0) != L'\\') && (wstrDst.Find( L':' ) < 0) )
		{
			wstrDst = m_wstrDstDir + wstrDst ;
		}
		if ( SaveImageContext( ictxSel, wstrDst, m_nFmtType ) )
		{
			m_nErrorCount ++ ;
			return ;
		}
	}
	else
	{
		imgctx.MergeContext( ictxSel ) ;
	}
}

// ファイルを読み込む
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::ProcessImages
	( ECSImageContext & imgctx, EDescription * pdscImage )
{
	//
	// 入力ファイル名取得
	//
	EObjArray<EWideString>	lstSources ;
	EnumFileName			efnSrc( this ) ;
	EWideString	wstrSrc = pdscImage->GetAttrString( L"src", NULL ) ;
	if ( wstrSrc.IsEmpty() )
	{
		EStreamWideString	swsSources =
			pdscImage->GetContentString( NULL, NULL ) ;
		int		i = 0 ;
		while ( !swsSources.DisregardSpace() )
		{
			EWideString	wstrFile =
				swsSources.GetEnclosedString( L';', FALSE ) ;
			wstrFile.TrimRight( ) ;
			if ( (wstrFile.GetAt(0) != L'\\')
				&& (wstrFile.Find( L':' ) < 0) )
			{
				lstSources[i ++] = m_wstrSrcDir + wstrFile ;
			}
			else
			{
				lstSources[i ++] = wstrFile ;
			}
		}
	}
	else
	{
		efnSrc.SetEnumFiles( m_wstrSrcDir, wstrSrc ) ;
	}
	//
	// αチャネル用ファイル名取得
	//
	EWideString		wstrAlpha = pdscImage->GetAttrString( L"alpha", NULL ) ;
	EnumFileName	efnAlpha( this ) ;
	if ( !wstrAlpha.IsEmpty() )
	{
		efnAlpha.SetEnumFiles( m_wstrSrcDir, wstrAlpha ) ;
	}
	//
	// ホットスポット取得
	//
	EWideString	wstrHotSpot = pdscImage->GetAttrString( L"hotspot", NULL ) ;
	//
	// 順次画像ファイルを読み込む
	//
	printf( "<image> :\n" ) ;
	//
	int		nIndex = 0 ;
	EString	strErrMsg ;
	for ( ; ; )
	{
		//
		// 入力ファイル名取得
		//
		if ( efnSrc.m_type )
		{
			efnSrc.m_lstArg.SetAt( 0, new ECSInteger( nIndex ) ) ;
			if ( efnSrc.GetNextFileName( wstrSrc, strErrMsg ) )
			{
				if ( !strErrMsg.IsEmpty() )
				{
					printf( "  %s\n", strErrMsg.CharPtr() ) ;
					m_nErrorCount ++ ;
				}
				break ;
			}
		}
		else
		{
			if ( nIndex >= (int) lstSources.GetSize() )
			{
				break ;
			}
			wstrSrc = lstSources[nIndex] ;
		}
		//
		// 画像ファイルの読み込み
		//
		ECSImageContext	icImage ;
		printf( "  %s : ", EString(wstrSrc).CharPtr() ) ;
		if ( LoadImageFile( icImage, wstrSrc, wstrHotSpot, nIndex ) )
		{
			printf( "\n  読み込みに失敗しました。\n" ) ;
			continue ;
		}
		printf( "OK\n" ) ;
		//
		// αチャネル画像の読み込み
		//
		if ( efnAlpha.m_type )
		{
			efnAlpha.m_lstArg.SetAt( 0, new ECSInteger( nIndex ) ) ;
			efnAlpha.m_lstArg.SetAt( 1, new ECSString( wstrSrc ) ) ;
			if ( efnAlpha.GetNextFileName( wstrAlpha, strErrMsg ) )
			{
				if ( !strErrMsg.IsEmpty() )
				{
					printf( "  alpha : %s\n", strErrMsg.CharPtr() ) ;
					m_nErrorCount ++ ;
				}
				break ;
			}
			printf( "  alpha : %s : ", EString(wstrAlpha).CharPtr() ) ;
			//
			ECSImageContext	icAlpha ;
			if ( LoadImageFile( icAlpha, wstrAlpha, NULL, nIndex ) )
			{
				printf( "\n  読み込みに失敗しました\n" ) ;
			}
			else if ( icImage.m_imgBuf.GetSize()
							!= icAlpha.m_imgBuf.GetSize() )
			{
				printf( "\n  画像サイズが一致しないので"
							"αチャネルを合成できません。\n" ) ;
			}
			else
			{
				printf( "OK\n" ) ;
				//
				while ( icImage.m_imgBuf.GetTotalFrameCount() > 1 )
				{
					icImage.m_imgBuf.RemoveFrameAt( 1 ) ;
				}
				while ( icAlpha.m_imgBuf.GetTotalFrameCount() > 1 )
				{
					icAlpha.m_imgBuf.RemoveFrameAt( 1 ) ;
				}
				icImage.m_imgBuf.GetSequenceTable().RemoveAll( ) ;
				icImage.m_imgBuf.GetSequenceTable().SetAt( 0, 0 ) ;
				//
				icAlpha.m_imgBuf.ConvertFormatTo( EIF_GRAY_BITMAP, 8 ) ;
				icImage.m_imgBuf.ConvertFormatTo( EIF_RGBA_BITMAP, 32 ) ;
				icImage.m_imgBuf.BlendAlphaChannel
					( NULL, icAlpha.m_imgBuf, EGL_BAC_MULTIPLY ) ;
			}
		}
		//
		// 画像バッファに追加
		//
		imgctx.MergeContext( icImage ) ;
		//
		nIndex ++ ;
	}
}

// 連番ファイル出力
//////////////////////////////////////////////////////////////////////////////
void EConverterApp::WriteSequenceFiles
	( ECSImageContext & imgctx, const wchar_t * pwszDstFile )
{
	EnumFileName	efnDst( this, m_wstrDstDir, pwszDstFile ) ;
	if ( efnDst.m_type == EnumFileName::typeWin32 )
	{
		printf( "\'%s\' 連番ファイルの書式が不正です。\n",
								EString(pwszDstFile).CharPtr() ) ;
		m_nErrorCount ++ ;
		return ;
	}
	int	i ;
	for ( i = 0; i < imgctx.GetLength(); i ++ )
	{
		ECSImage *	pImage = imgctx.GetImageAt( i ) ;
		if ( pImage == NULL )
		{
			continue ;
		}
		EWideString	wstrDst ;
		EString		strErrMsg ;
		efnDst.m_lstArg.SetAt( 0, new ECSReference( &imgctx ) ) ;
		efnDst.m_lstArg.SetAt( 1, new ECSInteger( i ) ) ;
		if ( efnDst.GetNextFileName( wstrDst, strErrMsg ) )
		{
			if ( !strErrMsg.IsEmpty() )
			{
				printf( "%s\n", strErrMsg.CharPtr() ) ;
			}
			break ;
		}
		//
		wstrDst = NormalizeDestinationFileName
					( wstrDst, imgctx.m_strFileName.m_varStr ) ;
		printf( " --> %s\n", EString(wstrDst).CharPtr() ) ;
		//
		if ( SaveImageFile( pImage->m_imgBuf, wstrDst, m_nFmtType ) )
		{
			printf( "    書き出しに失敗しました。\n" ) ;
			m_nErrorCount ++ ;
		}
	}
	imgctx.m_varArray.RemoveBetween( 0, i ) ;
}

// 出力ファイル名を整形する
//////////////////////////////////////////////////////////////////////////////
EWideString EConverterApp::NormalizeDestinationFileName
	( const wchar_t * pwszDst, const wchar_t * pwszSrc )
{
	EWideString	wstrDst, wstrBuf = pwszDst ;
	EWideString	wstrSrc = EWideString(pwszSrc).GetFileTitlePart() ;
	//
	// * 記号の置き換え
	//
	int	iLast = 0, iNext ;
	for ( ; ; )
	{
		iNext = wstrBuf.Find( L'*', iLast ) ;
		if ( iNext < 0 )
		{
			break ;
		}
		wstrDst += wstrBuf.Middle( iLast, iNext - iLast ) ;
		wstrDst += wstrSrc ;
		iLast = iNext + 1 ;
	}
	wstrDst += wstrBuf.Middle( iLast ) ;
	//
	// ? 記号の置き換え
	//
	int	i = 0, j = 0 ;
	while ( i < (int) wstrDst.GetLength() )
	{
		if ( wstrDst[i] == L'?' )
		{
			if ( j < (int) wstrSrc.GetLength() )
			{
				wstrDst.SetAt( i, wstrSrc.GetAt( j ++ ) ) ;
			}
			else
			{
				wstrDst = wstrDst.Left( i ) + wstrDst.Middle( i + 1 ) ;
			}
		}
		i ++ ;
	}
	return	wstrDst ;
}

// 画像を読み込む
//////////////////////////////////////////////////////////////////////////////
ESLError EConverterApp::LoadImageFile
	( ECSImageContext & imgctx,
		const wchar_t * pwszSrc,
		const wchar_t * pwszHotSpot, int nIndex )
{
	//
	// 画像読み込み
	//
	if ( imgctx.LoadImageFile( pwszSrc, m_fLayerBlend, m_fLayerMask ) )
	{
		return	eslErrSuccess ;
	}
	//
	// ホットスポット設定
	//
	EStreamWideString	swsHotSpot = pwszHotSpot ;
	if ( !swsHotSpot.IsEmpty() )
	{
		EObjArray<EWideString>	lstParam ;
		EStreamWideString		swsUsage = L"(%n),(%n)\\" ;
		EString					strErrMsg ;
		if ( swsHotSpot.CurrentCharacter() == L'$' )
		{
			DWORD *	pdwFuncAddr =
				m_csxi.GetFunctionAddress( swsHotSpot.Middle(1) ) ;
			if ( pdwFuncAddr != NULL )
			{
				EObjArray<ECSObject>	lstArg ;
				lstArg.SetAt( 0, new ECSReference( &imgctx ) ) ;
				lstArg.SetAt( 1, new ECSInteger( nIndex ) ) ;
				ESLError	err = CallFunction( *pdwFuncAddr, lstArg ) ;
				if ( err )
				{
					printf( "\nエラー：%s\n", GetESLErrorMsg(err) ) ;
				}
			}
			else
			{
				printf( "hotspot : %s 関数が見つかりません。\n",
						EString(swsHotSpot.Middle(1)).CharPtr() ) ;
			}
		}
		else if ( !swsHotSpot.IsMatchUsage( swsUsage, strErrMsg, &lstParam ) )
		{
			imgctx.m_ptHotSpot.
				SetMemberAsInt( L"x", lstParam[0].AsInteger() ) ;
			imgctx.m_ptHotSpot.
				SetMemberAsInt( L"y", lstParam[1].AsInteger() ) ;
		}
		else
		{
			printf( "\'%s\' は不正なホットスポットの書式です。\n",
									EString(pwszHotSpot).CharPtr() ) ;
		}
	}
	//
	return	eslErrSuccess ;
}

// 画像をファイルに書き出す
//////////////////////////////////////////////////////////////////////////////
ESLError EConverterApp::SaveImageContext
	( ECSImageContext & imgctx, const wchar_t * pwszDst, int nFmtType )
{
	EGL_IMAGE_RECT	irClip ;
	if ( imgctx.m_imgBuf.GetInfo() == NULL )
	{
		m_nErrorCount += imgctx.CutImage
			( m_nCutAlign, m_nCutMargin,
				m_nCutThreshold, m_fKeepHotspot, irClip ) ;
		m_nErrorCount +=
			imgctx.MakeAnimation( NULL, imgctx.m_intDuration.m_varInt ) ;
	}
	return	SaveImageFile( imgctx.m_imgBuf, pwszDst, m_nFmtType ) ;
}

ESLError EConverterApp::SaveImageFile
	( EGLMediaLoader & imgBuf, const wchar_t * pwszDst, int nFmtType )
{
	EWideString	wstrDst = pwszDst ;
	EWideString	wstrExt = wstrDst.GetFileExtensionPart( ) ;
	if ( wstrExt.IsEmpty() )
	{
		if ( wstrDst.Right(1) != L"." )
		{
			wstrDst += L'.' ;
		}
		wstrDst += GetDefaultFileExt( m_nFmtType ) ;
		pwszDst = wstrDst ;
	}
	if ( (nFmtType >= ftERI) && (nFmtType <= ftERISA) )
	{
		//
		// ERI フォーマット
		//
		static const EGLImage::CompressTypeFlag	ctfTypes[] =
		{
			EGLImage::ctfCompatibleFormat,
			EGLImage::ctfExtendedFormat,
			EGLImage::ctfSuperiorArchitecure
		} ;
		static const DWORD	dwFlags[] =
		{
			ERISAEncoder::efBestCmpr,
			ERISAEncoder::efHighCmpr,
			ERISAEncoder::efNormalCmpr,
			ERISAEncoder::efLowCmpr
		} ;
		ERawFile	file ;
		if ( file.Open( EString(pwszDst), file.modeCreate ) )
		{
			return	eslErrGeneral ;
		}
		if ( imgBuf.WriteImageFile
			( file, ctfTypes[nFmtType - ftERI], dwFlags[m_nCmprMode] ) )
		{
			return	eslErrGeneral ;
		}
		file.Close( ) ;
		return	eslErrSuccess ;
	}
	else if ( nFmtType == ftBMP )
	{
		//
		// Windows Bitmap フォーマット
		//
		ERawFile	file ;
		if ( file.Open( EString(pwszDst), file.modeCreate ) )
		{
			return	eslErrGeneral ;
		}
		if ( imgBuf.WriteBitmapFile( file ) )
		{
			return	eslErrGeneral ;
		}
		file.Close( ) ;
		return	eslErrSuccess ;
	}
	else
	{
		//
		// その他のフォーマット
		//
		const wchar_t *	pwszMimeType = NULL ;
		switch ( nFmtType )
		{
		case	ftTIFF:
			pwszMimeType = L"image/tiff" ;
			break ;
		case	ftPNG:
			pwszMimeType = L"image/png" ;
			break ;
		case	ftJPEG:
			pwszMimeType = L"image/jpeg" ;
			break ;
		default:
			return	eslErrGeneral ;
		}
		if ( !EGLMediaLoader::IsInstalledGDIplus() )
		{
			printf( "  %s 形式で保存するには GDI+ が必要です。\n",
									EString(pwszMimeType).CharPtr() ) ;
			return	eslErrGeneral ;
		}
		return	imgBuf.SaveWithGDIplus( EString(pwszDst), pwszMimeType ) ;
	}
}

