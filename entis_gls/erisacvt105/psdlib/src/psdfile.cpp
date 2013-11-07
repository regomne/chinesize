
/*****************************************************************************
                  Adobe(R) Photoshop(R) ファイルローダー
 *****************************************************************************/

#include <gls.h>
#include <psdfile.h>


using	namespace PSD ;

//////////////////////////////////////////////////////////////////////////////
// PSD ファイル
//////////////////////////////////////////////////////////////////////////////

// クラス情報
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_INFO( PSD::File, ESLObject )

// 構築関数
//////////////////////////////////////////////////////////////////////////////
File::File( void )
{
	m_pfile = NULL ;
}

// 消滅関数
//////////////////////////////////////////////////////////////////////////////
File::~File( void )
{
	Close( ) ;
}

// ファイルを開く
//////////////////////////////////////////////////////////////////////////////
ESLError File::Open( ESLFileObject & file )
{
	ESLError	errCannotRead = ESLErrorMsg( "読み込みに失敗しました。" ) ;
	//
	// 以前のデータを破棄
	//
	Close( ) ;
	//
	// ファイルヘッダを読み込む
	//
	if ( file.Read( &m_fhHeader, sizeof(m_fhHeader) ) < sizeof(m_fhHeader) )
	{
		return	ESLErrorMsg( "ファイルヘッダを読み込めませんでした。" ) ;
	}
	if ( dwswap('8BPS') != m_fhHeader.dwSignature )
	{
		return	ESLErrorMsg( "ファイルシグネチャが一致しません。" ) ;
	}
	//
	// Color Mode Data を読み込む
	//
	DWORD	dwLength ;
	if ( file.Read( &dwLength, sizeof(DWORD) ) < sizeof(DWORD) )
	{
		return	errCannotRead ;
	}
	if ( dwLength > 0 )
	{
		dwLength = dwswap( dwLength ) ;
		file.Read( m_bufColorMode.PutBuffer(dwLength), dwLength ) ;
		m_bufColorMode.Flush( dwLength ) ;
	}
	//
	// Image Resource Section を読み飛ばす
	//
	if ( file.Read( &dwLength, sizeof(DWORD) ) < sizeof(DWORD) )
	{
		return	errCannotRead ;
	}
	m_dwLayerDataPos = file.GetPosition() + dwswap(dwLength) ;
	if ( file.Seek( m_dwLayerDataPos, file.FromBegin ) != m_dwLayerDataPos )
	{
		return	errCannotRead ;
	}
	//
	// Layer Mask Information Section を読み込む
	//
	if ( file.Read( &dwLength, sizeof(DWORD) ) < sizeof(DWORD) )
	{
		return	errCannotRead ;
	}
	m_dwBaseDataPos = file.GetPosition() + dwswap(dwLength) ;
	if ( dwLength != 0 )
	{
		if ( file.Read( &dwLength, sizeof(DWORD) ) < sizeof(DWORD) )
		{
			return	errCannotRead ;
		}
		WORD	wLayerCount ;
		if ( file.Read( &wLayerCount, sizeof(WORD) ) < sizeof(WORD) )
		{
			return	errCannotRead ;
		}
		int		nLayerCount = (SWORD) wswap( wLayerCount ) ;
		if ( nLayerCount <= 0 )
		{
			nLayerCount = - nLayerCount ;
		}
		for ( int i = 0; i < nLayerCount; i ++ )
		{
			LAYER_RECORD *	plrLayer = new LAYER_RECORD ;
			m_lstLayer.Add( plrLayer ) ;
			//
			// レイヤー領域（矩形）、及びチャネル数
			//
			if ( file.Read( plrLayer, sizeof(DWORD) * 4 + sizeof(WORD) )
										< sizeof(DWORD) * 4 + sizeof(WORD) )
			{
				return	errCannotRead ;
			}
			//
			// チャネル長情報、及び残りのデータを読み込む
			//
			if ( wswap(plrLayer->wChannels) > 24 )
			{
				return	errCannotRead ;
			}
			dwLength =
				sizeof(CHANNEL_LENGTH_INFO) * wswap(plrLayer->wChannels) ;
			if ( file.Read( plrLayer->chlen, dwLength ) < dwLength )
			{
				return	errCannotRead ;
			}
			dwLength =
				(sizeof(DWORD) * 2 + sizeof(BYTE) * 4 + sizeof(DWORD)) ;
			if ( file.Read( &(plrLayer->dwSignature), dwLength ) < dwLength )
			{
				return	errCannotRead ;
			}
			//
			// 拡張情報を読み込む
			//
			EStreamBuffer	bufExtra ;
			dwLength = dwswap( plrLayer->dwExtraSize ) ;
			file.Read( bufExtra.PutBuffer( dwLength ), dwLength ) ;
			bufExtra.Flush( dwLength ) ;
			//
			// Adjustment Layer Data
			//
			bufExtra.Read( &(plrLayer->adjdata.dwSize), sizeof(DWORD) ) ;
			dwLength = dwswap( plrLayer->adjdata.dwSize ) ;
			if ( dwLength != 0 )
			{
				bufExtra.Read( &(plrLayer->adjdata.dwTop), dwLength ) ;
			}
			//
			// Layer Blending ranges data
			//
			dwLength = 0 ;
			bufExtra.Read( &dwLength, sizeof(DWORD) ) ;
			dwLength = dwswap( dwLength ) ;
			bufExtra.GetBuffer( dwLength ) ;
			bufExtra.Release( dwLength ) ;
			//
			// Layer name
			//
			EPtrBuffer	ptrbuf = bufExtra.GetBuffer( ) ;
			if ( ptrbuf.GetLength() > 2 )
			{
				const BYTE *	pchName = (const BYTE *) ptrbuf.GetBuffer() ;
				if ( ((int) pchName[0] + 1) <= (int) ptrbuf.GetLength() )
				{
					plrLayer->strLayerName =
						EString( (const char *) pchName + 1, (int) pchName[0] ) ;
				}
			}
		}
	}
	//
	// 完了
	//
	m_dwLayerDataPos = file.GetPosition( ) ;
	m_pfile = &file ;
	//
	return	eslErrSuccess ;
}

