
#include	"common.h"



static void		Sub1( CStream *s, vector<CLASS> *cache, word flag );
static void		Sub2( CStream *s, vector<CLASS> *cache );



/*
 *	ReadRelicUnitedGameProject
 */
void ReadRelicUnitedGameProject( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	cache->resize( 2 );
	s->readclasslist = true;

	s->readdword();
	s->readword();
	s->readword();
	
	CLASS cls;
	ReadClass( s, cache, &cls );

	cache->push_back( cls );
	
	Sub1( s, cache, 0 );

	byte *buf = ReadCrypted( s, 0x7e6b8ce2 );

	CMemoryStream m( buf );
	
	m.readdword();

	for ( int i=0; i<9; i++ )
		ReadClassList( &m, cache, object );

	free( buf );
}



/*
 *	Sub1
 */
void Sub1( CStream *s, vector<CLASS> *cache, word flag )
{
	if ( flag & 0x0200 )
		return;

	word n = s->readword();

	for ( int i=0; i<n; i++ )
		Sub2( s, cache );
}



/*
 *	Sub2
 */
void Sub2( CStream *s, vector<CLASS> *cache )
{
	word	flag = s->readword();
	word	w;
	CLASS	cls;
	dword	size, offset;

	switch ( flag & 7 )
	{
	case 0:
		if ( flag & 0x8000 )
			s->readbyte();
		else
			s->readword();
		
		ReadClass( s, cache, &cls );
		offset = DecodeOffset( s->readdword() );
		size = DecodeSize( s->readdword() );

		ObjectList.Add( cls.name, cls.schema, offset, size );

		Sub1( s, cache, flag );
		
		break;

	case 1:
		s->readdword();

		w = s->readword();
		if ( w == 0x2d6b )
		{
			cls.schema = s->readword();

			word len = s->readword();
			char *name = new char[len+1];
			s->read( name, len );
			name[len] = '\0';
			cls.name = string( name );
			delete[] name;
		}
		else if ( w == 0x1e57 )
		{
			ReadClass( s, cache, &cls );
		}
		else
			throw "relicUnitedGameProject";

		offset = DecodeOffset( s->readdword() );
		size = DecodeSize( s->readdword() );

		ObjectList.Add( cls.name, cls.schema, offset, size );

		Sub1( s, cache, 0 );

		break;

	default:
		throw "relicUnitedGameProject";
	}
}
