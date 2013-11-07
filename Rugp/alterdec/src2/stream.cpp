
#include	"stream.h"
#include	<string.h>



/*
 *	CStream
 */
CStream::CStream()
{
	delta = 0;
	temp = 0;
	readclasslist = false;
	redbyte = 0;
}



/*
 *	readbyte/readword/readdword/readqword
 */
byte CStream::readbyte()
{
	byte b;
	read( &b, 1 );
	return b;
}

word CStream::readword()
{
	word w;
	read( &w, 2 );
	return w;
}

dword CStream::readdword()
{
	dword d;
	read( &d, 4 );
	return d;
}

qword CStream::readqword()
{
	qword q;
	read( &q, 8 );
	return q;
}



/*
 *	readbit
 */
int CStream::readbit()
{
	if ( delta == 0 )
		temp = readbyte();

	int bit = temp & 1;

	temp >>= 1;
	delta = ( delta + 1 ) % 8;
	
	return bit;
};



/*
 *	readunsigned
 */
int CStream::readunsigned()
{
	if ( readbit() == 0 )
		return 0;

	int t = 1;

	do
	{
		t = ( t << 1 ) | readbit();
	}
	while ( readbit() != 0 );

	return t - 1;
}



/*
 *	readsigned
 */
int CStream::readsigned()
{
	if ( readbit() == 0 )
		return 0;

	int s = 1 - readbit() * 2;

	int t = 1;

	for ( int i=0; i<6 && readbit()!=0; i++ )
		t = ( t << 1 ) | readbit();
	
	return s * t;
}



/*
 *	readstring
 */
string CStream::readstring()
{
    char buf[257];

	int len = readbyte();	//	2byteÒÔÉÏ¤ÎˆöºÏ¤¢¤ê
    if(len==0xff)
    {
        len=readword();
        auto nbuf=new char[len+1];
        read(nbuf,len);
        nbuf[len]=0;
        return string(nbuf);
    }
	read( buf, len );
	buf[len] = '\0';

	return string( buf );
}



/*
 *	getreadbyte
 */
int CStream::getreadbyte()
{
	return redbyte;
}



/*
 *	CFileStream
 */
CFileStream::CFileStream( FILE *file )
{
	f = file;
}

/*
 *	read
 */
void CFileStream::read( void *buf, int len )
{
	if ( fread( buf, 1, len, f ) != len )
		throw "¥Õ¥¡¥¤¥ë¤ÎÕi¤ßŞz¤ß¤ËÊ§”¡¤·¤Ş¤·¤¿¡£";
	redbyte += len;
}

/*
 *	seek
 */
void CFileStream::seek( int n )
{
	if ( fseek( f, n, SEEK_CUR ) != 0 )
		throw "¥Õ¥¡¥¤¥ë¤Î¥·©`¥¯¤ËÊ§”¡¤·¤Ş¤·¤¿¡£";
	redbyte += n;
};




/*
 *	CMemoryStream
 */
CMemoryStream::CMemoryStream( void *buf )
{
	b = (byte *)buf;
}

/*
 *	read
 */
void CMemoryStream::read( void *buf, int len )
{
	memcpy( buf, b, len );
	b += len;
	redbyte += len;
}

/*
 *	seek
 */
void CMemoryStream::seek( int n )
{
	b += n;
	redbyte += n;
};




	