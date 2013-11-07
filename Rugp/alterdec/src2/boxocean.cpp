
#include	"common.h"



static void Sub( CStream *s, vector<CLASS> *cache, const OBJECT *object );



void ReadBoxOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	for ( int i=0; i<35; i++ ) //0x20+3?
		ReadClassList( s, cache, object );

	Sub( s, cache, object );
	ReadClassList( s, cache, object );
	Sub( s, cache, object );
}



void Sub( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	while ( 1 )
	{
		byte b = s->readbyte();

		if ( b == 0 )
			break;

		ReadClassList( s, cache, object );
		
		if ( b & 1 )
			Sub( s, cache, object );
	}
}