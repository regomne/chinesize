
#include	"common.h"



void ReadProcessOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	if ( s->readdword() != 0x23 )
		throw "ReadProcessOcean";

	for ( int i=0; i<7; i++ ) //7
		ReadClassList( s, cache, object );
}
