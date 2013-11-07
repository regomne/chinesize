// 立绘.cpp : 定义控制台应用程序的入口点。
// This program must use unicode
//#include "stdafx.h"
#include <cstdlib>
#include <SDKDDKVer.h>
#include <tchar.h>
#include "cstring"
#include "windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "SHB.h"
#include "CBG.h"          /* 解码核心 */
#include "ARC.h"          /* 从.ARC中提取文件 */
 
int filen(FILE *handle);          //取文件长度 

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pf ;
	int i ;
	CBG pic ;
	ARC pics ;
	wchar_t name[ 1024 ] ;
	wchar_t savedir[ 1024 ] ;
	wchar_t toname[ 64 ] ;
	memset( name , 0 , sizeof( name ) ) ;
	memset( savedir , 0 , sizeof( savedir ) ) ;
	memset( toname , 0 , sizeof( toname ) ) ;
	SetConsoleTitle( _T( "BGI新图像格式解码测试程序 xcjfgt@126.com" ) ) ;

	std::cout<<" *********     Make by xcjfht@126.com     ********** \n" ;
	std::cout << "**********        Ver 0.1.0      *********** \n" ;
	std::cout<<"按键继续程序...... \n " ;
	getchar( ) ;
	setlocale(LC_ALL,"Chinese-simplified") ;           /* 使用简体中文 */
	for( ; name[ 0 ] == 0 ; SHBopen( name , _T( "选择要解包的ARC文件" ) , 1 ) ) ;
	std::wcout << name ;
	for( ; savedir[ 0 ] == 0 ; SHBopen( savedir , _T( "选择要保存到的文件夹" ) , 0 ) ) ;

	if ( pics.ARCopen( name ) ) std::cout<<"打开成功！\n" ;
	else
	{
		std::cout<<"打开ARC文件失败！\n" ;
		system( "pause" );
		return -1 ;
	}

	for( i = 0 ; i < pics.FileNum ; i++ )
	{
		if ( pics.ARCread( i ) ) 
		{
			printf( "%s 读取成功 " , pics.OneName ) ;
			if ( pic.Uncompress( pics.OneBuf , ( pics.OneFile )->FileSize ) ) 
			{
				std::cout<<" 解码成功" ;
				mbstowcs( toname , pics.OneName , strlen( pics.OneName) ) ;
				wcscpy( name , savedir ) ;
				wcscat( name , _T( "\\" ) ) ;
				wcscat( name , toname ) ;
				wcscat( name , _T( ".bmp" ) ) ;
				pf = _wfopen( name , _T( "wb" ) ) ;
				if ( pf != NULL )
				 {
					fwrite( pic.BmpData , 1 , pic.Size , pf ) ;
					fclose( pf ) ;          //一定要记得释放文件句柄......
					std::cout << "  写出成功 \n" ;
				}
				else std::cout << "写出失败  \n" ;
				continue ;
			}
			  std::cout<<" 解码失败 \n" ;
			  printf( "%s \n" , pic.GetError( ) ) ;
			  continue ;
		}
		std::cout<<" 读取失败" ;
	}
	std::cout<<"处理完毕" ;
	system( "pause" ) ;
	return 0;
}
int filen(FILE *handle)
{
    int len;
     fseek(handle,0,2);
     len=ftell(handle);
     fseek(handle,0,0);
     return len;
}

