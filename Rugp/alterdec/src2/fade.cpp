
#include	"fade.h"



void ReadFadeNormal( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	int n = s->readword();
	s->readword();

	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );

	s->readqword();
	s->readqword();
	s->readqword();
	
	s->seek( n * 4 );
}



void ReadFadeMergeBlack( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readword();
	s->readword();

	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );

	s->readqword();
	s->readqword();
	s->readqword();
	
	s->seek( 0x44 );
}



void ReadFadeMergeWhite( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeNormal( s, cache, object );

	s->readdword();
}



void ReadFadeCarten( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readword();
	s->readword();
	s->readbyte();
	s->readbyte();

	ReadFadeNormal( s, cache, object );
}




void ReadFadeOverStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeStretchAnti( s, cache, object );
	
	for ( int i=0; i<4; i++ )
		s->readword();
}


void ReadFadeQubeStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeStretchAnti( s, cache, object );
	
	for ( int i=0; i<13; i++ )
		s->readword();
	s->readbyte();
}
void ReadFadeStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeNormal( s, cache, object );
	for ( int i=0; i<8; i++ )
		s->readword();
	s->readbyte();
}


void ReadFadeXsCircleRaster( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeXsSqrRaster_HRasterV_VRasterH( s, cache, object );
	s->readdword();
}



void ReadFadeXsRasterNoize( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeXsRatio( s, cache, object );
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readbyte();
	s->readbyte();
	s->readbyte();

	s->readdword();

	s->readword();
}



void ReadFadeXsRatio( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	int n = s->readword();
	s->readword();
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	s->readqword();
	s->readqword();
	s->readqword();
	s->seek( n * 4 );

	s->readbyte();
	s->readbyte();
	s->readword();
	s->readword();
}

void ReadFadeTelevisionWipe( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeNormal(s,cache,object);
	s->readbyte();
}

void ReadFadeXsSqrRaster_HRasterV_VRasterH( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeXsRatio( s, cache, object );
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readbyte();
	s->readbyte();
	s->readbyte();
}

void ReadFadeXsSrcRotate( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeXsRatio( s, cache, object );

	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readbyte();
	s->readbyte();
}

void ReadFadeXsOverStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadFadeXsRatio( s, cache, object );

	for ( int i=0; i<12; i++ )
		s->readword();
}
