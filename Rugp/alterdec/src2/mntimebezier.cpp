
#include	"common.h"



void ReadMN_Time_Bezier( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	int n = s->readword();

	for ( int i=0; i<n; i++ )
		s->seek( 0x18 );
}