// ファイルを閉じる
//////////////////////////////////////////////////////////////////////////////
void File::Close( void )
{
	m_bufColorMode.Delete( ) ;
	m_lstLayer.RemoveAll( ) ;
	m_pfile = NULL ;
}

// ヘッダ情報を取得する
//////////////////////////////////////////////////////////////////////////////
void File::GetFileHeader( FILE_HEADER & fhHeader ) const
{
	fhHeader = m_fhHeader ;
	fhHeader.wVersion = wswap( fhHeader.wVersion ) ;
	fhHeader.wChannels = wswap( fhHeader.wChannels ) ;
	fhHeader.dwHeight = dwswap( fhHeader.dwHeight ) ;
	fhHeader.dwWidth = dwswap( fhHeader.dwWidth ) ;
	fhHeader.wDepth = wswap( fhHeader.wDepth ) ;
	fhHeader.wMode = wswap( fhHeader.wMode ) ;
}

// カラーモード情報を取得する
//////////////////////////////////////////////////////////////////////////////
void File::GetColorModeData( EStreamBuffer bufColorMode ) const
{
	EPtrBuffer	ptrbuf = ((File*)this)->m_bufColorMode.GetBuffer() ;
	bufColorMode.Write( ptrbuf, ptrbuf.GetLength() ) ;
	((File*)this)->m_bufColorMode.Release( 0 ) ;
}

// レイヤーの総数を取得する
//////////////////////////////////////////////////////////////////////////////
unsigned int File::GetLayerCount( void ) const
{
	return	m_lstLayer.GetSize() ;
}

