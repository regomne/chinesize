
#pragma once



#include	"common.h"



class CImageWriter
{
	int		width, height;
	byte *	buf;

public:
			CImageWriter( int w, int h );
	void	set( int x, int y, dword color );
	void	set( int x, int y, byte r, byte g, byte b, byte a=0xff );
	void	write( dword offset );
			~CImageWriter();
};

