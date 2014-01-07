
#include	"common.h"


#if RUGP_VERSION==1
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
#elif RUGP_VERSION==2

void ReadPostureMoment( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
    s->readbyte();
    int n=s->readbyte();
    s->readbyte();

    if(n!=0x5)
        __asm{int 3}

    //if(n==9)
    //	ReadClassList( s, cache, object );


    for ( int i=0; i<5; i++ )
        ReadClassList( s, cache, object );

    s->seek( 12 );
    s->seek(8);
    if(s->readbyte()!=0)
    {
        s->seek(0x48);
    }
    s->seek(6);

    s->seek(12);

    printf("%s\n",s->readstring().c_str());
}

#endif