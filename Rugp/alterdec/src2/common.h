
#pragma once

/*
 *  this solution support 2 versions of rugp.
 *
 *  version 1: rugp 6.10.09D
 *    muvluv win7 & muvluv alternative win7 
 *  version 2: rugp 5.91.04
 *    muvluv chronicles 01
 */
#define RUGP_VERSION 2


#include	"stream.h"
#include	"objectlist.h"
#include	"imagewriter.h"
#include	<string>
#include	<vector>



using namespace std;

extern FILE* gfp;

void	ReadObject( CStream *s, vector<CLASS> *cache, const OBJECT *object );	//	main.cpp

byte *	ReadCrypted( CStream *s, dword key, dword* pSize=0 );
void	ReadClassList( CStream *s, vector<CLASS> *cache, const OBJECT *object );
bool	ReadClass( CStream *s, vector<CLASS> *cache, CLASS *cls );
bool	ReadRawClass( CStream *s, vector<CLASS> *cache, CLASS *cls );
bool	ReadClass2( CStream *s, vector<CLASS> *cache, CLASS *cls );
bool	ReadArchiveObject( CStream *s, vector<CLASS> *cache, const OBJECT *object );
dword	DecodeOffset( dword offset );
dword	DecodeSize( dword size );

void Log(char* s);