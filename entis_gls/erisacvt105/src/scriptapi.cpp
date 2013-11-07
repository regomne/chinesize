
/*****************************************************************************
                          ERISA 画像コンバーター
 *****************************************************************************/


#include "erisacvt.h"


// 拡張型オブジェクトを生成する
//////////////////////////////////////////////////////////////////////////////
ECSObject * EConverterApp::CreateExtendedObject
	( CSVariableType csvtType,
		const wchar_t * pwszType, DWORD * pdwFuncAddr )
{
	ECSObject *	pObj =
		ECSContext::CreateExtendedObject( csvtType, pwszType, pdwFuncAddr ) ;
	if ( pObj != NULL )
	{
		return	pObj ;
	}
	ECSWideString	wstrType = pwszType ;
	if ( wstrType == L"Image" )
	{
		return	new ECSImage ;
	}
	else if ( wstrType == L"ImageContext" )
	{
		return	new ECSImageContext ;
	}
	return	NULL ;
}

// 関数の引数を出力フォーマットに変換
//////////////////////////////////////////////////////////////////////////////
void InnerPrint
	( EStreamBuffer & bufOut,
		ECSContext & context, EObjArray<ECSObject> & lstArg )
{
	for ( int i = 0; i < (int) lstArg.GetSize(); i ++ )
	{
		ECSObject *	pObj = lstArg.GetAt(i)->GetObjectEntity( ) ;
		if ( pObj == NULL )
		{
			continue ;
		}
		EString	strFormat ;
		switch ( pObj->m_vtType )
		{
		case	csvtString:
			strFormat = ((ECSString*)pObj)->m_varStr ;
			bufOut.Write( strFormat.CharPtr(), strFormat.GetLength() ) ;
			break ;
		case	csvtInteger:
			strFormat = EString( (int) ((ECSInteger*)pObj)->m_varInt ) ;
			bufOut.Write( strFormat.CharPtr(), strFormat.GetLength() ) ;
			break ;
		case	csvtReal:
			strFormat = EString( ((ECSInteger*)pObj)->m_varInt ) ;
			bufOut.Write( strFormat.CharPtr(), strFormat.GetLength() ) ;
			break ;
		default:
			pObj->DumpObject( bufOut, 0, context ) ;
			break ;
		}
	}
}

// 関数 : print( ... )
//////////////////////////////////////////////////////////////////////////////
ECS_EXPORT ESLError ecs_print
	( ECSContext & context, EObjArray<ECSObject> & lstArg )
{
	EStreamBuffer	buf ;
	InnerPrint( buf, context, lstArg ) ;
	//
	EPtrBuffer	ptrbuf = buf.GetBuffer() ;
	DWORD	dwWrittenBytes ;
	::WriteFile
		( ::GetStdHandle( STD_OUTPUT_HANDLE ),
			ptrbuf, ptrbuf.GetLength(), &dwWrittenBytes, NULL ) ;
	//
	return	context.PushObject( new ECSInteger ) ;
}

// 関数 : output( ... )
//////////////////////////////////////////////////////////////////////////////
ECS_EXPORT ESLError ecs_output
	( ECSContext & context, EObjArray<ECSObject> & lstArg )
{
	EConverterApp *	app = ESLTypeCast<EConverterApp>( &context ) ;
	if ( app != NULL )
	{
		InnerPrint( app->m_bufOutput, context, lstArg ) ;
	}
	return	context.PushObject( new ECSInteger ) ;
}
