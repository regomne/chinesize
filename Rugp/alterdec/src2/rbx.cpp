
#include	"common.h"



void ReadRbx( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	ReadClassList( s, cache, object );
	ReadClassList( s, cache, object );

	int n = s->readword();

	for ( int i=0; i<n; i++ )
	{
		CLASS cls;	//	41b470
		if ( ReadClass( s, cache, &cls ) )
		{
			cache->push_back( cls );
			OBJECT o( cls.name, cls.schema, object->offset, object->size );
			ReadObject( s, cache, &o );
		}
	}

	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	if ( s->readdword() != 0 )
		throw "ReadRbx";
}
