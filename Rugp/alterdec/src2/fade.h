
#pragma once



#include	"common.h"



void	ReadFadeMergeBlack( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsRatio( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadPmBgm( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeSdtRatio( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeOverStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsRasterNoize( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeCarten( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeQubeStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsSqrRaster_HRasterV_VRasterH( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsSrcRotate( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsCircleRaster( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeXsOverStretchAnti( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeNormal( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeMergeWhite( CStream *s, vector<CLASS> *cache, const OBJECT *object );
void	ReadFadeTelevisionWipe( CStream *s, vector<CLASS> *cache, const OBJECT *object );

