
#include	"common.h"
#include	"textwriter.h"



extern void		ReadOM( CStream *s, vector<CLASS> *cache, const OBJECT *object, string om, CTextWriter* );	//	rsa2.cpp
static void		ReadVmSound( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmFlagOp( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmGenericMsg( CStream *s, vector<CLASS> *cache, const OBJECT *object, CTextWriter* );
static void		ReadVmCall( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmSync( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmLabel( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmJump( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmRet( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmBlt( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		ReadVmMsg( CStream *s, vector<CLASS> *cache, const OBJECT *object, CTextWriter *writer );
static void		ReadVmImage( CStream *s, vector<CLASS> *cache, const OBJECT *object );
static void		Sub1( CStream *s, vector<CLASS> *cache, const OBJECT *object );

dword CalcHash(byte* s,dword len)
{
	dword hash=0;
	for(dword i=0;i<len;i++)
	{
		hash+=(hash<<5)+byte(s[i]);
	}
	return hash;
}


void ReadRsa( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	dword offset = object->offset + s->getreadbyte() / 2;

	CTextWriter writer;
	
	if ( s->readword() != 0x36e )
		throw "ReadRsa";
	s->readword();
	s->readbyte();
	s->readdword();
	
	dword bufSize;

	byte *buf = ReadCrypted( s, 0x7e6b8ce2, &bufSize );
	dword blockHash=CalcHash(buf,bufSize);

	CMemoryStream m( buf );

	vector<CLASS> mcache;
	mcache.push_back( CLASS() );
	mcache.push_back( CLASS() );

    m.readword();
	m.readword();
	m.readdword();
	m.readdword();
	m.readdword();
	int n = m.readdword();
	m.readdword();

    for ( int i=0; i<n; i++ )
	{
		CLASS cls;

		ReadClass( &m, &mcache, &cls );

		printf( "%s\n", cls.name.c_str() );

		if ( cls.name == "CVmSound" )  ReadVmSound( &m, &mcache, object );
		else if ( cls.name == "CVmFlagOp" )  ReadVmFlagOp( &m, &mcache, object );
		else if ( cls.name == "CVmGenericMsg" )  ReadVmGenericMsg( &m, &mcache, object, &writer );
		else if ( cls.name == "CVmCall" )  ReadVmCall( &m, &mcache, object );
		else if ( cls.name == "CVmSync" )  ReadVmSync( &m, &mcache, object );
		else if ( cls.name == "CVmLabel" )  ReadVmLabel( &m, &mcache, object );
		else if ( cls.name == "CVmJump" )  ReadVmJump( &m, &mcache, object );
		else if ( cls.name == "CVmRet" )  ReadVmRet( &m, &mcache, object );
		else if ( cls.name == "CVmBlt" )  ReadVmBlt( &m, &mcache, object );
		else if ( cls.name == "CVmMsg" )  ReadVmMsg( &m, &mcache, object, &writer );
		else if ( cls.name == "CVmImage" )  ReadVmImage( &m, &mcache, object );
		else
			throw "未対応のVMクラスが見つかりました。";
	}

	free( buf );

	writer.write( offset ,blockHash);
}



void ReadVmSound( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );
	s->readbyte();

	byte f = s->readbyte();

	s->readword();
	s->readword();
	s->readword();

	if ( f & 0x4 )
	{
		if ( f & 0x8 )
		{
			ReadClassList( s, cache, object );
		}
		else
		{
			s->readword();
			s->readword();
			s->readword();
			s->readword();

			if ( ! ( f & 0x10 ) )
			{
				s->readword();
				s->readword();
				s->readword();
			}
		}
	}
}



void ReadVmFlagOp( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	for ( int i=0; i<2; i++ )
		Sub1( s, cache, object );
	ReadClassList( s, cache, object );
	s->readword();
	s->readbyte();
	s->readdword();
}



void ReadVmGenericMsg( CStream *s, vector<CLASS> *cache, const OBJECT *object, CTextWriter* writer )
{
	s->readdword();
	s->readdword();
	
	Sub1( s, cache, object );

	CLASS cls;
	ReadRawClass( s, cache, &cls );

	printf( "%s\n", cls.name.c_str() );

	cache->push_back( cls );

	ReadOM( s, cache, object, cls.name, writer );

	s->readword();
	word n = s->readword();

	for ( int i=0; i<n; i++ )
	{
		word w = s->readword();

		CLASS cls;

		if ( w == 0x1e57 )
			ReadClass( s, cache, &cls );
		else if ( w == 0x369e )
			ReadRawClass( s, cache, &cls );
		else if ( w == 0x2d6b  ||  w == 0x2f1a )
		{
			s->readword();			//	?
			int n = s->readword();
			s->seek( n );
		}
		else
			throw "VmGenericMsg错误";

		dword w2=s->readword();
		if(w2==0x7fff)
			s->readdword();
		if ( w2 == 0xffff )
		{
			if(0)
			{
				char paramname[256];
				word len = s->readword();
				if ( len >= 256 )
					throw "パラメタ名が長すぎます。";
				s->read( paramname, len );
				paramname[len] = '\0';
				printf( "%s\n", paramname );

				CLASS cls;
				cls.name = paramname;
			
				cache->push_back( cls );
			}
			else
			{
				CLASS cls;
				s->readdword();
				cache->push_back( cls );
			}
		}
		
		Sub1( s, cache, object );
	}
}


void ReadVmCall( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );
	word n = s->readword();

	for ( int i=0; i<n; i++ )
	{
		int d=s->readdword();
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
				throw "VmCall Error";

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
				throw "Rsa.cpp Sub1";
			}
		}
	}
}



void ReadVmSync( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );
	s->readbyte();
	s->readdword();

	
}

void ReadVmLabel( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );

	
}

void ReadVmJump( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	ReadClassList( s, cache, object );

	
}

void ReadVmRet( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();

	
}

void ReadVmBlt( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();

	CLASS cls;
	if ( ReadClass( s, cache, &cls ) )
	{
		cache->push_back( cls );
		OBJECT o( cls.name, cls.schema, object->offset, object->size );
		ReadObject( s, cache, &o );
	}

	s->readword();
	s->readbyte();
	s->readbyte();
	for(int i=0;i<4;i++)
		Sub1( s, cache, object );
}



/*
 *	ReadVmMsg
 */
void ReadVmMsg( CStream *s, vector<CLASS> *cache, const OBJECT *object, CTextWriter *writer )
{
	dword num=s->readdword();
	dword flag = s->readdword();
	ReadClassList( s, cache, object );

	dword offset=s->getreadbyte();

	string sb;
	char buf[1024];

	dword n = s->readbyte();
	if(n==0xff)
		n=s->readword();
	if(n>=1024)
		throw "文本超长！";

	s->read(buf,n);
	
	if(n>1)
	{
		dword strHash=CalcHash((byte*)buf,n);
		dword strHash2=0;
		if(n>4)
			strHash2=CalcHash((byte*)buf+4,n-4);
		for(int i=0;i<n;i++)
		{
			if(buf[i]=='\n')
				sb+="\\n";
			else if((byte)buf[i]<0x20)
			{
				char temp[8];
				sprintf(temp,"\\%02d",(byte)buf[i]);
				sb+=temp;
			}
			else
				sb+=buf[i];
		}
		printf("%s\n",sb.c_str());
		writer->addLine(sb,num,strHash,strHash2);
	}
	
	ReadClassList( s, cache, object );

	if ( flag & 0x4000000 )
		ReadClassList( s, cache, object );

	if ( flag & 0x200000 )
	{
		word w = s->readword();
		if(w&4)
			printf("%s\n",s->readstring().c_str());
		if(w&0x40)
			printf("%s\n",s->readstring().c_str());
		if(w&0x20)
			s->readdword();
		if ( (w&3) != 0 )
		{       
			int n = s->readword();

			for ( int i=0; i<n; i++ )
			{
				if(w&8)
					s->readword();
				ReadClassList( s, cache, object );
				if ( (w&3) == 3 )
					ReadClassList( s, cache, object );
			}
		}
	}
}


/*
 *	ReadVmImage
 */
void ReadVmImage( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readbyte();
	s->readbyte();
	
	Sub1( s, cache, object );
	Sub1( s, cache, object );
}

/*
 *	Sub1
 */
void Sub1( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	dword d = s->readdword();

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
		throw "Rsa.cpp Sub1";
	}
}
