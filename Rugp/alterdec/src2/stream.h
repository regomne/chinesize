
#pragma once



#include	"core.h"
#include	<stdio.h>
#include	<string>



using namespace std;



class CStream
{
	int		delta;
	byte	temp;

public:
	bool	readclasslist;
	int		redbyte;

			CStream();

	virtual void	read( void *buf, int len ) = 0;
	virtual void	seek( int n ) = 0;

	byte	readbyte();
	word	readword();
	dword	readdword();
	qword	readqword();
	int		readbit();
	int		readunsigned();
	int		readsigned();
	string	readstring();
	int		getreadbyte();
};



class CFileStream : public CStream
{
	FILE *	f;

public:
			CFileStream( FILE *file );
	void	read( void *buf, int len );
	void	seek( int n );
};



class CMemoryStream : public CStream
{
	byte *	b;

public:
			CMemoryStream( void *buf );
	void	read( void *buf, int len );
	void	seek( int n );
};




	