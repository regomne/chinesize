
#include	"imagewriter.h"
//#include	<png.h>



//#ifdef _DEBUG
//	#pragma comment( lib, "libpngd.lib" )
//#else
//	#pragma comment( lib, "libpng.lib" )
//#endif



/*
 *	CImageWriter
 */
CImageWriter::CImageWriter( int w, int h )
{
	width = w;
	height = h;
	buf = new byte[w*h*4];
}



/*
 *	set
 */
void CImageWriter::set( int x, int y, dword color )
{
	*(dword *)&buf[4*(y*width+x)] = color;
}

void CImageWriter::set( int x, int y, byte r, byte g, byte b, byte a )
{
	buf[4*(y*width+x)+0] = b;
	buf[4*(y*width+x)+1] = g;
	buf[4*(y*width+x)+2] = r;
	buf[4*(y*width+x)+3] = a;
}



/*
 *	write
 *		offsetはファイル名に使用
 */
void CImageWriter::write( dword offset )
{
	char filename[16];

	//sprintf( filename, "%08x.png", offset );

	//FILE *f = fopen( filename, "wb" );
	//if ( f == NULL )
	//	throw "書き込み先ファイルを開けませんでした。";

	//png_structp	png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	//png_infop info_ptr = png_create_info_struct( png_ptr );
	//png_init_io( png_ptr, f );
	//png_set_IHDR( png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
	//						PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );
	//png_write_info( png_ptr, info_ptr );

	//png_bytep *line = new png_bytep[height];
	//for ( int i=0; i<height; i++ )
	//	line[i] = buf + 4 * width * i;
	//png_write_image( png_ptr, line );
	//delete[] line;

	//png_write_end( png_ptr, info_ptr );
	//png_destroy_write_struct( &png_ptr, &info_ptr );

	//fclose( f );
}



/* 
 *	~CImageWriter
 */
CImageWriter::~CImageWriter()
{
	delete[] buf;
}