// レイヤー情報を取得する
//////////////////////////////////////////////////////////////////////////////
ESLError File::GetLayerInfo
	( File::LayerInfo & liLayer, unsigned int iLayer ) const
{
	LAYER_RECORD *	plrLayer = m_lstLayer.GetAt( iLayer ) ;
	if ( plrLayer == NULL )
	{
		return	eslErrGeneral ;
	}
	//
	liLayer.irRect.x = (int) dwswap( plrLayer->dwLeft ) ;
	liLayer.irRect.y = (int) dwswap( plrLayer->dwTop ) ;
	liLayer.irRect.w = (int) dwswap( plrLayer->dwRight ) - liLayer.irRect.x ;
	liLayer.irRect.h = (int) dwswap( plrLayer->dwBottom ) - liLayer.irRect.y ;
	//
	if ( plrLayer->adjdata.dwSize != 0 )
	{
		liLayer.irMask.x = (int) dwswap( plrLayer->adjdata.dwLeft ) ;
		liLayer.irMask.y = (int) dwswap( plrLayer->adjdata.dwTop ) ;
		liLayer.irMask.w =
			(int) dwswap( plrLayer->adjdata.dwRight ) - liLayer.irMask.x ;
		liLayer.irMask.h =
			(int) dwswap( plrLayer->adjdata.dwBottom ) - liLayer.irMask.y ;
	}
	else
	{
		liLayer.irMask = liLayer.irRect ;
	}
	//
	liLayer.nChannels = wswap( plrLayer->wChannels ) ;
	for ( int i = 0; i < liLayer.nChannels; i ++ )
	{
		liLayer.chlen[i].wChannelID = wswap( plrLayer->chlen[i].wChannelID ) ;
		liLayer.chlen[i].dwLength = dwswap( plrLayer->chlen[i].dwLength ) ;
	}
	liLayer.dwBlendMode = plrLayer->dwBlendMode ;
	liLayer.nTransparency =
		0x100 - ((int) plrLayer->bytOpacity + (plrLayer->bytOpacity >> 7)) ;
	liLayer.dwFlags = plrLayer->bytFlags ;
	liLayer.strLayerName = plrLayer->strLayerName ;
	//
	return	eslErrSuccess ;
}

