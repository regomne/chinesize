
#include	"common.h"

#if RUGP_VERSION==1


void ReadProcessOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	if ( s->readdword() != 0x23 )
		throw "ReadProcessOcean";

	for ( int i=0; i<7; i++ ) //7
		ReadClassList( s, cache, object );
}

#elif RUGP_VERSION==2

void ReadProcessOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
    if ( s->readdword() != 0x22 )
        throw "ReadProcessOcean";

    for ( int i=0; i<5; i++ ) //7
        ReadClassList( s, cache, object );
}
#endif
