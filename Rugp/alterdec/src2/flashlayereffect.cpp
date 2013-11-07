
#include	"common.h"



void ReadFlashLayerEffect( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	ReadClassList( s, cache, object );
	
	s->seek( 4*8+2*2+4*7+2*2+4*7 );
}
