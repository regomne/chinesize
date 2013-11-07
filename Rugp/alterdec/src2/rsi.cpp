
#include	"common.h"



void ReadRsi( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readword();
	s->readbyte();
	s->readbyte();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	int n = s->readword();	//	count
	for ( int i=0; i<n; i++ )
	{
		word w;
		printf( "%2x ", s->readbyte() );
		printf( "%8x ", s->readdword() );
		printf( "%4x ", s->readword() );
		printf( "%4x ", w=s->readword() );
		if ( ( w & 0x1c00 ) == 0x400 )
			printf( "%4x ", s->readword() );
		printf( "%8x\n", s->readdword() );
	}
}
