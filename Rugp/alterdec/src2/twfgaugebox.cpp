
#include	"common.h"


void ReadCharBox( CStream *s, vector<CLASS> *cache, const OBJECT *object );

void ReadTWFGaugeBox( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadCharBox(s,cache,object);

	s->readword();
	for ( int i=0; i<6; i++ )
		ReadClassList( s, cache, object );
}
