
#include	"common.h"



void ReadPostureMoment( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readbyte();
	int n=s->readbyte();
	s->readbyte();

	if(n!=0xa)
		__asm{int 3}

	//if(n==9)
	//	ReadClassList( s, cache, object );


	for ( int i=0; i<3; i++ )
		ReadClassList( s, cache, object );
	
	s->seek( 12 );
	
	s->seek(6);

	s->seek(12);

	printf("%s\n",s->readstring().c_str());

	s->readdword();
}
