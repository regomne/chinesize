#include <cstring>
class ArcList
{
public:
	char Name[ 16 ] ;
	char *FileStart ;
	int FileSize ;
	char zzz[ 8 ] ;
} ;

class ARC
{
public:
	int FileListSize ;
	int FileNum ;
	ArcList *OneFile ;
	char *List ;
	char *OneBuf ;
	char *OneName ;

	ARC( ) ;
	~ARC( ) ;
	bool ARCopen( wchar_t *name ) ;
	bool ARCread( int OrderNum ) ;
private:
	FILE *pf ;
} ;
ARC::ARC( )
{
	FileListSize = 0 ;
	List = 0 ;
	OneBuf = 0 ;
	pf = NULL ;
} 

ARC::~ARC( ) 
{
	if ( pf != NULL ) fclose( pf ) ;
	if ( List != 0 ) delete [ ] List ;
	if ( OneBuf != 0 ) delete [ ] OneBuf ;
	pf = 0 ;
	List = 0 ;
	OneBuf = 0 ;
}

bool ARC::ARCopen( wchar_t *name )
{
	char temp[ 13 ] ;
	temp[ 12 ] = 0 ;

	//pf = fopen( name , "rb" ) ;
	pf = _wfopen( name , _T( "rb" ) ) ;
	if ( pf == NULL ) return false ;
	fread( &temp[ 0 ] , 1 , 12 , pf ) ;
	if ( strcmp( &temp[ 0 ] , "PackFile    " ) != 0 ) return false ;
	fread( &FileNum , 4 , 1 , pf ) ;

	FileListSize = FileNum * 32 ;
	if ( List != 0 ) delete [ ] List ;
	List = 0 ;
	List = new char[ FileListSize ] ;
	fread( List , 1 , FileListSize , pf ) ;

	return true ;
}




bool ARC::ARCread( int OrderNum )
{
	OneName =  List + OrderNum * 32 ;
	OneFile = ( ArcList * )OneName ;
	OneBuf = new char[ OneFile->FileSize ] ;

	fseek( pf , 0 , 0 ) ;
	fseek( pf , ( long )( OneFile->FileStart + FileListSize + 16 ) , 0 ) ;
	if ( fread( OneBuf , 1 ,  OneFile->FileSize , pf ) != OneFile->FileSize ) return false ;
	return true ;
}

	




