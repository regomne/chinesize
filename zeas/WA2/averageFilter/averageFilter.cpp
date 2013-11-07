// averageFilter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

__declspec(dllexport)
unsigned int __stdcall averageFilter(unsigned char *inBuf, unsigned int bmBufWidthBytes, unsigned int FontEdgeSize, unsigned char *outDib)
{
	unsigned char *editBuf = inBuf;
	unsigned int Denominator = FontEdgeSize * FontEdgeSize * 2 / 3;
	for(int y = 0; y < bmBufWidthBytes; ++y)
	{
		int yend = y + FontEdgeSize > bmBufWidthBytes ? bmBufWidthBytes : y + FontEdgeSize;
		int ybegin = y - FontEdgeSize < 0 ? 0 : y - FontEdgeSize;
		unsigned int avgsum = 0;//每行开始重新建立窗口
		for(int yy = ybegin; yy < yend; ++yy)
			for(int xx = 0; xx < FontEdgeSize; ++xx)
				avgsum += editBuf[yy * bmBufWidthBytes + xx];
		for(int x = 0; x < bmBufWidthBytes; ++x)//自左向右移动
		{
			int xleft = x - FontEdgeSize - 1;
			if(xleft >= 0) //移除左像素
			{
				for(int yy = ybegin; yy < yend; ++yy)
				{
					avgsum -= editBuf[yy * bmBufWidthBytes + xleft];
				}
			}
			int xright = x + FontEdgeSize;
			if(xright < bmBufWidthBytes) //增加右像素
			{
				for(int yy = ybegin; yy < yend; ++yy)
				{
					avgsum += editBuf[yy * bmBufWidthBytes + xright];
				}
			}
			unsigned int avg = avgsum / Denominator;
			avg = avg > 255 ? 255 : avg; //限值
			outDib[y * bmBufWidthBytes + x] = avg; //同时作透明通道用
		}
	}

	return 0;
}
