

#include	"common.h"



void ReadMoveAcsOngen( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->seek( 12 );

    while ( 1 )
	{
		byte b = s->readbyte();

		if ( b == 0x70 )
			break;

		b = s->readbyte();
		s->readdword();
		if ( b & 1 )
			s->seek( 12 );
		else
			s->readdword(),
			s->readdword();

		int n = s->readword();
		for ( int i=0; i<n; i++ )
			s->seek( 12 );

		if ( b & 2 )
			ReadArchiveObject( s, cache, object );
	}

	s->readdword();
	s->readdword();
	s->readdword();
	s->readbyte();
	s->readword();
}
