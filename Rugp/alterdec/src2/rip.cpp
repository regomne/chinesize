
#include	"common.h"



void ReadRip( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	dword offset = object->offset + s->getreadbyte() / 2;

	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	int width = s->readword();
	int height = s->readword();
	s->readdword();
	s->readdword();		//	data size
	s->readdword();

    //

	CImageWriter	image( width, height );

	byte temp;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; ; )
		{
			int n = s->readbyte();

			if ( n > 0 )
			{
				x+=n;
				if ( x >= width )
					break;
			}
			temp = s->readbyte();
		}
	}

	image.write( offset );
}

void ReadRip007( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	//d w*6 d
	s->seek(20);
	//b*7
	s->seek(7);

	ReadClassList(s,cache,object);

	dword n=s->readdword();
	s->readdword();

	s->seek(n);
}