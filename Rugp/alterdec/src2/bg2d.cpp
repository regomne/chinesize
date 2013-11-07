
#include	"common.h"



void ReadBg2d( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	//cache->push_back( CLASS("CRip008") );
	//s->readword();
	cache->push_back( CLASS("Cr6Ti") );
	s->readword();

	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readword();
	s->seek( 12 );
	
	dword f = s->readdword();

	if ( f & 2 )
		s->readword(),
		s->readword(),
		s->readword();
	if ( f & 8 )
		s->readword();
	if ( f & 4 )
		s->seek( 12 );

	int n = s->readword();	//	count

	for ( int i=0; i<n; i++ )
		s->seek( 12 ),
		ReadClassList( s, cache, object );

	if ( s->readbyte() != 0 )
		ReadClassList( s, cache, object );

	s->readbyte();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();

	s->readdword();

	//Î´Íê
}
