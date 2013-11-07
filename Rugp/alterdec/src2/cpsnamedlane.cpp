#include "common.h"

void ReadCpsNamedLane( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	byte n=s->readbyte();
	s->seek(6);
	if(n>=2)
		s->readdword();
	if(n>=3)
	{
		byte n=s->readbyte();
		if(n!=0)
		{
			ReadClassList(s,cache,object);
			word n=s->readword();
			s->readdword();
			if(n<=4)
				n=4;
			while(n--)
			{
				ReadClassList(s,cache,object);
			}
		}
	}
}
