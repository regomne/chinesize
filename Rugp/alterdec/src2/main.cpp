/*
 *  this solution support 2 versions of rugp.
 *
 *  version 1: rugp 6.10.09D
 *    muvluv win7 & muvluv alternative win7 
 *  version 2: rugp 5.91.04
 *    muvluv chronicles 01
 */
#include	"common.h"
#include	"fade.h"
#include	<time.h>
#include	<conio.h>
#include <hash_map>


void	ReadRelicUnitedGameProject( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadBoxOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadProcessOcean( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadRsa( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadPostureMoment( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFlashLayerEffect( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadTWFGaugeBox( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadCharBox( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadTWFRemainderBox( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadR6Ti( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadBg2d( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadPmEffect( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadRsi( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadRbx( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadImgSel( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadRip( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadMoveAcsOngen( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadAcsPos( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadMN_Time_Bezier( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadMN_Time_2G( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadNormalCamera( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadPmVideoRef( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadRip007( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadCpsNamedLane( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void    ReadLayoutTextImg(CStream *s, vector<CLASS> *cache, const OBJECT *object );
void    ReadDcAgesModelQsT( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void    ReadS5i( CStream *s, vector<CLASS> *cache, const OBJECT *object );

typedef FILE* (*OpenFileRoutine)( const char *filename, dword offset );
void	Decode( const char *filename , dword,OpenFileRoutine);
FILE *	OpenRio( const char *filename, dword offset );
FILE *	OpenRio2( const char *filename, dword offset );
void	ReadNull( CStream *s, vector<CLASS> *cache, const OBJECT *object );

void InitOMFuncTable();
void InitClassFuncTable();


FILE* gfp;
typedef void (*ClassFunction)(CStream *s, vector<CLASS> *cache, const OBJECT *object);
hash_map<string, ClassFunction> ClassFuncTable;

extern vector<dword> sOffsets;

int main( int argc, char **argv )
{
	try
	{
		//if ( argc != 2 )
		//	throw "alterdec Alternative.rio";
		InitOMFuncTable();
		InitClassFuncTable();

		gfp=fopen("log.txt","w");
		if(gfp==0)
			throw "Error000!";

        //rio中查找0x1edb927c，该位置为根对象所在地
        Decode("D:\\Program Files\\マブラヴオルタネイティヴクロニクルズ 01\\オルタクロニクルズ01.rio",
            0x20ecf,OpenRio);
		//Decode( "D:\\Program Files\\マブラヴ14\\muvluv14.rio",
  //          0x335b6/2, OpenRio2);
		//Decode( "D:\\Program Files\\マブラヴオルタネイティヴ14\\alternative14.rio",
  //          0x53a98/2, OpenRio2);

		FILE* fbb=fopen("a.b","wb");
		for(int i=0;i<sOffsets.size();i++)
		{
			fwrite(&sOffsets[i],1,4,fbb);
		}
		fclose(fbb);

		printf( "%.2f[s]\n", (double)clock() / CLOCKS_PER_SEC );
	}
	catch ( const char *str )
	{
		fclose(gfp);
		printf( "%s\n", str );
	}
	getch();
}



void Decode( const char *filename , dword firstOffset, OpenFileRoutine openf)
{
    ObjectList.Add( "CrelicUnitedGameProject", 0, firstOffset, 100 );
	
	for ( int i=0; i<ObjectList.GetNumber(); i++ )
	{
		OBJECT object = ObjectList.Get( i );

		printf( "---- %d/%d %s %x %x %x\n", i, ObjectList.GetNumber(), object.name.c_str(), object.schema, object.offset, object.size );

		if(i==0x248)
		{
			int a=3;
		}

        FILE *f = openf( filename, object.offset );

        CFileStream fs( f );
		vector<CLASS> cache;
		
		for ( int j=0; j<5; j++ )
			cache.push_back( CLASS() );

		if ( object.name != "CrelicUnitedGameProject" )
			fs.seek( 3 );

		ReadObject( &fs, &cache, &object );

		fclose( f );
	}
}



/*
 *	ReadObject
 */
void ReadObject( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	printf("--Reading object: %s\n",object->name.c_str());
	if ( object->size <= 3 )
		return;

	hash_map<string,ClassFunction>::iterator it=ClassFuncTable.find(object->name);
	if(it==ClassFuncTable.end())
	{
		printf( "%s\n", object->name.c_str() );
		throw "未対応のクラスが見つかりました。";
	}
	else
	{
		it->second(s,cache,object);
	}

};



/*
 *	OpenRio
 *		offsetから適切なファイルを開きシーク
 */
FILE *OpenRio2( const char *filename, dword offset )
{
	static string name;
	static vector<dword> filesize;

	FILE *f;
	
	//	ファイルサイズの取得

	if ( name != filename )
	{
		name = filename;

		filesize.clear();

		f = fopen( filename, "rb" );
		if ( f == NULL )
			throw "ファイルを開けませんでした。";
		fseek( f, 0, SEEK_END );
		dword s = (dword)ftell( f );
		filesize.push_back( s );
		fclose( f );
	
		char *fn = new char[strlen(filename)+5];

		for ( int i=2; ; i++ )
		{
			sprintf( fn, "%s.%03d", filename, i );

			f = fopen( fn, "rb" );
			if ( f == NULL )
				break;
			fseek( f, 0, SEEK_END );
			s = (dword)ftell( f );
			filesize.push_back( s );
			fclose( f );
		}

		delete[] fn;
	}

	//

	if ( offset < filesize[0] / 2 )
	{
		f = fopen( filename, "rb" );
		if ( f == NULL )
			throw "ファイルを開けませんでした。";
		fseek( f, offset*2, SEEK_SET );

		return f;
	}
	
	int n;

	for ( n=0; ; n++ )
	{
		if ( n >= (int)filesize.size() )
			throw "オフセットが大きすぎます。";
		if ( offset < filesize[n] / 2 )
			break;
		offset -= filesize[n] / 2;
	}

	char *fn = new char[strlen(filename)+5];

	sprintf( fn, "%s.%03d", filename, n+1 );

	f = fopen( fn, "rb" );
	if ( f == NULL )
		throw "ファイルを開けませんでした。";
	fseek( f, offset*2, SEEK_SET );

	delete[] fn;
	
    return f;
}

FILE *OpenRio( const char *filename, dword offset )
{
    static string name;
    static vector<dword> filesize;

    FILE *f;

    //	ファイルサイズの取得

    if ( name != filename )
    {
        name = filename;

        filesize.clear();

        f = fopen( filename, "rb" );
        if ( f == NULL )
            throw "ファイルを開けませんでした。";
        fseek( f, 0, SEEK_END );
        dword s = (dword)ftell( f );
        filesize.push_back( s );
        fclose( f );

        char *fn = new char[strlen(filename)+5];

        for ( int i=2; ; i++ )
        {
            sprintf( fn, "%s.%03d", filename, i );

            f = fopen( fn, "rb" );
            if ( f == NULL )
                break;
            fseek( f, 0, SEEK_END );
            s = (dword)ftell( f );
            filesize.push_back( s );
            fclose( f );
        }

        delete[] fn;
    }

    //

    if ( offset < filesize[0] )
    {
        f = fopen( filename, "rb" );
        if ( f == NULL )
            throw "ファイルを開けませんでした。";
        fseek( f, offset, SEEK_SET );

        return f;
    }

    int n;

    for ( n=0; ; n++ )
    {
        if ( n >= (int)filesize.size() )
            throw "オフセットが大きすぎます。";
        if ( offset < filesize[n] )
            break;
        offset -= filesize[n];
    }

    char *fn = new char[strlen(filename)+5];

    sprintf( fn, "%s.%03d", filename, n+1 );

    f = fopen( fn, "rb" );
    if ( f == NULL )
        throw "ファイルを開けませんでした。";
    fseek( f, offset, SEEK_SET );

    delete[] fn;

    return f;
}

void InitClassFuncTable()
{
	struct funcPair
	{
		char *name;
		ClassFunction func;
	} readfunc[] = {
		{ "CrelicUnitedGameProject", ReadRelicUnitedGameProject },
		{ "CBoxOcean", ReadBoxOcean },
		{ "CProcessOcean", ReadProcessOcean },
		{ "CRsa", ReadRsa },
		{ "CPostureMoment", ReadPostureMoment },
		{ "CFlashLayerEffect", ReadFlashLayerEffect },
		{ "CTWFGaugeBox", ReadTWFGaugeBox },
		{ "CCharBox", ReadCharBox },
        { "CCharBox2", ReadCharBox },
		{ "CTWFRemainderBox", ReadTWFRemainderBox },
		{ "Cr6Ti", ReadR6Ti },
		{ "CBg2d", ReadBg2d },
		{ "CFadeNormal", ReadFadeNormal },
		{ "CFadeMergeWhite", ReadFadeMergeWhite },
		{ "CPmEffect", ReadPmEffect },
		{ "CRsi", ReadNull }, //ReadRsi },
		{ "CRbx", ReadRbx },
		{ "CImgSel", ReadImgSel },
		{ "CRip", ReadNull }, //ReadRip },
		{ "CMoveAcsOngen", ReadMoveAcsOngen },
		{ "CFadeMergeBlack", ReadFadeMergeBlack },
		{ "CFadeXsRatio", ReadFadeXsRatio },
		{ "CPmBgm", ReadPmBgm },
		{ "CFadeSdtRatio", ReadFadeNormal },
		{ "CFadeRSideCarten", ReadFadeNormal },
		{ "CFadeMozaik", ReadFadeNormal },
		{ "CFadeMulHorz", ReadFadeNormal },
		{ "CFadeOverStretchAnti", ReadFadeOverStretchAnti },
		{ "CFadeXsRasterNoize", ReadFadeXsRasterNoize },
		{ "CFadeCarten", ReadFadeCarten },
		{ "CFadeQubeStretchAnti", ReadFadeQubeStretchAnti },
		{ "CFadeStretchAnti", ReadFadeStretchAnti },
		{ "CAcsPos", ReadAcsPos },
		{ "CFadeXsSqrRaster_HRasterV_VRasterH", ReadFadeXsSqrRaster_HRasterV_VRasterH },
		{ "CFadeInvert", ReadFadeNormal },
		{ "CFadeXsSrcRotate", ReadFadeXsSrcRotate },
		{ "CFadeShock", ReadFadeNormal },
		{ "CFadeXsCircleRaster", ReadFadeXsCircleRaster },
		{ "CFadeTelevisionWipe", ReadFadeTelevisionWipe },
		{ "CFadeXsOverStretchAnti", ReadFadeXsOverStretchAnti },
		{ "CFadeMergeColor", ReadFadeMergeWhite },
        { "CFadeHSV", ReadFadeHSV },
		{ "CMN_Time_Bezier", ReadMN_Time_Bezier },
		{ "CMN_Time_2G", ReadMN_Time_2G },
		{ "CFadeXsMergeBlack", ReadFadeXsRatio },
		{ "CFadeXsHRasterHOffset", ReadFadeXsSqrRaster_HRasterV_VRasterH },
		{ "CNormalCamera", ReadNormalCamera },
		{ "CPmVideoRef", ReadPmVideoRef},
		{ "CRip007", ReadRip007},
		{ "CCpsNamedLane", ReadCpsNamedLane},
        { "CLayoutTextImg", ReadLayoutTextImg },
        { "CDcAgesModelQsT", ReadDcAgesModelQsT },
        { "CS5i", ReadS5i },

		{ "CObjectOcean", ReadNull },
		{ "CStdb", ReadNull },
		{ "抁惍悢", ReadNull },
		{ "惓惍悢", ReadNull },
		{ "怓", ReadNull },
		{ "惍悢", ReadNull },
		{ "CSbm", ReadNull },
		{ "僶僀僩", ReadNull },
		{ "怓", ReadNull },
		{ "CBox", ReadNull },
		{ "抁惓惍悢", ReadNull },
		{ "CGenericCamera", ReadNull },
		{ "CDbBrowseBox", ReadNull },
		{ "CFrameBuffer", ReadNull },
		{ "CFloatStaffRollBox", ReadNull },
		{ "CImgBox", ReadNull },
		{ "CImgAreaBox", ReadNull },
		{ "CParseBgBox", ReadNull },
		{ "CVmVarObj", ReadNull },
		{ "CMsgSelBox", ReadNull },
		{ "CParseBgBox", ReadNull },
		{ "CTextStaffrollSeq", ReadNull },
		{ "CInt", ReadNull },
		{ "CKiminozoEffectManager", ReadNull },
		{ "CBool", ReadNull },
		{ "CRio", ReadNull },
		{ "CLayerBacklight", ReadNull },
		{ "CLayerSimpleFlush", ReadNull },
		{ "CLayer3dShooting", ReadNull },
		{ "CFontAttr", ReadNull },
		{ "CCharVoiceRegistOb", ReadNull },
		{ "CVideoSeq", ReadNull },
		{ "CWaitableTimer", ReadNull },
		{ "CIcon", ReadNull },
		{ "CCursor", ReadNull },
		{ "CRip008", ReadNull },
		{ "CFrameImage", ReadNull },
		{ "CLabel", ReadNull },
		{ "COptimizedObs", ReadNull },
		{ "CCharVoiceOcean", ReadNull },
		{ "CRmt", ReadNull },
		{ "CBasicArray", ReadNull },
		{ "CMimeFlat16LimitOne", ReadNull },
		{ "CPmBgmSeq", ReadNull },
		{ "CRmti", ReadNull },
        { "CRimp", ReadNull },
        { "CRMG0001Box", ReadNull },
//		{ "", ReadNull },
	};

	for(int i=0;i<sizeof(readfunc)/sizeof(funcPair);i++)
		ClassFuncTable.insert(hash_map<string,ClassFunction>::value_type(readfunc[i].name,readfunc[i].func));

}

/*
 *	ReadNull
 *		未対応クラス用ダミー
 */
void ReadNull( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
}