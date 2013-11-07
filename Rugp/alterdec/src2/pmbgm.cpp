
#include	"common.h"



void ReadPmBgm( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readword();
	ReadClassList( s, cache, object );
	printf( "%s\n", s->readstring().c_str() );
	s->readword();
}
