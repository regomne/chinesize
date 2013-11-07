
#include	"common.h"



void ReadImgSel( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );
	printf( "%s\n", s->readstring().c_str() );
	s->readword();
	s->readqword();
}
