
#include	"common.h"



static string	ReadClassName( CStream *s );
static string	DecodeClassName( byte *buf, int len );

/*
 *	ReadCrypted
 *		返り値はfreeで開放すること
 */
byte *ReadCrypted( CStream *s, dword key, dword* pSize)
{
	dword size1 = ~( s->readdword() ^ 0xc92e568b );
	dword size2 = ( s->readdword() ^ 0xc92e568f ) / 8;

	byte *buf = (byte *)malloc( size2 );

	if ( ! buf )
		throw "メモリの確保に失敗しました。";

	dword k = key;
	word c = 0;

	for ( int i=0; i<(int)size2; i++ )
	{
		buf[i] = s->readbyte() ^ (byte)k;

		c += buf[i] * ( 0x20 - i % 0x20 );

		k = ~( k*2 + 0xa3b376c9 + ( (k>>15) & 1 ) );

		if ( i % 0x20 == 0x1f )
		{
			word check = s->readword();

			if ( c != check )
				throw "パリティが一致しませんでした。";
			c = 0;
		}
	}

	if(pSize)
		*pSize=size2;

	return buf;
}



/*
 *	ReadClassList
 */
void ReadClassList( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	if ( ! s->readclasslist )
	{
		int n = s->readbyte();
		for ( int i=0; i<n; i++ )
			cache->push_back( CLASS() );
		s->readclasslist = true;
	}

	CLASS cls;
	
	if ( ReadClass( s, cache, &cls ) )
	{
		int flag = s->readword();

		if ( flag & 0x40 )
		{
			cache->push_back( cls );

			OBJECT o( cls.name, cls.schema, object->offset, object->size );
			ReadObject( s, cache, &o );
		}
		else
		{
			dword offset = DecodeOffset( s->readdword() );
			dword size = DecodeSize( s->readdword() );

			ObjectList.Add( cls.name, cls.schema, offset, size );

			ReadClassList( s, cache, object );
		
			if ( ( flag & 0x7 ) == 1 )
				s->readdword();
			else
				if ( flag & 0x8000 )
					s->readbyte();
				else
					s->readword();

			cache->push_back( cls );
		}
	}
}



/*
 *	ReadClass
 */
bool ReadClass( CStream *s, vector<CLASS> *cache, CLASS *cls )
{
	dword w = s->readword();

	if(w==0x7fff)
		throw "Class Index 32位";
	if ( w == 0xffff )
	{
		cls->schema = s->readword();
		cls->name = ReadClassName( s );

		cache->push_back( *cls );

		return true;
	}
	else if ( w & 0x8000 )
	{
		int n = w & 0x7fff;

		if ( n >= (int)cache->size() )
			throw "Class index 引用超出";

		*cls = (*cache)[n];

		return true;
	}
	else
	{
		return false;
	}
}



/*
 *	ReadRawClass
 */
bool ReadRawClass( CStream *s, vector<CLASS> *cache, CLASS *cls )
{
	word w = s->readword();
	if(w==0x7fff)
		throw "7fff error in ReadRawClass";

	if ( w == 0xffff )
	{
		cls->schema = s->readbyte();
		cls->name = s->readstring();

		cache->push_back( *cls );

		return true;
	}
	else if ( w & 0x8000 )
	{
		int n = w & 0x7fff;

		if ( n >= (int)cache->size() )
			throw "クラスキャッシュの範囲外が参照されました。";

		*cls = (*cache)[n];

		return true;
	}
	else
	{
		return false;
	}
}



/*
 *	ReadArchiveClass
 */
bool ReadArchiveClass( CStream *s, vector<CLASS> *cache, CLASS *cls )
{
	word w = s->readword();
	if(w==0x7fff)
		throw "7fff error in ReadArcClass";

	if ( w == 0xffff )
	{
		cls->schema = s->readword();

		int n = s->readword();
		char *name = new char[n+1];
		s->read( name, n );
		name[n] = '\0';
		cls->name = name;
		delete[] name;

		cache->push_back( *cls );

		return true;
	}
	else if ( w & 0x8000 )
	{
		int n = w & 0x7fff;

		if ( n >= (int)cache->size() )
			throw "クラスキャッシュの範囲外が参照されました。";

		*cls = (*cache)[n];

		return true;
	}
	else
	{
		return false;
	}
}



/*
 *	ReadArchiveObject
 */
bool ReadArchiveObject( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	CLASS	cls;

	if ( ! ReadArchiveClass( s, cache, &cls ) )
		return false;
	cache->push_back( cls );

	OBJECT o( cls.name, cls.schema, object->offset, object->size );

	ReadObject( s, cache, &o );

	return true;
}



/*
 *	ReadClassName
 */
string ReadClassName( CStream *s )
{
	int len = s->readbyte();

	byte buf[256];
	s->read( buf, len );

    return DecodeClassName( buf, len );	
}



/*
 *	DecodeClassName
 */
string DecodeClassName( byte *buf, int len )
{
	int		s;		// * 8
	int		t;
	int		i;
	string	name;

	s = 0;

	if ( ( buf[s/8]&(1<<s%8) ) == 0 )
		name += 'C';
	s++;

	while ( 1 )
	{
		if ( s >= len*8 )
			break;

		if ( ( buf[s/8]&(1<<s%8) ) == 0 )
		{
			s++;

			if ( s+3 >= len*8 )
				break;
	
			t = 0;
			for ( i=0; i<4; i++ )
			{
				if ( buf[s/8]&(1<<s%8) )
					t |= 1<<i;
				s++;
			}

			name += "eaitrosducmnSglR"[t];
		}
		else
		{
			s++;

			if ( s >= len*8 )
				break;

			if ( ( buf[s/8]&(1<<s%8) ) == 0 )
			{
				s++;

				if ( s+3 >= len*8 )
					break;

				t = 0;
				for ( i=0; i<4; i++ )
				{
					if ( buf[s/8]&(1<<s%8) )
						t |= 1<<i;
					s++;
				}
				
				if ( t != 0 )
					name += "_COFLfBMxphyAVbI"[t];
				else
				{
					if ( s+7 >= len*8 )
						break;

					t = 0;
					for ( i=0; i<8; i++ )
					{
						if ( buf[s/8]&(1<<s%8) )
							t |= 1<<i;
						s++;
					}
					
					name += t;
				}
			}
			else
			{
				s++;

				if ( s+4 >= len*8 )
					break;
				
				t = 0;
				for ( i=0; i<5; i++ )
				{
					if ( buf[s/8]&(1<<s%8) )
						t |= 1<<i;
					s++;
				}

				name += "EHTDPWXkqvNjwGz02U_K15JQZ467839"[t];
			}
		}
	}

	return string( name.c_str() );	//	末尾の'\0'を除く
}



/*
 *	DecodeOffset
 *		実際のオフセットの半分
 */
dword DecodeOffset( dword offset )
{
	return offset - 0xa2fb6ad1;
}



/*
 *	DecodeSize
 */
dword DecodeSize( dword size )
{
	dword a, c, d;

	a = size - 0xe7b5d9f8;
	c = a >> 0x0d;
	d = c & 0xfff;
	a -= d;
	return (a<<0x13) | c;
}

void Log(char* s)
{
	fprintf(gfp,"%s\n",s);
}