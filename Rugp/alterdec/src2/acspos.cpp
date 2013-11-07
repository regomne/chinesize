
#include	"common.h"



void ReadAcsPos( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	for ( int i=0; i<6; i++ )
		s->readdword();
	s->readword();
	s->readword();
	s->readdword();
}
