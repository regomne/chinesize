
#include	"common.h"

void ReadCharBox( CStream *s, vector<CLASS> *cache, const OBJECT *object );

void ReadTWFRemainderBox( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{

	ReadCharBox(s,cache,object);

	s->readword();
	ReadClassList( s, cache, object );

	s->readdword();
	ReadClassList( s, cache, object );
}