// レイヤー画像を取得する
//////////////////////////////////////////////////////////////////////////////
ESLError File::LoadLayerImage
	( EGLImage & imgBuf, unsigned int iLayer )
{
	if ( m_pfile == NULL )
	{
		return	eslErrGeneral ;
	}
	//
	// データのファイル上のアドレスを計算する
	//
	DWORD		dwDataPos ;
	LayerInfo	liLayer ;
	if ( iLayer == (unsigned int) -1 )
	{
		return	LoadBaseImage( imgBuf ) ;
	}
	else
	{
		dwDataPos = m_dwLayerDataPos ;
		for ( unsigned int i = 0; i < iLayer; i ++ )
		{
			if ( GetLayerInfo( liLayer, i ) )
			{
				return	eslErrGeneral ;
			}
			for ( int j = 0; j < liLayer.nChannels; j ++ )
			{
				dwDataPos += liLayer.chlen[j].dwLength ;
			}
		}
		if ( GetLayerInfo( liLayer, iLayer ) )
		{
			return	eslErrGeneral ;
		}
	}
	//
	// 画像バッファの生成
	//
	DWORD	fdwFormat, dwBitsPerPixel, dwDefLineBytes, dwPixelStep ;
	if ( liLayer.nChannels < 3 )
	{
		if ( liLayer.nChannels <= 0 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_GRAY_BITMAP ;
		dwBitsPerPixel = wswap( m_fhHeader.wDepth ) ;
		dwDefLineBytes = (liLayer.irRect.w * dwBitsPerPixel + 0x07) >> 3 ;
		dwPixelStep = 1 ;
	}
	else if ( liLayer.nChannels == 3 )
	{
		if ( wswap( m_fhHeader.wDepth ) != 8 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_RGB_BITMAP ;
		dwDefLineBytes = liLayer.irRect.w ;
		dwBitsPerPixel = 32 ;
		dwPixelStep = 4 ;
	}
	else if ( liLayer.nChannels >= 4 )
	{
		if ( wswap( m_fhHeader.wDepth ) != 8 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_RGBA_BITMAP ;
		dwDefLineBytes = liLayer.irRect.w ;
		dwBitsPerPixel = 32 ;
		dwPixelStep = 4 ;
	}
	PEGL_IMAGE_INFO	pImage =
		imgBuf.CreateImage
			( fdwFormat, liLayer.irRect.w, liLayer.irRect.h, dwBitsPerPixel ) ;
	if ( pImage == NULL )
	{
		return	eslErrGeneral ;
	}
	imgBuf.ReverseVertically( ) ;
	//
	// 各チャネルを展開
	//
	bool	fAlphaFlag = false ;
	for ( int iChannel = 0; iChannel < liLayer.nChannels; iChannel ++ )
	{
		//
		// ファイルのシーク
		//
		if ( m_pfile->Seek
			( dwDataPos, ESLFileObject::FromBegin ) != dwDataPos )
		{
			return	eslErrGeneral ;
		}
		dwDataPos += liLayer.chlen[iChannel].dwLength ;
		//
		// 圧縮方式取得
		//
		WORD	wCompression ;
		if ( m_pfile->Read( &wCompression, sizeof(WORD) ) < sizeof(WORD) )
		{
			return	eslErrGeneral ;
		}
		wCompression = wswap( wCompression ) ;
		//
		// 各行のバイト数を取得
		//
		EStreamBuffer	bufLineBuf ;
		EStreamBuffer	bufLineSize ;
		EStreamBuffer	bufRLE ;
		WORD *			pwLineSize = NULL ;
		BYTE *			ptrRLE = NULL ;
		int				iRLE = 0 ;
		DWORD			dwTotalBytes = 0 ;
		DWORD			dwLineBytes ;
		EGL_IMAGE_RECT	irRect ;
		//
		if ( liLayer.chlen[iChannel].wChannelID
						== (WORD) chidUserSuppliedMask )
		{
			irRect = liLayer.irMask ;
			irRect.x -= liLayer.irRect.x ;
			irRect.y -= liLayer.irRect.y ;
			if ( liLayer.nChannels < 3 )
			{
				dwLineBytes = (irRect.w * dwBitsPerPixel + 0x07) >> 3 ;
			}
			else
			{
				dwLineBytes = irRect.w ;
			}
		}
		else
		{
			irRect.x = 0 ;
			irRect.y = 0 ;
			irRect.w = liLayer.irRect.w ;
			irRect.h = liLayer.irRect.h ;
			dwLineBytes = dwDefLineBytes ;
		}
		BYTE *			ptrLineBuf =
			(BYTE*) bufLineBuf.PutBuffer( dwLineBytes ) ;
		//
		if ( wCompression == 1 )	// RLE
		{
			DWORD	dwBytes = irRect.h * sizeof(WORD) ;
			pwLineSize = (WORD*) bufLineSize.PutBuffer( dwBytes ) ;
			if ( m_pfile->Read( pwLineSize, dwBytes ) < dwBytes )
			{
				return	eslErrGeneral ;
			}
			for ( int i = 0; i < irRect.h; i ++ )
			{
				pwLineSize[i] = wswap( pwLineSize[i] ) ;
				dwTotalBytes += pwLineSize[i] ;
			}
			ptrRLE = (BYTE*) bufRLE.PutBuffer( dwTotalBytes ) ;
			if ( m_pfile->Read( ptrRLE, dwTotalBytes ) < dwTotalBytes )
			{
				return	eslErrGeneral ;
			}
		}
		else if ( wCompression != 0 )
		{
			return	eslErrGeneral ;
		}
		//
		// 各ラインを順次読み込み・展開
		//
		for ( int y = 0; y < irRect.h; y ++ )
		{
			if ( wCompression == 0 )	// Non-compress
			{
				if ( m_pfile->Read( ptrLineBuf, dwLineBytes ) < dwLineBytes )
				{
					return	eslErrGeneral ;
				}
			}
			else						// RLE
			{
				UnpackBits
					( ptrLineBuf, (int) dwLineBytes,
						ptrRLE + iRLE, pwLineSize[y] ) ;
				iRLE += pwLineSize[y] ;
			}
			//
			BYTE *	ptrDstLine = (BYTE*) pImage->ptrImageArray ;
			ptrDstLine += pImage->dwBytesPerLine * (y + irRect.y) ;
			//
			int		i ;
			bool	fProduct = false ;
			switch ( liLayer.chlen[iChannel].wChannelID )
			{
			case	chidRed:
				i = 2 ;
				break ;
			case	chidGreen:
				i = 1 ;
				break ;
			case	chidBlue:
				i = 0 ;
				break ;
			default:
				i = 3 ;
				fProduct = fAlphaFlag ;
				break ;
			}
			if ( i * 8 > (int) dwBitsPerPixel )
			{
				i = 0 ;
			}
			i += dwPixelStep * irRect.x ;
			//
			if ( fProduct )
			{
				for ( int x = 0; x < irRect.w; x ++ )
				{
					ptrDstLine[i] =
						(BYTE) (((int) ptrDstLine[i]
								* ((int) ptrLineBuf[x] + 1)) >> 8) ;
					i += dwPixelStep ;
				}
			}
			else
			{
				for ( int x = 0; x < irRect.w; x ++ )
				{
					ptrDstLine[i] = ptrLineBuf[x] ;
					i += dwPixelStep ;
				}
			}
		}
		switch ( liLayer.chlen[iChannel].wChannelID )
		{
		case	chidRed:
		case	chidGreen:
		case	chidBlue:
			break ;
		default:
			fAlphaFlag = true ;
			break ;
		}
	}
	//
	return	eslErrSuccess ;
}

// ベース画像を取得する
//////////////////////////////////////////////////////////////////////////////
ESLError File::LoadBaseImage( EGLImage & imgBuf )
{
	if ( m_pfile == NULL )
	{
		return	eslErrGeneral ;
	}
	//
	// ヘッダ情報取得
	//
	FILE_HEADER	fhHeader ;
	GetFileHeader( fhHeader ) ;
	//
	// 画像バッファの生成
	//
	DWORD	fdwFormat, dwBitsPerPixel, dwLineBytes, dwPixelStep ;
	int		nChannels = (int) fhHeader.wChannels ;
	if ( nChannels < 3 )
	{
		if ( nChannels <= 0 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_GRAY_BITMAP ;
		dwBitsPerPixel = fhHeader.wDepth ;
		dwLineBytes = fhHeader.dwWidth * dwBitsPerPixel ;
		dwPixelStep = 1 ;
	}
	else if ( nChannels == 3 )
	{
		if ( fhHeader.wDepth != 8 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_RGB_BITMAP ;
		dwLineBytes = fhHeader.dwWidth ;
		dwBitsPerPixel = 32 ;
		dwPixelStep = 4 ;
	}
	else if ( nChannels >= 4 )
	{
		if ( fhHeader.wDepth != 8 )
		{
			return	eslErrGeneral ;
		}
		fdwFormat = EIF_RGBA_BITMAP ;
		dwLineBytes = fhHeader.dwWidth ;
		dwBitsPerPixel = 32 ;
		dwPixelStep = 4 ;
	}
	PEGL_IMAGE_INFO	pImage =
		imgBuf.CreateImage
			( fdwFormat, fhHeader.dwWidth, fhHeader.dwHeight, dwBitsPerPixel ) ;
	if ( pImage == NULL )
	{
		return	eslErrGeneral ;
	}
	imgBuf.ReverseVertically( ) ;
	//
	// ファイルのシーク
	//
	if ( m_pfile->Seek
		( m_dwBaseDataPos, ESLFileObject::FromBegin ) != m_dwBaseDataPos )
	{
		return	eslErrGeneral ;
	}
	//
	// 圧縮方式取得
	//
	WORD	wCompression ;
	if ( m_pfile->Read( &wCompression, sizeof(WORD) ) < sizeof(WORD) )
	{
		return	eslErrGeneral ;
	}
	wCompression = wswap( wCompression ) ;
	//
	// 各行のバイト数を取得
	//
	EStreamBuffer	bufLineBuf ;
	EStreamBuffer	bufLineSize ;
	EStreamBuffer	bufRLE ;
	BYTE *	ptrLineBuf = (BYTE*) bufLineBuf.PutBuffer( dwLineBytes ) ;
	WORD *	pwLineSize = NULL ;
	BYTE *	ptrRLE = NULL ;
	int		iRLE = 0 ;
	DWORD	dwTotalBytes = 0 ;
	int		nTotalLines = (int) fhHeader.dwHeight * nChannels ;
	//
	if ( wCompression == 1 )	// RLE
	{
		DWORD	dwBytes = nTotalLines * sizeof(WORD) ;
		pwLineSize = (WORD*) bufLineSize.PutBuffer( dwBytes ) ;
		if ( m_pfile->Read( pwLineSize, dwBytes ) < dwBytes )
		{
			return	eslErrGeneral ;
		}
		for ( int i = 0; i < nTotalLines; i ++ )
		{
			pwLineSize[i] = wswap( pwLineSize[i] ) ;
			dwTotalBytes += pwLineSize[i] ;
		}
		ptrRLE = (BYTE*) bufRLE.PutBuffer( dwTotalBytes ) ;
		if ( m_pfile->Read( ptrRLE, dwTotalBytes ) < dwTotalBytes )
		{
			return	eslErrGeneral ;
		}
	}
	else if ( wCompression != 0 )
	{
		return	eslErrGeneral ;
	}
	//
	// 各チャネルを展開
	//
	for ( int iChannel = 0; iChannel < nChannels; iChannel ++ )
	{
		//
		// 各ラインを順次読み込み
		//
		for ( int y = 0; y < (int) fhHeader.dwHeight; y ++ )
		{
			if ( wCompression == 0 )	// Non-compress
			{
				if ( m_pfile->Read( ptrLineBuf, dwLineBytes ) < dwLineBytes )
				{
					return	eslErrGeneral ;
				}
			}
			else						// RLE
			{
				int	i = y + iChannel * fhHeader.dwHeight ;
				UnpackBits
					( ptrLineBuf, (int) dwLineBytes,
						ptrRLE + iRLE, pwLineSize[i] ) ;
				iRLE += pwLineSize[i] ;
			}
			//
			BYTE *	ptrDstLine = (BYTE*) pImage->ptrImageArray ;
			ptrDstLine += pImage->dwBytesPerLine * y ;
			//
			int		i ;
			if ( iChannel < 3 )
			{
				i = 2 - iChannel ;
			}
			else
			{
				i = 3 ;
			}
			if ( i * 8 > (int) dwBitsPerPixel )
			{
				i = 0 ;
			}
			if ( iChannel < 4 )
			{
				for ( int x = 0; x < (int) fhHeader.dwWidth; x ++ )
				{
					ptrDstLine[i] = ptrLineBuf[x] ;
					i += dwPixelStep ;
				}
			}
			else
			{
				for ( int x = 0; x < (int) fhHeader.dwWidth; x ++ )
				{
					ptrDstLine[i] =
						(BYTE) (((int) ptrDstLine[i]
								* ((int) ptrLineBuf[x] + 1)) >> 8) ;
					i += dwPixelStep ;
				}
			}
		}
	}
	//
	return	eslErrSuccess ;
}

// RLE の展開
//////////////////////////////////////////////////////////////////////////////
void File::UnpackBits
	( BYTE * ptrBuf, int nBufSize, const BYTE * ptrRLE, int nRLESize )
{
	int	i = 0, j = 0 ;
	while ( (j < nBufSize) && (i + 2 <= nRLESize) )
	{
		int		nLength ;
		if ( ptrRLE[i]  & 0x80 )
		{
			BYTE	bytFill ;
			nLength = 1 - (SBYTE) ptrRLE[i ++] ;
			bytFill = ptrRLE[i ++] ;
			if ( j + nLength > nBufSize )
			{
				nLength = nBufSize - j ;
			}
			for ( int k = 0; k < nLength; k ++ )
			{
				ptrBuf[j + k] = bytFill ;
			}
			j += k ;
		}
		else
		{
			nLength = ptrRLE[i ++] + 1 ;
			if ( j + nLength > nBufSize )
			{
				nLength = nBufSize - j ;
			}
			if ( i + nLength > nRLESize )
			{
				nLength = nRLESize - i ;
			}
			for ( int k = 0; k < nLength; k ++ )
			{
				ptrBuf[j + k] = ptrRLE[i + k] ;
			}
			i += nLength ;
			j += nLength ;
		}
	}
}

