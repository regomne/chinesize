#include <cstring>
#include "shlobj.h"
#include "windows.h"
bool SHBopen( wchar_t *dest , wchar_t *textinfo , int IsFile ) ;

bool SHBopen( wchar_t *dest , wchar_t *textinfo , int IsFile )
{
	BROWSEINFO bi ;
	bi.hwndOwner = NULL ;   // 父窗口句柄
	bi.pidlRoot = NULL ;        // 要显示的文件目录对话框的根(Root)
   bi.pszDisplayName = dest ;  // 保存被选取的文件夹路径的缓冲区
   bi.lpszTitle = textinfo  ;  // 显示位于对话框左上部的标题
   if ( IsFile == 1 ) bi.ulFlags = BIF_BROWSEINCLUDEFILES ;  //同时显示文件
   else    bi.ulFlags = BIF_STATUSTEXT;    // 只显示目录
   bi.lpfn = NULL ;        // 处理事件的回调函数
   bi.lParam = 0 ;         // 应用程序传给回调函数的参数
   bi.iImage= NULL ;    // 文件夹对话框的图片索引
   LPITEMIDLIST pIDList = SHBrowseForFolder( &bi ) ;
   if(pIDList)
       {
          SHGetPathFromIDList(pIDList, dest ) ;
       }

return true ;
}