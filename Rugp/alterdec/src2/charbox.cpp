
#include	"common.h"



void ReadCharBox( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readword();
	s->readword();
	s->readdword();
	s->readbyte();
	for ( int i=0; i<5; i++ )
		s->readqword();
	printf( "%s\n", s->readstring().c_str() );

	word n = s->readword();
	for ( int i=0; i<n; i++ )
		throw "ReadCharBox/TWFbox";

	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readqword();
	s->readword();
	s->readword();
	for(int i=0;i<4;i++)
		ReadClassList( s, cache, object );

	s->readbyte();
	s->readbyte();

	s->readdword();
	s->readdword();

	ReadClassList( s, cache, object );
	s->readdword();
	s->readdword();
	s->readbyte();

	s->readbyte();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();}
