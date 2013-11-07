#include "common.h"

static void Sub0( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{

	dword d=s->readdword();
	if(d==4)
	{
		word w = s->readword();

		CLASS cls;

		if ( w == 0x1e57 )
			ReadClass( s, cache, &cls );
		else if ( w == 0x369e )
			ReadRawClass( s, cache, &cls );
		else if ( w == 0x2d6b  ||  w == 0x2f1a )
		{
			char paraName[64];
			s->readword();			//	?
			int n = s->readword();
			if(n>=64)
				throw "参数名太长! in REadVmCall";
			s->read(paraName,n);
			paraName[n]='\0';
			Log(paraName);
			//s->seek( n );
		}
		else
			throw "pmvideoref Sub1";

		//s->readdword();
		s->seek(4);
	}
	else
	{
		switch ( d & 0x3 )
		{
		case 0:
			ReadClassList( s, cache, object );
			break;
		case 1:
			printf( "%s\n", s->readstring().c_str() );
			break;
		case 2:
			break;
		default:
			throw "pmvideoref Sub1";
		}
	}
}


void ReadPmVideoRef( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	Sub0(s,cache,object);
	Sub0(s,cache,object);
	ReadClassList(s,cache,object);
	dword n=s->readdword();
	if(n)
	{
		s->seek(36); //2*w+6*d+2*f
	}

	//f f w w w w d d d d
	s->seek(32);
}
