// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		メモリイメージ制御用ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// 多重インクルード防止用定義
#ifndef __DX_MEMIMG_H__
#define __DX_MEMIMG_H__

// インクルード----------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

#define MEMIMG_VALID(MIMG)			( (MIMG).Image != NULL )
#define MEMIMG_INITIALIZECODE		(0x12345678)

#define MEMIMG_XRGB8_A				(0x00000000)
#define MEMIMG_XRGB8_R				(0x00ff0000)
#define MEMIMG_XRGB8_G				(0x0000ff00)
#define MEMIMG_XRGB8_B				(0x000000ff)

#define MEMIMG_XRGB8_LR				(16)
#define MEMIMG_XRGB8_LG				(8)
#define MEMIMG_XRGB8_LB				(0)

#define MEMIMG_ARGB8_A				(0xff000000)
#define MEMIMG_ARGB8_R				(0x00ff0000)
#define MEMIMG_ARGB8_G				(0x0000ff00)
#define MEMIMG_ARGB8_B				(0x000000ff)

#define MEMIMG_X8A8R5G6B5_A			(0x00ff0000)
#define MEMIMG_X8A8R5G6B5_R			(0x0000f800)
#define MEMIMG_X8A8R5G6B5_G			(0x000007e0)
#define MEMIMG_X8A8R5G6B5_B			(0x0000001f)

#define MEMIMG_R5G6B5_A				(0x0000)
#define MEMIMG_R5G6B5_R				(0xf800)
#define MEMIMG_R5G6B5_G				(0x07e0)
#define MEMIMG_R5G6B5_B				(0x001f)

#define MEMIMG_R5G6B5_LR			(11)
#define MEMIMG_R5G6B5_LG			(5)
#define MEMIMG_R5G6B5_LB			(0)

#define MEMIMG_R5G6B5_ALPHAMASK1	(0x7bef7bef)
#define MEMIMG_R5G6B5_ADDMASK1		(0xf7def7df)
#define MEMIMG_R5G6B5_ADDMASK2		(0x08210820)
#define MEMIMG_R5G6B5_ADDMASK3		(0x08210820)
#define MEMIMG_R5G6B5_ADDSHFT		(5)

#define MEMIMG_R5G6B5_SUBMASK1		MEMIMG_R5G6B5_ADDMASK1
#define MEMIMG_R5G6B5_SUBMASK2		MEMIMG_R5G6B5_ADDMASK2
#define MEMIMG_R5G6B5_SUBMASK3		MEMIMG_R5G6B5_ADDMASK3
#define MEMIMG_R5G6B5_SUBSHFT		MEMIMG_R5G6B5_ADDSHFT

#define MEMIMG_RGB5_R				(0x7c00)
#define MEMIMG_RGB5_G				(0x3e0)
#define MEMIMG_RGB5_B				(0x1f)

#define MEMIMG_RGB5_LR				(10)
#define MEMIMG_RGB5_LG				(5)
#define MEMIMG_RGB5_LB				(0)

#define MEMIMG_XRGB8_ALPHAMASK1		(0x007f7f7f)
#define MEMIMG_XRGB8_ADDMASK1		(0x00fefeff)
#define MEMIMG_XRGB8_ADDMASK2		(0x01010100)
#define MEMIMG_XRGB8_ADDMASK3		(0x00010100)
#define MEMIMG_XRGB8_ADDSHFT		(8)

#define MEMIMG_XRGB8_SUBMASK1		MEMIMG_XRGB8_ADDMASK1
#define MEMIMG_XRGB8_SUBMASK2		MEMIMG_XRGB8_ADDMASK2
#define MEMIMG_XRGB8_SUBMASK3		MEMIMG_XRGB8_ADDMASK3
#define MEMIMG_XRGB8_SUBSHFT		MEMIMG_XRGB8_ADDSHFT

// 固定小数点に使用される小数点ビット数
#define QUALI	(1 << 18)

// ペイント用バッファのサイズ
#define PAINTBUFFERSIZE		(8000)

// アルファ用マスクマクロ
#if 1
#define MK565_5 	(0X00000000)         //ShiftMasking  PixelFormat565
#define MK565_4 	(0X08610861)
#define MK565_3 	(0X18E318E3)
#define MK565_2 	(0X39E739E7)
#define MK565_1 	(0X7BEF7BEF)
#define MK565_0 	(0XFFFFFFFF)
#else
#define MK565_5 	(0X00000000)         //ShiftMasking  PixelFormat565
#define MK565_4 	(0X08410841)
#define MK565_3 	(0X18c318c3)
#define MK565_2 	(0X39c739c7)
#define MK565_1 	(0X7BcF7BcF)
#define MK565_0 	(0XFFFFFFFF)
#endif

#define MK565_R 	(0xf800)
#define MK565_G 	(0x7e0)
#define MK565_B 	(0x1f)

#define MK565_LR 	(11)
#define MK565_LG 	(5)
#define MK565_LB 	(0)

#define MK565_WR 	(5)
#define MK565_WG 	(6)
#define MK565_WB 	(5)

#define MK565_MR 	(0x20)
#define MK565_MG 	(0x40)
#define MK565_MB 	(0x20)
									    //ADD Mask      PixelFormat565
#define MK565_ADD4 	(0x7BEF7BEF)    //桁上がり部分を０にする
#define MK565_ADD5 	(0x84108410)    //桁上がり検出マスク（ADDで使用）
#define MK565_ADD6 	(0x08210821)	//最下位ビットマスク

#define MK565_ADD1 	(0xF7DF)		// 桁上がり部分を０にする
#define MK565_ADD2 	(0x10820)		// 桁上がりビット検出マスク
#define MK565_ADD3 	(0x0820)		// 最下位ビットマスク

#define MK565_SUB1 	(0xF7DF)		// 桁上がり部分を０にする
#define MK565_SUB2 	(0x10820)		// 桁上がりビット検出マスク
#define MK565_SUB3 	(0x0820)		// 最下位ビットマスク


//#define MK565_ADD1 	(0x7BEE7BEE)
//#define MK565_ADD2 	(0x84108410)
//#define MK565_ADD3 	(0x08610861)	//最下位ビットマスク

#define MK565_R5 	(0x00000000)
#define MK565_R4 	(0x08000800)
#define MK565_R3 	(0x18001800)
#define MK565_R2 	(0x38003800)
#define MK565_R1 	(0x78007800)
#define MK565_R0 	(0xf800f800)

#define MK565_G5 	(0x00000000)
#define MK565_G4 	(0x00600060)
#define MK565_G3 	(0x00e000e0)
#define MK565_G2 	(0x01e001e0)
#define MK565_G1 	(0x03e003e0)
#define MK565_G0 	(0x07e007e0)

#define MK565_B5 	(0x00000000)
#define MK565_B4 	(0x00010001)
#define MK565_B3 	(0x00030003)
#define MK565_B2 	(0x00070007)
#define MK565_B1 	(0x000f000f)
#define MK565_B0 	(0x001f001f)

#define MK565_TN 	(64)


#define MK555_5 	(0X00000000)         //ShiftMasking  PixelFormat555
#define MK555_4 	(0X04210421)
#define MK555_3 	(0X0C630C63)
#define MK555_2 	(0X1CE71CE7)
#define MK555_1 	(0X3DEF3DEF)
#define MK555_0 	(0XFFFFFFFF)

#define MK555_R5 	(0x00000000)
#define MK555_R4 	(0x04000400)
#define MK555_R3 	(0x0c000c00)
#define MK555_R2 	(0x1c001c00)
#define MK555_R1 	(0x3c003c00)
#define MK555_R0 	(0x7c007c00)

#define MK555_G5 	(0x00000000)
#define MK555_G4 	(0x00200020)
#define MK555_G3 	(0x00600060)
#define MK555_G2 	(0x00e000e0)
#define MK555_G1 	(0x01e001e0)
#define MK555_G0 	(0x03e003e0)

#define MK555_B5 	(0x00000000)
#define MK555_B4 	(0x00010001)
#define MK555_B3 	(0x00030003)
#define MK555_B2 	(0x00070007)
#define MK555_B1 	(0x000f000f)
#define MK555_B0 	(0x001f001f)

#define MK555_R 	(0x7c00)
#define MK555_G 	(0x3e0)
#define MK555_B 	(0x1f)

#define MK555_LR 	(10)
#define MK555_LG 	(5)
#define MK555_LB 	(0)

#define MK555_WR 	(5)
#define MK555_WG 	(5)
#define MK555_WB 	(5)

#define MK555_MR 	(0x20)
#define MK555_MG 	(0x20)
#define MK555_MB 	(0x20)

#define MK555_TN 	(64)

/*									    //ADD Mask      PixelFormat555
#define MK555_ADD1 	(0x3DEF3DEF)      //桁上がり部分を０にする
//#define MK555_ADD2 	(0x42104210)      //桁上がり検出マスク（ADDで使用）

//#define MK555_ADD1 	(0x3DEE3DEE)
#define MK555_ADD2 	(0x42104210)
#define MK555_ADD3 	(0x04210421)	//最下位ビットマスク
*/

#define MK555_ADD4 	(0x3DEF3DEF)    //桁上がり部分を０にする
#define MK555_ADD5 	(0x42104210)    //桁上がり検出マスク（ADDで使用）
#define MK555_ADD6 	(0x04210421)	//最下位ビットマスク

#define MK555_ADD1 	(0x7BDF)		// 桁上がり部分を０にする
#define MK555_ADD2 	(0x8420)		// 桁上がりビット検出マスク
#define MK555_ADD3 	(0x0420)		// 最下位ビットマスク

#define MK555_SUB1 	(0x7BDF)		// 桁上がり部分を０にする
#define MK555_SUB2 	(0x8420)		// 桁上がりビット検出マスク
#define MK555_SUB3 	(0x0420)		// 最下位ビットマスク



#define MK888_5 	(0X00070707)         //ShiftMasking  PixelFormat888
#define MK888_4 	(0X000F0F0F)
#define MK888_3 	(0X001F1F1F)
#define MK888_2 	(0X003F3F3F)
#define MK888_1 	(0X007F7F7F)
#define MK888_0 	(0X00FFFFFF)

#define MK888_R5 	(0x00070000)
#define MK888_R4 	(0x000f0000)
#define MK888_R3 	(0x001f0000)
#define MK888_R2 	(0x003f0000)
#define MK888_R1 	(0x007f0000)
#define MK888_R0 	(0x00ff0000)

#define MK888_G5 	(0x00000700)
#define MK888_G4 	(0x00000f00)
#define MK888_G3 	(0x00001f00)
#define MK888_G2 	(0x00003f00)
#define MK888_G1 	(0x00007f00)
#define MK888_G0 	(0x0000ff00)

#define MK888_B5 	(0x00000007)
#define MK888_B4 	(0x0000000f)
#define MK888_B3 	(0x0000001f)
#define MK888_B2 	(0x0000003f)
#define MK888_B1 	(0x0000007f)
#define MK888_B0 	(0x000000ff)


#define MK888_R 	(0xFF0000)
#define MK888_G 	(0x00FF00)
#define MK888_B 	(0x0000FF)

#define MK888_LR 	(16)
#define MK888_LG 	(8)
#define MK888_LB 	(0)

#define MK888_WR 	(8)
#define MK888_WG 	(8)
#define MK888_WB 	(8)

#define MK888_MR 	(0x100)
#define MK888_MG 	(0x100)
#define MK888_MB 	(0x100)

#define MK888_TN 	(256)

/*									    //ADD Mask      PixelFormat565
#define MK888_ADD1 	(0x7F7F7F)      //桁上がり部分を０にする
//#define MK888_ADD2 	(0x808080)      //桁上がり検出マスク（ADDで使用）

//#define MK888_ADD1 	(0x7F7F7E)
#define MK888_ADD2 	(0x808080)
#define MK888_ADD3 	(0x10101)	//最下位ビットマスク
*/

#define MK888_ADD4 	(0x7F7F7F)		//桁上がり部分を０にする
#define MK888_ADD5 	(0x808080)		//桁上がり検出マスク（ADDで使用）
#define MK888_ADD6 	(0x010101)		//最下位ビットマスク

#define MK888_ADD1 	(0x0FEFEFF)		// 桁上がり部分を０にする
#define MK888_ADD2 	(0x1010100)		// 桁上がりビット検出マスク
#define MK888_ADD3 	(0x0010100)		// 最下位ビットマスク

#define MK888_SUB1 	(0x0FEFEFF)		// 桁上がり部分を０にする
#define MK888_SUB2 	(0x1010100)		// 桁上がりビット検出マスク
#define MK888_SUB3	(0x0010100)		// 最下位ビットマスク







// 16bit

  #define RMSK16(DATA)   ((DWORD)(DATA) & MEMIMG_R5G6B5_R)
  #define GMSK16(DATA)   ((DWORD)(DATA) & MEMIMG_R5G6B5_G)
  #define BMSK16(DATA)   ((DWORD)(DATA) & MEMIMG_R5G6B5_B)

  #define RBOT16(DATA)	 (  (DATA)                    >> MEMIMG_R5G6B5_LR )
  #define GBOT16(DATA)	 ( ((DATA) & MEMIMG_R5G6B5_G) >> MEMIMG_R5G6B5_LG )
  #define BBOT16(DATA)	 (  (DATA) & MEMIMG_R5G6B5_B                      )

  #define RUP16(DATA)	 ( (DATA) << MEMIMG_R5G6B5_LR )
  #define GUP16(DATA)	 ( (DATA) << MEMIMG_R5G6B5_LG )
  #define BUP16(DATA)	 ( (DATA)                     )

  #define BIMG(DATA)	  MemImgManage.BlendGraphTable[(DATA)]

  #define RM16			MEMIMG_R5G6B5_R
  #define GM16			MEMIMG_R5G6B5_G
  #define BM16			MEMIMG_R5G6B5_B

  #define RL16			MEMIMG_R5G6B5_LR
  #define GL16			MEMIMG_R5G6B5_LG
  #define BL16			MEMIMG_R5G6B5_LB

  #define RTABLE(DATA)	RedTable[(DATA)]
  #define GTABLE(DATA)	GreenTable[(DATA)]
  #define BTABLE(DATA)	BlueTable[(DATA)]

  #define MT1(DATA)		RateTable1[(DATA)]
  #define MT2(DATA)		RateTable2[(DATA)]

  #define BPARAM		MemImgManage.BlendParam

  // BI:ブレンドイメージ   BR:輝度  AC:αチャンネル 
  // BNO:ブレンド無し  BAL:αブレンド  BAD:加算ブレンド  BSB:減算ブレンド  BML:乗算ブレンド  BMA:乗算＋αブレンド  BIS:描画元の色を反転＋αブレンド
  // ND:標準描画  FD:高速描画
  // TCK:透過色チェック  ACK:αチェック
  // NTBL:ブレンド画像以外の乗暫テーブルを使用しない

  // ブレンド無し

	// ブレンド画像あり

	  // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BNO( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			Src2 = ( DWORD )( (DEST) << 16 ) ;\
			*(DESTP) = (WORD)( RMSK16( ( ( RMSK16(SRC) - RMSK16(DEST) ) * BIMG(BLEND) * SRCA + ( Src2 & ( RM16 << 16 ) ) ) >> 16 ) |\
							   GMSK16( ( ( GMSK16(SRC) - GMSK16(DEST) ) * BIMG(BLEND) * SRCA + ( Src2 & ( GM16 << 16 ) ) ) >> 16 ) |\
							         ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * BIMG(BLEND) * SRCA + ( Src2 & ( BM16 << 16 ) ) ) >> 16 ) ) ;\
		}
		
		// αチャンネルあり(チェックコード付き)
		#define CODE_UBI_C16_NBR_UAC_BNO_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_NBR_UAC_BNO( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP, BLEND )	\
		{\
			Src2 = ( DWORD )( (DEST) << 8 ) ;\
			*(DESTP) = (WORD)( RMSK16( ( ( RMSK16(SRC) - RMSK16(DEST) ) * BIMG(BLEND) + ( Src2 & ( RM16 << 8 ) ) ) >> 8 ) |\
							   GMSK16( ( ( GMSK16(SRC) - GMSK16(DEST) ) * BIMG(BLEND) + ( Src2 & ( GM16 << 8 ) ) ) >> 8 ) |\
							         ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * BIMG(BLEND) + ( Src2 & ( BM16 << 8 ) ) ) >> 8 ) ) ;\
		}
		
		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BNO_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP, BLEND )
		

	  // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BNO( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA + (RMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LR ) ) ) >> ( 16 - MEMIMG_R5G6B5_LR ) ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA + (GMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LG ) ) ) >> ( 16 - MEMIMG_R5G6B5_LG ) ) |\
							         ( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA + (BMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LB ) ) ) >> ( 16 - MEMIMG_R5G6B5_LB ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BNO_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA + (RMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LR ) ) ) >> ( 16 - MEMIMG_R5G6B5_LR ) ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA + (GMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LG ) ) ) >> ( 16 - MEMIMG_R5G6B5_LG ) ) |\
							         ( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA + (BMSK16(DEST) << ( 16 - MEMIMG_R5G6B5_LB ) ) ) >> ( 16 - MEMIMG_R5G6B5_LB ) ) ) ;\
		}
		
		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BNO_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_UBR_UAC_BNO( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BNO_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_UBI_C16_UBR_UAC_BNO_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * BIMG(BLEND) + (RMSK16(DEST) >> 3) ) << 3 ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * BIMG(BLEND) + (GMSK16(DEST) << 3) ) >> 3 ) |\
							         ( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * BIMG(BLEND) + (BMSK16(DEST) << 8) ) >> 8 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) + (RMSK16(DEST) >> 3) ) << 3 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) + (GMSK16(DEST) << 3) ) >> 3 ) |\
							         ( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) + (BMSK16(DEST) << 8) ) >> 8 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BNO_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BNO_TCK_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

	// ブレンド画像無し

	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BNO( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RMSK16(SRC) - RMSK16(DEST) ) * SRCA + ( RMSK16(DEST) << 8 ) ) >> 8 ) |\
							   GMSK16( ( ( GMSK16(SRC) - GMSK16(DEST) ) * SRCA + ( GMSK16(DEST) << 8 ) ) >> 8 ) |\
							         ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * SRCA + ( BMSK16(DEST) << 8 ) ) >> 8 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BNO_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_NBR_UAC_BNO( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)(SRC) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BNO_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BNO( SRC, DEST, DESTP )

	
	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BNO( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * SRCA + ( RMSK16(DEST) >> 3 ) ) << 3 ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * SRCA + ( GMSK16(DEST) << 3 ) ) >> 3 ) |\
							         ( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * SRCA + ( BMSK16(DEST) << 8 ) ) >> 8 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BNO_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * SRCA + ( RMSK16(DEST) >> 3 ) ) << 3 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * SRCA + ( GMSK16(DEST) << 3 ) ) >> 3 ) |\
							         ( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * SRCA + ( BMSK16(DEST) << 8 ) ) >> 8 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BNO_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_UBR_UAC_BNO( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BNO_ACK_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_NBI_C16_UBR_UAC_BNO_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP )	\
		{\
		    *(DESTP) = (WORD)( RUP16( RTABLE( RBOT16(SRC) ) ) |\
							   GUP16( GTABLE( GBOT16(SRC) ) ) |\
							   BUP16( BTABLE( BBOT16(SRC) ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RMSK16(SRC) * RBRIGHT ) >> 8 ) |\
							   GMSK16( ( GMSK16(SRC) * GBRIGHT ) >> 8 ) |\
							   BMSK16( ( BMSK16(SRC) * BBRIGHT ) >> 8 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BNO_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BNO( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BNO_TCK_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BNO_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )

  // αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BAL( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RBOT16(SRC) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( RMSK16(DEST) << 13 ) ) >> 13 ) ) |\
							   GMSK16( ( ( ( GBOT16(SRC) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( GMSK16(DEST) << 19 ) ) >> 19 ) ) |\
							         ( ( ( ( BBOT16(SRC) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RBOT16(SRC) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( RMSK16(DEST) << 13 ) ) >> 13 ) ) |\
							   GMSK16( ( ( ( GBOT16(SRC) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( GMSK16(DEST) << 19 ) ) >> 19 ) ) |\
							         ( ( ( ( BBOT16(SRC) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BAL_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_NBR_UAC_BAL( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BAL_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )\
				else				CODE_UBI_C16_NBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16(SRC) - RMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16(SRC) - GMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16(SRC) - RMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16(SRC) - GMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BAL_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BAL_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BAL( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BAL_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_UBR_UAC_BAL( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BAL_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_UBI_C16_UBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( RMSK16(DEST) << 5 ) ) >> 5 ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( RMSK16(DEST) << 5 ) ) >> 5 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BAL_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BAL_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BAL( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16(SRC) - RMSK16(DEST) ) * SRCA * BPARAM + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16(SRC) - GMSK16(DEST) ) * SRCA * BPARAM + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * SRCA * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16(SRC) - RMSK16(DEST) ) * SRCA * ABRIGHT + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16(SRC) - GMSK16(DEST) ) * SRCA * ABRIGHT + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16(SRC) - BMSK16(DEST) ) * SRCA * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BAL_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_NBR_UAC_BAL( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BAL_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT )\
				else				CODE_NBI_C16_NBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)RBOT16(SRC) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)GBOT16(SRC) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)BBOT16(SRC) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RUP16( ( ( ( (int)RBOT16(SRC) - (int)RBOT16(DEST) ) * ABRIGHT ) >> 8 ) + RBOT16(DEST) ) |\
							   GUP16( ( ( ( (int)GBOT16(SRC) - (int)GBOT16(DEST) ) * ABRIGHT ) >> 8 ) + GBOT16(DEST) ) |\
							   BUP16( ( ( ( (int)BBOT16(SRC) - (int)BBOT16(DEST) ) * ABRIGHT ) >> 8 ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BAL_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BAL( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BAL_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BAL( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16(SRC) ) - RBOT16(DEST) ) * BPARAM * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( GTABLE( GBOT16(SRC) ) - GBOT16(DEST) ) * BPARAM * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( BTABLE( BBOT16(SRC) ) - BBOT16(DEST) ) * BPARAM * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * ABRIGHT * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * ABRIGHT * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * ABRIGHT * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BAL_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_UBR_UAC_BAL( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BAL_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_NBI_C16_UBR_UAC_BAL_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)RTABLE( RBOT16(SRC) ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)GTABLE( GBOT16(SRC) ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)BTABLE( BBOT16(SRC) ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RUP16( ( ( ( (int)( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( ( ( ( (int)( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( ( ( ( (int)( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BAL_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BAL( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BAL_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BAL_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

  // 加算ブレンド

	// ブレンド画像あり

      // 輝度なし

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BAD( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( (DWORD)( RMSK16(SRC) * SRCA * BIMG(BLEND) ) >> 27 ) ) +\
						   GUP16( MT1( (DWORD)( GMSK16(SRC) * SRCA * BIMG(BLEND) ) >> 21 ) ) +\
						   BUP16( MT1( (DWORD)( BMSK16(SRC) * SRCA * BIMG(BLEND) ) >> 16 ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( (DWORD)( RBOT16(SRC) * SRCA * BIMG(BLEND) * ABRIGHT ) >> 24 ) +\
						   GUP16( (DWORD)( GBOT16(SRC) * SRCA * BIMG(BLEND) * ABRIGHT ) >> 24 ) +\
						   BUP16( (DWORD)( BBOT16(SRC) * SRCA * BIMG(BLEND) * ABRIGHT ) >> 24 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BAD_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_NBR_UAC_BAD( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BAD_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )\
				else				CODE_UBI_C16_NBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( RMSK16(SRC) * BIMG(BLEND) ) >> 19 ) ) +\
						   GUP16( MT1( ( GMSK16(SRC) * BIMG(BLEND) ) >> 13 ) ) +\
						   BUP16( MT1( ( BMSK16(SRC) * BIMG(BLEND) ) >> 8  ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> 27 ) +\
						   GUP16( ( GMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> 21 ) +\
						   BUP16( ( BMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> 16 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BAD_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BAD_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BAD( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( RTABLE( RBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) +\
						   GUP16( MT1( ( GTABLE( GBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) +\
						   BUP16( MT1( ( BTABLE( BBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   GUP16( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   BUP16( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BAD_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_UBR_UAC_BAD( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BAD_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_UBI_C16_UBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( BIMG(BLEND) * RTABLE( RBOT16(SRC) ) ) >> 8 ) ) +\
						   GUP16( MT1( ( BIMG(BLEND) * GTABLE( GBOT16(SRC) ) ) >> 8 ) ) +\
						   BUP16( MT1( ( BIMG(BLEND) * BTABLE( BBOT16(SRC) ) ) >> 8 ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RBOT16(SRC) * RBRIGHT * ABRIGHT * BIMG(BLEND) ) >> 24 ) +\
						   GUP16( ( GBOT16(SRC) * GBRIGHT * ABRIGHT * BIMG(BLEND) ) >> 24 ) +\
						   BUP16( ( BBOT16(SRC) * BBRIGHT * ABRIGHT * BIMG(BLEND) ) >> 24 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BAD_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BAD_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

	// ブレンド画像無し
	
	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BAD( SRC, SRCA, DEST, DESTP )	\
		{\
			Src1 = (WORD)(  RUP16( MT1( ( RMSK16(SRC) * SRCA ) >> 19 ) ) +\
							GUP16( MT1( ( GMSK16(SRC) * SRCA ) >> 13 ) ) +\
							BUP16( MT1( ( BMSK16(SRC) * SRCA ) >> 8  ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
		{\
			Src1 = (WORD)(  RUP16( ( RMSK16(SRC) * SRCA * ABRIGHT ) >> 27 ) +\
							GUP16( ( GMSK16(SRC) * SRCA * ABRIGHT ) >> 21 ) +\
							BUP16( ( BMSK16(SRC) * SRCA * ABRIGHT ) >> 16 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BAD_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_NBR_UAC_BAD( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BAD_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT )\
				else				CODE_NBI_C16_NBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP )	\
		{\
			Src1 = (WORD)(  RUP16( MT1( RBOT16(SRC) ) ) +\
							GUP16( MT1( GBOT16(SRC) ) ) +\
							BUP16( MT1( BBOT16(SRC) ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
		{\
			Src1 = (WORD)(  RUP16( ( RMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LR ) ) +\
							GUP16( ( GMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LG ) ) +\
							BUP16( ( BMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BAD_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BAD( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BAD_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BAD( SRC, SRCA, DEST, DESTP )	\
		{\
			Src1 = (WORD)( RUP16( MT1( RTABLE( ( RMSK16(SRC) * SRCA ) >> 19 ) ) ) +\
						   GUP16( MT1( GTABLE( ( GMSK16(SRC) * SRCA ) >> 13 ) ) ) +\
						   BUP16( MT1( BTABLE( ( BMSK16(SRC) * SRCA ) >> 8  ) ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RBOT16(SRC) * SRCA * ABRIGHT * RBRIGHT ) >> 24 ) +\
						   GUP16( ( GBOT16(SRC) * SRCA * ABRIGHT * GBRIGHT ) >> 24 ) +\
						   BUP16( ( BBOT16(SRC) * SRCA * ABRIGHT * BBRIGHT ) >> 24 ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BAD_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_UBR_UAC_BAD( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BAD_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_NBI_C16_UBR_UAC_BAD_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP )	\
		{\
			Src1 = (WORD)( RUP16( MT1( RTABLE( RBOT16(SRC) ) ) ) +\
						   GUP16( MT1( GTABLE( GBOT16(SRC) ) ) ) +\
						   BUP16( MT1( BTABLE( BBOT16(SRC) ) ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RMSK16(SRC) * ABRIGHT * RBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LR ) ) +\
						   GUP16( ( GMSK16(SRC) * ABRIGHT * GBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LG ) ) +\
						   BUP16( ( BMSK16(SRC) * ABRIGHT * BBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( Src1 & MEMIMG_R5G6B5_ADDMASK1 ) + ( (DEST) & MEMIMG_R5G6B5_ADDMASK1 ) ;\
			DataD = Data & MEMIMG_R5G6B5_ADDMASK2 ;\
			*(DESTP) = (WORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_R5G6B5_ADDSHFT ) ) ) ) | ( ( Src1 | (DEST) ) & MEMIMG_R5G6B5_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BAD_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BAD( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BAD_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT  )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BAD_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

  // 減算ブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BSB( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( (DWORD)( RMSK16(SRC) * BIMG(BLEND) * SRCA ) >> 27 ) ) +\
						   GUP16( MT1( (DWORD)( GMSK16(SRC) * BIMG(BLEND) * SRCA ) >> 21 ) ) +\
						   BUP16( MT1( (DWORD)( BMSK16(SRC) * BIMG(BLEND) * SRCA ) >> 16 ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( (DWORD)( RBOT16(SRC) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   GUP16( (DWORD)( GBOT16(SRC) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   BUP16( (DWORD)( BBOT16(SRC) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BSB_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_NBR_UAC_BSB( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BSB_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )\
				else				CODE_UBI_C16_NBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( RMSK16(SRC) * BIMG(BLEND) ) >> ( 8 + MEMIMG_R5G6B5_LR ) ) ) +\
						   GUP16( MT1( ( GMSK16(SRC) * BIMG(BLEND) ) >> ( 8 + MEMIMG_R5G6B5_LG ) ) ) +\
						   BUP16( MT1( ( BMSK16(SRC) * BIMG(BLEND) ) >> ( 8 + MEMIMG_R5G6B5_LB ) ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LR ) ) +\
						   GUP16( ( GMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LG ) ) +\
						   BUP16( ( BMSK16(SRC) * BIMG(BLEND) * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BSB_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BSB_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BSB( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( RTABLE( RBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) +\
						   GUP16( MT1( ( GTABLE( GBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) +\
						   BUP16( MT1( ( BTABLE( BBOT16(SRC) ) * BIMG(BLEND) * SRCA ) >> 16 ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   GUP16( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) +\
						   BUP16( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) * BIMG(BLEND) * SRCA * ABRIGHT ) >> 24 ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BSB_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_UBR_UAC_BSB( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BSB_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_UBI_C16_UBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP, BLEND )	\
		{\
			Src1 = (WORD)( RUP16( MT1( ( RTABLE( RBOT16(SRC) ) * BIMG(BLEND) ) >> 8 ) ) +\
						   GUP16( MT1( ( GTABLE( GBOT16(SRC) ) * BIMG(BLEND) ) >> 8 ) ) +\
						   BUP16( MT1( ( BTABLE( BBOT16(SRC) ) * BIMG(BLEND) ) >> 8 ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RBOT16(SRC) * BIMG(BLEND) * ABRIGHT * RBRIGHT ) >> 24 ) +\
						   GUP16( ( GBOT16(SRC) * BIMG(BLEND) * ABRIGHT * GBRIGHT ) >> 24 ) +\
						   BUP16( ( BBOT16(SRC) * BIMG(BLEND) * ABRIGHT * BBRIGHT ) >> 24 ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BSB_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BSB_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BSB( SRC, SRCA, DEST, DESTP )	\
		{\
		    Src1 = (WORD)(  RUP16( MT1( ( RMSK16(SRC) * SRCA ) >> ( 8 + MEMIMG_R5G6B5_LR ) ) ) +\
							GUP16( MT1( ( GMSK16(SRC) * SRCA ) >> ( 8 + MEMIMG_R5G6B5_LG ) ) ) +\
							BUP16( MT1( ( BMSK16(SRC) * SRCA ) >> ( 8 + MEMIMG_R5G6B5_LB ) ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
		{\
		    Src1 = (WORD)(  RUP16( ( RMSK16(SRC) * SRCA * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LR ) ) +\
							GUP16( ( GMSK16(SRC) * SRCA * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LG ) ) +\
							BUP16( ( BMSK16(SRC) * SRCA * ABRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BSB_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_NBR_UAC_BSB( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BSB_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT )\
				else				CODE_NBI_C16_NBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP )	\
		{\
		    Src1 = (WORD)(  RUP16( MT1( RBOT16(SRC) ) ) +\
							GUP16( MT1( GBOT16(SRC) ) ) +\
							BUP16( MT1( BBOT16(SRC) ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
		{\
		    Src1 = (WORD)(  RUP16( ( RMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LR ) ) +\
							GUP16( ( GMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LG ) ) +\
							BUP16( ( BMSK16(SRC) * ABRIGHT ) >> ( 8 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BSB_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BSB( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BSB_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BSB( SRC, SRCA, DEST, DESTP )	\
		{\
			Src1 = (WORD)( RUP16( MT1( RTABLE( ( RMSK16(SRC) * SRCA ) >> 19 ) ) ) +\
						   GUP16( MT1( GTABLE( ( GMSK16(SRC) * SRCA ) >> 13 ) ) ) +\
						   BUP16( MT1( BTABLE( ( BMSK16(SRC) * SRCA ) >> 8  ) ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RBOT16(SRC) * SRCA * ABRIGHT * RBRIGHT ) >> 24 ) +\
						   GUP16( ( GBOT16(SRC) * SRCA * ABRIGHT * GBRIGHT ) >> 24 ) +\
						   BUP16( ( BBOT16(SRC) * SRCA * ABRIGHT * BBRIGHT ) >> 24 ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BSB_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_UBR_UAC_BSB( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BSB_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_NBI_C16_UBR_UAC_BSB_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP )	\
		{\
			Src1 = (WORD)( RUP16( MT1( RTABLE( RBOT16(SRC) ) ) ) +\
						   GUP16( MT1( GTABLE( GBOT16(SRC) ) ) ) +\
						   BUP16( MT1( BTABLE( BBOT16(SRC) ) ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			Src1 = (WORD)( RUP16( ( RMSK16(SRC) * ABRIGHT * RBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LR ) ) +\
						   GUP16( ( GMSK16(SRC) * ABRIGHT * GBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LG ) ) +\
						   BUP16( ( BMSK16(SRC) * ABRIGHT * BBRIGHT ) >> ( 16 + MEMIMG_R5G6B5_LB ) ) ) ;\
			Data = ( (DEST) | MEMIMG_R5G6B5_SUBMASK2 ) - ( Src1 & ~MEMIMG_R5G6B5_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_R5G6B5_SUBMASK2 ;\
			*(DESTP) = (WORD)( Data & ( DataD - ( DataD >> MEMIMG_R5G6B5_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BSB_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BSB( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BSB_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BSB_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )


  // 乗算ブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BML( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * BIMG(BLEND) * SRCA * RBOT16(DEST) ) >> 10  ) |\
							   GMSK16( ( GBOT16(SRC) * BIMG(BLEND) * SRCA * GBOT16(DEST) ) >> 17  ) |\
							         ( ( BBOT16(SRC) * BIMG(BLEND) * SRCA * BBOT16(DEST) ) >> 21 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BML_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BML( SRC, DEST, DESTP, BLEND )\
			else				CODE_UBI_C16_NBR_UAC_BML( SRC, SRCA, DEST, DESTP, BLEND )

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BML( SRC, DEST, DESTP, BLEND )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * BIMG(BLEND) * RBOT16(DEST) ) >> 2  ) |\
							   GMSK16( ( GBOT16(SRC) * BIMG(BLEND) * GBOT16(DEST) ) >> 9  ) |\
							         ( ( BBOT16(SRC) * BIMG(BLEND) * BBOT16(DEST) ) >> 13 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BML_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BML( SRC, DEST, DESTP, BLEND )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BML( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RTABLE( RBOT16(SRC) ) * SRCA * BIMG(BLEND) * RBOT16(DEST) ) >> 10  ) |\
							   GMSK16( ( GTABLE( GBOT16(SRC) ) * SRCA * BIMG(BLEND) * GBOT16(DEST) ) >> 17  ) |\
							         ( ( BTABLE( BBOT16(SRC) ) * SRCA * BIMG(BLEND) * BBOT16(DEST) ) >> 21 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BML_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( ( ( RBOT16(SRC) * RBRIGHT ) >> 8 ) * SRCA * BIMG(BLEND) * RBOT16(DEST) ) >> 10  ) |\
							   GMSK16( ( ( ( GBOT16(SRC) * GBRIGHT ) >> 8 ) * SRCA * BIMG(BLEND) * GBOT16(DEST) ) >> 17  ) |\
							         ( ( ( ( BBOT16(SRC) * BBRIGHT ) >> 8 ) * SRCA * BIMG(BLEND) * BBOT16(DEST) ) >> 21 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BML_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BML( SRC, DEST, DESTP, BLEND )\
			else				CODE_UBI_C16_UBR_UAC_BML( SRC, SRCA, DEST, DESTP, BLEND )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BML_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
			else				CODE_UBI_C16_UBR_UAC_BML_NTBL( SRC, SRCA, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BML( SRC, DEST, DESTP, BLEND )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RTABLE( RBOT16(SRC) ) * BIMG(BLEND) * RBOT16(DEST) ) >> 2  ) |\
							   GMSK16( ( GTABLE( GBOT16(SRC) ) * BIMG(BLEND) * GBOT16(DEST) ) >> 9  ) |\
							         ( ( BTABLE( BBOT16(SRC) ) * BIMG(BLEND) * BBOT16(DEST) ) >> 13 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * RBRIGHT * BIMG(BLEND) * RBOT16(DEST) ) >> 10  ) |\
							   GMSK16( ( GBOT16(SRC) * GBRIGHT * BIMG(BLEND) * GBOT16(DEST) ) >> 17  ) |\
							         ( ( BBOT16(SRC) * BBRIGHT * BIMG(BLEND) * BBOT16(DEST) ) >> 21 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BML_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BML( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BML_TCK_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BML( SRC, SRCA, DEST, DESTP )	\
		{\
		    *(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * SRCA * RBOT16(DEST) ) >> 2  ) |\
							   GMSK16( ( GBOT16(SRC) * SRCA * GBOT16(DEST) ) >> 9  ) |\
							         ( ( BBOT16(SRC) * SRCA * BBOT16(DEST) ) >> 13 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BML_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BML( SRC, DEST, DESTP )\
			else				CODE_NBI_C16_NBR_UAC_BML( SRC, SRCA, DEST, DESTP )

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BML( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( RMSK16(SRC) * RMSK16(DEST) ) >> 16 ) |\
							   GMSK16( ( GMSK16(SRC) * GMSK16(DEST) ) >> 11 ) |\
							         ( ( BMSK16(SRC) * BMSK16(DEST) ) >> 5  ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BML_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BML( SRC, DEST, DESTP )

      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BML( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( RTABLE( RBOT16(SRC) ) * SRCA * RBOT16(DEST) ) >> 2 ) |\
							   GMSK16( ( GTABLE( GBOT16(SRC) ) * SRCA * GBOT16(DEST) ) >> 9 ) |\
							         ( ( BTABLE( BBOT16(SRC) ) * SRCA * BBOT16(DEST) ) >> 13 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BML_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * RBRIGHT * SRCA * RBOT16(DEST) ) >> 10 ) |\
							   GMSK16( ( GBOT16(SRC) * GBRIGHT * SRCA * GBOT16(DEST) ) >> 17 ) |\
							         ( ( BBOT16(SRC) * BBRIGHT * SRCA * BBOT16(DEST) ) >> 21 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BML_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BML( SRC, DEST, DESTP )\
			else				CODE_NBI_C16_UBR_UAC_BML( SRC, SRCA, DEST, DESTP )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BML_ACK_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
			else				CODE_NBI_C16_UBR_UAC_BML_NTBL( SRC, SRCA, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BML( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( RTABLE( RBOT16(SRC) ) * RBOT16(DEST) ) << 6 ) |\
							   GMSK16( ( GTABLE( GBOT16(SRC) ) * GBOT16(DEST) ) >> 1 ) |\
							         ( ( BTABLE( BBOT16(SRC) ) * BBOT16(DEST) ) >> 5 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( RBOT16(SRC) * RBRIGHT * RBOT16(DEST) ) >> 2 ) |\
							   GMSK16( ( GBOT16(SRC) * GBRIGHT * GBOT16(DEST) ) >> 9 ) |\
							         ( ( BBOT16(SRC) * BBRIGHT * BBOT16(DEST) ) >> 13 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BML_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BML( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BML_TCK_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BML_NTBL( SRC, DEST, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )

  // 乗算＋αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BMA( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( ( RBOT16(SRC) * RBOT16(DEST) ) >> 5 ) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( RBOT16(DEST) << 24 ) ) >> 13 ) ) |\
							   GMSK16( ( ( ( ( ( GBOT16(SRC) * GBOT16(DEST) ) >> 6 ) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( GBOT16(DEST) << 24 ) ) >> 19 ) ) |\
							         ( ( ( ( ( ( BBOT16(SRC) * BBOT16(DEST) ) >> 5 ) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( BBOT16(DEST) << 24 ) ) >> 24 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BMA_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP, BLEND )\
			else				CODE_UBI_C16_NBR_UAC_BMA( SRC, SRCA, DEST, DESTP, BLEND )

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( ( RBOT16(SRC) * RBOT16(DEST) ) >> 5 ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( RBOT16(DEST) << 16 ) ) >> 5 ) ) |\
							   GMSK16( ( ( ( ( ( GBOT16(SRC) * GBOT16(DEST) ) >> 6 ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( GBOT16(DEST) << 16 ) ) >> 11 ) ) |\
							         ( ( ( ( ( ( BBOT16(SRC) * BBOT16(DEST) ) >> 5 ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( BBOT16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BMA_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP, BLEND )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BMA( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RTABLE( RBOT16(SRC) ) * RBOT16(DEST) ) >> 5 ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( ( ( GTABLE( GBOT16(SRC) ) * GBOT16(DEST) ) >> 6 ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( ( ( BTABLE( BBOT16(SRC) ) * BBOT16(DEST) ) >> 5 ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BMA_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT * RBOT16(DEST) ) >> 13 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT * GBOT16(DEST) ) >> 14 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT * BBOT16(DEST) ) >> 13 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BMA_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP, BLEND )\
			else				CODE_UBI_C16_UBR_UAC_BMA( SRC, SRCA, DEST, DESTP, BLEND )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BMA_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BMA_NTBL( SRC,       DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else				CODE_UBI_C16_UBR_UAC_BMA_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RTABLE( RBOT16(SRC) ) * RBOT16(DEST) ) >> 5 ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( ( ( GTABLE( GBOT16(SRC) ) * GBOT16(DEST) ) >> 6 ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BTABLE( BBOT16(SRC) ) * BBOT16(DEST) ) >> 5 ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BMA_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT * RBOT16(DEST) ) >> 13 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( RMSK16(DEST) << 5 ) ) >> 5 ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT * GBOT16(DEST) ) >> 14 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT * BBOT16(DEST) ) >> 13 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BMA_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BMA_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BMA_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BMA( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( ( RMSK16(SRC) * RMSK16(DEST) ) >> 27 ) - RBOT16(DEST) ) * SRCA * BPARAM + ( RBOT16(DEST) << 16 ) ) >> 5  ) ) |\
							   GMSK16( ( ( ( ( ( GMSK16(SRC) * GMSK16(DEST) ) >> 11 ) - GMSK16(DEST) ) * SRCA * BPARAM + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( ( ( BMSK16(SRC) * BMSK16(DEST) ) >> 5  ) - BMSK16(DEST) ) * SRCA * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BMA_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP )\
			else				CODE_NBI_C16_NBR_UAC_BMA( SRC, SRCA, DEST, DESTP )

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)( ( RBOT16(SRC) * RBOT16(DEST) ) >> 5 ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)( ( GBOT16(SRC) * GBOT16(DEST) ) >> 6 ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)( ( BBOT16(SRC) * BBOT16(DEST) ) >> 5 ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BMA_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BMA( SRC, DEST, DESTP )

      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BMA( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RTABLE( RBOT16(SRC) ) * RBOT16(DEST) ) >> 5 ) - RBOT16(DEST) ) * BPARAM * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( ( ( GTABLE( GBOT16(SRC) ) * GBOT16(DEST) ) >> 6 ) - GBOT16(DEST) ) * BPARAM * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BTABLE( BBOT16(SRC) ) * BBOT16(DEST) ) >> 5 ) - BBOT16(DEST) ) * BPARAM * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BMA_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16(SRC) * RBRIGHT * RBOT16(DEST) ) >> 13 ) - RBOT16(DEST) ) * ABRIGHT * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( ( ( GBOT16(SRC) * GBRIGHT * GBOT16(DEST) ) >> 14 ) - GBOT16(DEST) ) * ABRIGHT * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16(SRC) * BBRIGHT * BBOT16(DEST) ) >> 13 ) - BBOT16(DEST) ) * ABRIGHT * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BMA_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP )\
			else				CODE_NBI_C16_UBR_UAC_BMA( SRC, SRCA, DEST, DESTP )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BMA_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BMA_NTBL( SRC,       DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else				CODE_NBI_C16_UBR_UAC_BMA_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)( ( RTABLE( RBOT16(SRC) ) * RBOT16(DEST) ) >> 5 ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)( ( GTABLE( GBOT16(SRC) ) * GBOT16(DEST) ) >> 6 ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)( ( BTABLE( BBOT16(SRC) ) * BBOT16(DEST) ) >> 5 ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BMA_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RUP16( ( ( ( (int)( ( RBOT16(SRC) * RBRIGHT * RBOT16(DEST) ) >> 13 ) * ABRIGHT ) >> 8 ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( ( ( ( (int)( ( GBOT16(SRC) * GBRIGHT * GBOT16(DEST) ) >> 14 ) * ABRIGHT ) >> 8 ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( ( ( ( (int)( ( BBOT16(SRC) * BBRIGHT * BBOT16(DEST) ) >> 13 ) * ABRIGHT ) >> 8 ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BMA_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BMA( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BMA_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BMA_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )


  // 描画元の色を反転＋αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C16_NBR_UAC_BIS( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RBOT16((WORD)~(SRC)) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( RMSK16(DEST) << 13 ) ) >> 13 ) ) |\
							   GMSK16( ( ( ( GBOT16((WORD)~(SRC)) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( GMSK16(DEST) << 19 ) ) >> 19 ) ) |\
							         ( ( ( ( BBOT16((WORD)~(SRC)) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA * BPARAM + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RBOT16((WORD)~(SRC)) - RBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( RMSK16(DEST) << 13 ) ) >> 13 ) ) |\
							   GMSK16( ( ( ( GBOT16((WORD)~(SRC)) - GBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( GMSK16(DEST) << 19 ) ) >> 19 ) ) |\
							         ( ( ( ( BBOT16((WORD)~(SRC)) - BBOT16(DEST) ) * BIMG(BLEND) * SRCA * ABRIGHT + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_NBR_UAC_BIS_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_NBR_UAC_BIS( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_UAC_BIS_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )\
				else				CODE_UBI_C16_NBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16((WORD)~(SRC)) - RMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16((WORD)~(SRC)) - GMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16((WORD)~(SRC)) - BMSK16(DEST) ) * BIMG(BLEND) * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16((WORD)~(SRC)) - RMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16((WORD)~(SRC)) - GMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16((WORD)~(SRC)) - BMSK16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_NBR_NAC_BIS_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_NBR_NAC_BIS_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C16_UBR_UAC_BIS( SRC, SRCA, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16((WORD)~(SRC)) ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( GTABLE( GBOT16((WORD)~(SRC)) ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( BTABLE( BBOT16((WORD)~(SRC)) ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16((WORD)~(SRC)) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( RMSK16(DEST) << 13 ) ) >> 13 ) |\
							   GMSK16( ( ( ( ( GBOT16((WORD)~(SRC)) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( GMSK16(DEST) << 19 ) ) >> 19 ) |\
							   BMSK16( ( ( ( ( BBOT16((WORD)~(SRC)) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT * SRCA + ( BMSK16(DEST) << 24 ) ) >> 24 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C16_UBR_UAC_BIS_ACK( SRC, SRCA, DEST, DESTP, BLEND )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP, BLEND )\
				else				CODE_UBI_C16_UBR_UAC_BIS( SRC, SRCA, DEST, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_UAC_BIS_ACK_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_UBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_UBI_C16_UBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP, BLEND )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16((WORD)~(SRC)) ) - RBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( RMSK16(DEST) << 5 ) ) >> 5 ) |\
							   GMSK16( ( ( GTABLE( GBOT16((WORD)~(SRC)) ) - GBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( BTABLE( BBOT16((WORD)~(SRC)) ) - BBOT16(DEST) ) * BIMG(BLEND) * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16((WORD)~(SRC)) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( RMSK16(DEST) << 5 ) ) >> 5 ) |\
							   GMSK16( ( ( ( ( GBOT16((WORD)~(SRC)) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16((WORD)~(SRC)) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * BIMG(BLEND) * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C16_UBR_NAC_BIS_TCK( SRC, DEST, DESTP, BLEND )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C16_UBR_NAC_BIS_TCK_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_UBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C16_NBR_UAC_BIS( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16((WORD)~(SRC)) - RMSK16(DEST) ) * SRCA * BPARAM + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16((WORD)~(SRC)) - GMSK16(DEST) ) * SRCA * BPARAM + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16((WORD)~(SRC)) - BMSK16(DEST) ) * SRCA * BPARAM + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( RMSK16((WORD)~(SRC)) - RMSK16(DEST) ) * SRCA * ABRIGHT + ( RMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							   GMSK16( ( ( ( GMSK16((WORD)~(SRC)) - GMSK16(DEST) ) * SRCA * ABRIGHT + ( GMSK16(DEST) << 16 ) ) >> 16 ) ) |\
							         ( ( ( ( BMSK16((WORD)~(SRC)) - BMSK16(DEST) ) * SRCA * ABRIGHT + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_NBR_UAC_BIS_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_NBR_UAC_BIS( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_UAC_BIS_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT )\
				else				CODE_NBI_C16_NBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)RBOT16((WORD)~(SRC)) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)GBOT16((WORD)~(SRC)) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)BBOT16((WORD)~(SRC)) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
		{\
			*(DESTP) = (WORD)( RUP16( ( ( ( (int)RBOT16((WORD)~(SRC)) - (int)RBOT16(DEST) ) * ABRIGHT ) >> 8 ) + RBOT16(DEST) ) |\
							   GUP16( ( ( ( (int)GBOT16((WORD)~(SRC)) - (int)GBOT16(DEST) ) * ABRIGHT ) >> 8 ) + GBOT16(DEST) ) |\
							   BUP16( ( ( ( (int)BBOT16((WORD)~(SRC)) - (int)BBOT16(DEST) ) * ABRIGHT ) >> 8 ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_NBR_NAC_BIS_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BIS( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_NBR_NAC_BIS_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_NBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C16_UBR_UAC_BIS( SRC, SRCA, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( RTABLE( RBOT16((WORD)~(SRC)) ) - RBOT16(DEST) ) * BPARAM * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( GTABLE( GBOT16((WORD)~(SRC)) ) - GBOT16(DEST) ) * BPARAM * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( BTABLE( BBOT16((WORD)~(SRC)) ) - BBOT16(DEST) ) * BPARAM * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RMSK16( ( ( ( ( RBOT16((WORD)~(SRC)) * RBRIGHT ) >> 8 ) - RBOT16(DEST) ) * ABRIGHT * SRCA + ( RMSK16(DEST) << 5  ) ) >> 5  ) |\
							   GMSK16( ( ( ( ( GBOT16((WORD)~(SRC)) * GBRIGHT ) >> 8 ) - GBOT16(DEST) ) * ABRIGHT * SRCA + ( GMSK16(DEST) << 11 ) ) >> 11 ) |\
							   BMSK16( ( ( ( ( BBOT16((WORD)~(SRC)) * BBRIGHT ) >> 8 ) - BBOT16(DEST) ) * ABRIGHT * SRCA + ( BMSK16(DEST) << 16 ) ) >> 16 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C16_UBR_UAC_BIS_ACK( SRC, SRCA, DEST, DESTP )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP )\
				else				CODE_NBI_C16_UBR_UAC_BIS( SRC, SRCA, DEST, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_UAC_BIS_ACK_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRCA != 0 )\
			{\
				if( SRCA == 255 ) 	CODE_NBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else				CODE_NBI_C16_UBR_UAC_BIS_NTBL( SRC, SRCA, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP )	\
		{\
			*(DESTP) = (WORD)( RUP16( MT2( (int)RTABLE( RBOT16((WORD)~(SRC)) ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( MT2( (int)GTABLE( GBOT16((WORD)~(SRC)) ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( MT2( (int)BTABLE( BBOT16((WORD)~(SRC)) ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			*(DESTP) = (WORD)( RUP16( ( ( ( (int)( ( RBOT16((WORD)~(SRC)) * RBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)RBOT16(DEST) ) + RBOT16(DEST) ) |\
							   GUP16( ( ( ( (int)( ( GBOT16((WORD)~(SRC)) * GBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)GBOT16(DEST) ) + GBOT16(DEST) ) |\
							   BUP16( ( ( ( (int)( ( BBOT16((WORD)~(SRC)) * BBRIGHT ) >> 8 ) * ABRIGHT ) >> 8 ) - (int)BBOT16(DEST) ) + BBOT16(DEST) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C16_UBR_NAC_BIS_TCK( SRC, DEST, DESTP )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BIS( SRC, DEST, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C16_UBR_NAC_BIS_TCK_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( SRC != TransColor ) CODE_NBI_C16_UBR_NAC_BIS_NTBL( SRC, DEST, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )


// 32bit

  // ブレンド無し

	// ブレンド画像あり

	  // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BNO( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}
		
		// αチャンネルあり(チェックコード付き)
		#define CODE_UBI_C32_NBR_UAC_BNO_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BNO(  SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_NBR_UAC_BNO( SRCP, DESTP, BLEND )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BNO( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BIMG(BLEND) + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BIMG(BLEND) + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BIMG(BLEND) + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}
		
		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BNO_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BNO( SRCP, DESTP, BLEND )
		

	  // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BNO( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BNO_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}
		
		// αチャンネルあり(チェックコード付き)
		#define CODE_UBI_C32_UBR_UAC_BNO_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BNO( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_UBR_UAC_BNO( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェックコード付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BNO_ACK_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_UBI_C32_UBR_UAC_BNO_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BNO( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BIMG(BLEND) + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BIMG(BLEND) + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BIMG(BLEND) + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * BIMG(BLEND) + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * BIMG(BLEND) + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * BIMG(BLEND) + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BNO_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BNO( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BNO_TCK_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

	// ブレンド画像無し

	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BNO( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * SRCP[3] + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * SRCP[3] + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * SRCP[3] + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BNO_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BNO( SRCP, DESTP )\
				else					CODE_NBI_C32_NBR_UAC_BNO( SRCP, DESTP )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BNO( SRCP, DESTP )	\
		{\
			*((DWORD *)(DESTP)) = *((DWORD *)(SRCP)) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BNO_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BNO( SRCP, DESTP )

	
	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BNO( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * SRCP[3] + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * SRCP[3] + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * SRCP[3] + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BNO_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * SRCP[3] + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * SRCP[3] + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * SRCP[3] + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BNO_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BNO( SRCP, DESTP )\
				else					CODE_NBI_C32_UBR_UAC_BNO( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BNO_ACK_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_NBI_C32_UBR_UAC_BNO_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BNO( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( BTABLE( (SRCP)[0] ) ) ;\
			(DESTP)[1] = (BYTE)( GTABLE( (SRCP)[1] ) ) ;\
			(DESTP)[2] = (BYTE)( RTABLE( (SRCP)[2] ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BNO_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BNO( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BNO_TCK_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BNO_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )

  // αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BAL( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BAL_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BAL( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_NBR_UAC_BAL( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BAL_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
				else					CODE_UBI_C32_NBR_UAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BAL( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BAL_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BAL( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BAL_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BAL( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BAL_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BAL( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_UBR_UAC_BAL( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BAL_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_UBI_C32_UBR_UAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BAL( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BAL_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BAL( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BAL_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BAL( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_NBR_UAC_BAL_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * ABRIGHT * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * ABRIGHT * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * ABRIGHT * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BAL_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BAL( SRCP, DESTP )\
				else					CODE_NBI_C32_NBR_UAC_BAL( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_NBR_UAC_BAL_ACK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT )\
				else					CODE_NBI_C32_NBR_UAC_BAL_NTBL( SRCP, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BAL( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (SRCP)[0] - (DESTP)[0] ) * ABRIGHT + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (SRCP)[1] - (DESTP)[1] ) * ABRIGHT + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (SRCP)[2] - (DESTP)[2] ) * ABRIGHT + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BAL_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BAL( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BAL_TCK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT )


      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BAL( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BAL_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BAL_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BAL( SRCP, DESTP )\
				else					CODE_NBI_C32_UBR_UAC_BAL( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BAL_ACK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_NBI_C32_UBR_UAC_BAL_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BAL( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( BTABLE( (SRCP)[0] ) - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( GTABLE( (SRCP)[1] ) - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( RTABLE( (SRCP)[2] ) - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BAL_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BAL( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BAL_TCK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BAL_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

  // 加算ブレンド

	// ブレンド画像あり

      // 輝度なし

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BAD( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BAD_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BAD( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_NBR_UAC_BAD( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BAD_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
				else					CODE_UBI_C32_NBR_UAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BAD( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BAD_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BAD( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BAD_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BAD( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BAD_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BAD( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_UBR_UAC_BAD( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BAD_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_UBI_C32_UBR_UAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BAD( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BAD_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BAD( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BAD_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

	// ブレンド画像無し
	
	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BAD( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_NBR_UAC_BAD_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BAD_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BAD( SRCP, DESTP )\
				else					CODE_NBI_C32_NBR_UAC_BAD( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_NBR_UAC_BAD_ACK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT )\
				else					CODE_NBI_C32_NBR_UAC_BAD_NTBL( SRCP, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BAD( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( MT1( (SRCP)[0] ) ) ;\
			SrcB[1] = (BYTE)( MT1( (SRCP)[1] ) ) ;\
			SrcB[2] = (BYTE)( MT1( (SRCP)[2] ) ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * ABRIGHT ) >> 8 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * ABRIGHT ) >> 8 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * ABRIGHT ) >> 8 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BAD_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BAD( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BAD_TCK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BAD( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BAD_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BAD_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BAD( SRCP, DESTP )\
				else					CODE_NBI_C32_UBR_UAC_BAD( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BAD_ACK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_NBI_C32_UBR_UAC_BAD_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BAD( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( MT1( BTABLE( (SRCP)[0] ) ) ) ;\
			SrcB[1] = (BYTE)( MT1( GTABLE( (SRCP)[1] ) ) ) ;\
			SrcB[2] = (BYTE)( MT1( RTABLE( (SRCP)[2] ) ) ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * ABRIGHT * BBRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * ABRIGHT * GBRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * ABRIGHT * RBRIGHT ) >> 16 ) ;\
			Data  = ( SrcD & MEMIMG_XRGB8_ADDMASK1 ) + ( *((DWORD *)(DESTP)) & MEMIMG_XRGB8_ADDMASK1 ) ;\
			DataD =   Data & MEMIMG_XRGB8_ADDMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( ( ( Data | ( DataD - ( DataD >> MEMIMG_XRGB8_ADDSHFT ) ) ) ) | ( ( SrcD | *((DWORD *)(DESTP)) ) & MEMIMG_XRGB8_ADDMASK3 ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BAD_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BAD( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BAD_TCK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BAD_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

  // 減算ブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BSB( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BSB_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BSB( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_NBR_UAC_BSB( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_UAC_BSB_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
				else					CODE_UBI_C32_NBR_UAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BSB( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * ABRIGHT ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BSB_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BSB( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_NBR_NAC_BSB_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BSB( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (SRCP)[3] * BIMG(BLEND) * BPARAM ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) * (SRCP)[3] * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BSB_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BSB( SRCP, DESTP, BLEND )\
				else					CODE_UBI_C32_UBR_UAC_BSB( SRCP, DESTP, BLEND )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BSB_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_UBI_C32_UBR_UAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BSB( SRCP, DESTP, BLEND )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * BIMG(BLEND) * BPARAM ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * BIMG(BLEND) * ABRIGHT ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BSB_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BSB( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BSB_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

	  // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BSB( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		#define CODE_NBI_C32_NBR_UAC_BSB_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * ABRIGHT ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BSB_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BSB( SRCP, DESTP )\
				else					CODE_NBI_C32_NBR_UAC_BSB( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BSB_ACK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT )\
				else					CODE_NBI_C32_NBR_UAC_BSB_NTBL( SRCP, DESTP, ABRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BSB( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( MT1( (SRCP)[0] ) ) ;\
			SrcB[1] = (BYTE)( MT1( (SRCP)[1] ) ) ;\
			SrcB[2] = (BYTE)( MT1( (SRCP)[2] ) ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * ABRIGHT ) >> 8 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * ABRIGHT ) >> 8 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * ABRIGHT ) >> 8 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BSB_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BSB( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_NBR_NAC_BSB_TCK_NTBL( SRCP, DESTP, ABRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT )

	  // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BSB( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (SRCP)[3] * BPARAM ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BSB_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * (SRCP)[3] * ABRIGHT ) >> 24 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BSB_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BSB( SRCP, DESTP )\
				else					CODE_NBI_C32_UBR_UAC_BSB( SRCP, DESTP )\
			}

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BSB_ACK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] != 0 )\
			{\
				if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
				else					CODE_NBI_C32_UBR_UAC_BSB_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			}

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BSB( SRCP, DESTP )	\
		{\
			SrcB[0] = (BYTE)( MT1( BTABLE( (SRCP)[0] ) ) ) ;\
			SrcB[1] = (BYTE)( MT1( GTABLE( (SRCP)[1] ) ) ) ;\
			SrcB[2] = (BYTE)( MT1( RTABLE( (SRCP)[2] ) ) ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			SrcB[0] = (BYTE)( ( (SRCP)[0] * ABRIGHT * BBRIGHT ) >> 16 ) ;\
			SrcB[1] = (BYTE)( ( (SRCP)[1] * ABRIGHT * GBRIGHT ) >> 16 ) ;\
			SrcB[2] = (BYTE)( ( (SRCP)[2] * ABRIGHT * RBRIGHT ) >> 16 ) ;\
			Data = ( *((DWORD *)(DESTP)) | MEMIMG_XRGB8_SUBMASK2 ) - ( SrcD & ~MEMIMG_XRGB8_SUBMASK2 ) ;\
			DataD = Data & MEMIMG_XRGB8_SUBMASK2 ;\
			*((DWORD *)(DESTP)) = (DWORD)( Data & ( DataD - ( DataD >> MEMIMG_XRGB8_SUBSHFT ) ) ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BSB_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BSB( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BSB_TCK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BSB_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )


  // 乗算ブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BML( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * (SRCP)[3] * (DESTP)[0] ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * (SRCP)[3] * (DESTP)[1] ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * (SRCP)[3] * (DESTP)[2] ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BML_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BML( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_NBR_UAC_BML( SRCP, DESTP, BLEND )

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BML( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BIMG(BLEND) * (DESTP)[0] ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * BIMG(BLEND) * (DESTP)[1] ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * BIMG(BLEND) * (DESTP)[2] ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BML_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BML( SRCP, DESTP, BLEND )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BML( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[0] ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[1] ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[2] ) >> 24 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BML_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( ( (SRCP)[0] * BBRIGHT ) >> 8 ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[0] ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( ( (SRCP)[1] * GBRIGHT ) >> 8 ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[1] ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( ( (SRCP)[2] * RBRIGHT ) >> 8 ) * BIMG(BLEND) * (SRCP)[3] * (DESTP)[2] ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BML_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BML( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_UBR_UAC_BML( SRCP, DESTP, BLEND )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BML_ACK_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_UBI_C32_UBR_UAC_BML_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BML( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * BIMG(BLEND) * (DESTP)[0] ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * BIMG(BLEND) * (DESTP)[1] ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * BIMG(BLEND) * (DESTP)[2] ) >> 16 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * BIMG(BLEND) * (DESTP)[0] ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * BIMG(BLEND) * (DESTP)[1] ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * BIMG(BLEND) * (DESTP)[2] ) >> 24 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BML_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BML( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BML_TCK_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, BLEND, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BML( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * (SRCP)[3] * (DESTP)[0] ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * (SRCP)[3] * (DESTP)[1] ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * (SRCP)[3] * (DESTP)[2] ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BML_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BML( SRCP, DESTP )\
			else					CODE_NBI_C32_NBR_UAC_BML( SRCP, DESTP )

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BML( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * (DESTP)[0] ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * (DESTP)[1] ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * (DESTP)[2] ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BML_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BML( SRCP, DESTP )

      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BML( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (SRCP)[3] * (DESTP)[0] ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (SRCP)[3] * (DESTP)[1] ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (SRCP)[3] * (DESTP)[2] ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BML_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * (SRCP)[3] * (DESTP)[0] ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * (SRCP)[3] * (DESTP)[1] ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * (SRCP)[3] * (DESTP)[2] ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BML_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BML( SRCP, DESTP )\
			else					CODE_NBI_C32_UBR_UAC_BML( SRCP, DESTP )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BML_ACK_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_NBI_C32_UBR_UAC_BML_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BML( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( BTABLE( (SRCP)[0] ) * (DESTP)[0] ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( GTABLE( (SRCP)[1] ) * (DESTP)[1] ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( RTABLE( (SRCP)[2] ) * (DESTP)[2] ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( (SRCP)[0] * BBRIGHT * (DESTP)[0] ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( (SRCP)[1] * GBRIGHT * (DESTP)[1] ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( (SRCP)[2] * RBRIGHT * (DESTP)[2] ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BML_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BML( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BML_TCK_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BML_NTBL( SRCP, DESTP, RBRIGHT, GBRIGHT, BBRIGHT )


  // 乗算＋αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BMA( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BMA_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BMA( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_NBR_UAC_BMA( SRCP, DESTP, BLEND )

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BMA( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BMA_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BMA( SRCP, DESTP, BLEND )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BMA( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( BTABLE( (SRCP)[0] ) * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( GTABLE( (SRCP)[1] ) * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( RTABLE( (SRCP)[2] ) * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BMA_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * BBRIGHT * (DESTP)[0] ) >> 16 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * GBRIGHT * (DESTP)[1] ) >> 16 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * RBRIGHT * (DESTP)[2] ) >> 16 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BMA_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BMA( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_UBR_UAC_BMA( SRCP, DESTP, BLEND )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BMA_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_UBI_C32_UBR_UAC_BMA_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BMA( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( BTABLE( (SRCP)[0] ) * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( GTABLE( (SRCP)[1] ) * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( RTABLE( (SRCP)[2] ) * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * BBRIGHT * (DESTP)[0] ) >> 16 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * GBRIGHT * (DESTP)[1] ) >> 16 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * RBRIGHT * (DESTP)[2] ) >> 16 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BMA_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BMA( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BMA_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BMA( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BMA_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BMA( SRCP, DESTP )\
			else					CODE_NBI_C32_NBR_UAC_BMA( SRCP, DESTP )

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BMA( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BMA_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BMA( SRCP, DESTP )

      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BMA( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( BTABLE( (SRCP)[0] ) * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( GTABLE( (SRCP)[1] ) * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( RTABLE( (SRCP)[2] ) * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BMA_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * BBRIGHT * (DESTP)[0] ) >> 16 ) - (DESTP)[0] ) * ABRIGHT * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * GBRIGHT * (DESTP)[1] ) >> 16 ) - (DESTP)[1] ) * ABRIGHT * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * RBRIGHT * (DESTP)[2] ) >> 16 ) - (DESTP)[2] ) * ABRIGHT * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BMA_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BMA( SRCP, DESTP )\
			else					CODE_NBI_C32_UBR_UAC_BMA( SRCP, DESTP )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BMA_ACK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_NBI_C32_UBR_UAC_BMA_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BMA( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( BTABLE( (SRCP)[0] ) * (DESTP)[0] ) >> 8 ) - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( GTABLE( (SRCP)[1] ) * (DESTP)[1] ) >> 8 ) - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( RTABLE( (SRCP)[2] ) * (DESTP)[2] ) >> 8 ) - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (SRCP)[0] * BBRIGHT * (DESTP)[0] ) >> 16 ) - (DESTP)[0] ) * ABRIGHT + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (SRCP)[1] * GBRIGHT * (DESTP)[1] ) >> 16 ) - (DESTP)[1] ) * ABRIGHT + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (SRCP)[2] * RBRIGHT * (DESTP)[2] ) >> 16 ) - (DESTP)[2] ) * ABRIGHT + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BMA_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BMA( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BMA_TCK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BMA_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )


  // 描画元の色を反転＋αブレンド

    // ブレンド画像あり
 
      // 輝度無し

		// αチャンネルあり
		#define CODE_UBI_C32_NBR_UAC_BIS( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ~(SRCP)[0] ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ~(SRCP)[1] ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ~(SRCP)[2] ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_NBR_UAC_BIS_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_NBR_NAC_BIS( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_NBR_UAC_BIS( SRCP, DESTP, BLEND )

		// αチャンネルなし
		#define CODE_UBI_C32_NBR_NAC_BIS( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ~(SRCP)[0] ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ~(SRCP)[1] ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ~(SRCP)[2] ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_NBR_NAC_BIS_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_NBR_NAC_BIS( SRCP, DESTP, BLEND )

      // 輝度あり

		// αチャンネルあり
		#define CODE_UBI_C32_UBR_UAC_BIS( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( BTABLE( ( BYTE )~(SRCP)[0] ) ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( GTABLE( ( BYTE )~(SRCP)[1] ) ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( RTABLE( ( BYTE )~(SRCP)[2] ) ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BIS_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( ( BYTE )~(SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[0] << 24 ) ) >> 24 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( ( BYTE )~(SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[1] << 24 ) ) >> 24 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( ( BYTE )~(SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) * SRCP[3] + ( (DESTP)[2] << 24 ) ) >> 24 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_UBI_C32_UBR_UAC_BIS_ACK( SRCP, DESTP, BLEND )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BIS( SRCP, DESTP, BLEND )\
			else					CODE_UBI_C32_UBR_UAC_BIS( SRCP, DESTP, BLEND )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_UAC_BIS_ACK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_UBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_UBI_C32_UBR_UAC_BIS_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_UBI_C32_UBR_NAC_BIS( SRCP, DESTP, BLEND )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( BTABLE( (BYTE)~(SRCP)[0] ) ) - (DESTP)[0] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( GTABLE( (BYTE)~(SRCP)[1] ) ) - (DESTP)[1] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( RTABLE( (BYTE)~(SRCP)[2] ) ) - (DESTP)[2] ) * BPARAM * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * BIMG(BLEND) + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_UBI_C32_UBR_NAC_BIS_TCK( SRCP, DESTP, BLEND )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BIS( SRCP, DESTP, BLEND )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_UBI_C32_UBR_NAC_BIS_TCK_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_UBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, BLEND, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

    // ブレンド画像無し

      // 輝度無し

		// αチャンネルあり
		#define CODE_NBI_C32_NBR_UAC_BIS( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)~(SRCP)[0] - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)~(SRCP)[1] - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)~(SRCP)[2] - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_NBR_UAC_BIS_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_NBR_NAC_BIS( SRCP, DESTP )\
			else					CODE_NBI_C32_NBR_UAC_BIS( SRCP, DESTP )

		// αチャンネルなし
		#define CODE_NBI_C32_NBR_NAC_BIS( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)~(SRCP)[0] - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)~(SRCP)[1] - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)~(SRCP)[2] - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_NBR_NAC_BIS_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_NBR_NAC_BIS( SRCP, DESTP )

      // 輝度あり

		// αチャンネルあり
		#define CODE_NBI_C32_UBR_UAC_BIS( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( BTABLE( (BYTE)~(SRCP)[0] ) ) - (DESTP)[0] ) * BPARAM * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( GTABLE( (BYTE)~(SRCP)[1] ) ) - (DESTP)[1] ) * BPARAM * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( RTABLE( (BYTE)~(SRCP)[2] ) ) - (DESTP)[2] ) * BPARAM * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BIS_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT * SRCP[3] + ( (DESTP)[0] << 16 ) ) >> 16 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT * SRCP[3] + ( (DESTP)[1] << 16 ) ) >> 16 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT * SRCP[3] + ( (DESTP)[2] << 16 ) ) >> 16 ) ;\
		}

		// αチャンネルあり(チェック付き)
		#define CODE_NBI_C32_UBR_UAC_BIS_ACK( SRCP, DESTP )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BIS( SRCP, DESTP )\
			else					CODE_NBI_C32_UBR_UAC_BIS( SRCP, DESTP )

		// αチャンネルあり(チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_UAC_BIS_ACK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( (SRCP)[3] == 255 ) 	CODE_NBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )\
			else					CODE_NBI_C32_UBR_UAC_BIS_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )

		// αチャンネルなし
		#define CODE_NBI_C32_UBR_NAC_BIS( SRCP, DESTP )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( BTABLE( (BYTE)~(SRCP)[0] ) ) - (DESTP)[0] ) * BPARAM + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( GTABLE( (BYTE)~(SRCP)[1] ) ) - (DESTP)[1] ) * BPARAM + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( RTABLE( (BYTE)~(SRCP)[2] ) ) - (DESTP)[2] ) * BPARAM + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
		{\
			(DESTP)[0] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[0] * BBRIGHT ) >> 8 ) - (DESTP)[0] ) * ABRIGHT + ( (DESTP)[0] << 8 ) ) >> 8 ) ;\
			(DESTP)[1] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[1] * GBRIGHT ) >> 8 ) - (DESTP)[1] ) * ABRIGHT + ( (DESTP)[1] << 8 ) ) >> 8 ) ;\
			(DESTP)[2] = (BYTE)( ( ( (BYTE)( ( (BYTE)~(SRCP)[2] * RBRIGHT ) >> 8 ) - (DESTP)[2] ) * ABRIGHT + ( (DESTP)[2] << 8 ) ) >> 8 ) ;\
		}

		// αチャンネルなし(透過色チェック付き)
		#define CODE_NBI_C32_UBR_NAC_BIS_TCK( SRCP, DESTP )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BIS( SRCP, DESTP )

		// αチャンネルなし(透過色チェック付き)(テーブル非使用)
		#define CODE_NBI_C32_UBR_NAC_BIS_TCK_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )	\
			if( *((DWORD *)(SRCP)) != TransColor ) CODE_NBI_C32_UBR_NAC_BIS_NTBL( SRCP, DESTP, ABRIGHT, RBRIGHT, GBRIGHT, BBRIGHT )











// 構造体定義 --------------------------------------------------------------------

// 自由変形描画補助用ポリゴン描画関数
struct DX_POINTDATA
{
	int						x , y ;
	int						u , v ;
} ;

/*
解析データフォーマット

  解析情報は一つ１バイト
	
    2bit(BitMask 11000000):データの種類
      0(00B):描画する必要が無い
      1(01B):α値が最大なのでα値を適応する必要が無い
      2(10B):α値を適応する必要がある
      3(11B):行の終端

    6bit(BitMask 00111111):データの種類に基づくデータの長さ(最大64(0を1個と数える))
*/

// MEMIMG の Derivation しても共通な部分のデータ
struct MEMIMGBASE
{
	int						RefCount ;							// 参照カウント
	int						BaseWidth ;							// 幅
	int						BaseHeight ;						// 高さ
	unsigned int			Pitch ;								// ピッチ
	int						PitchPow2n ;						// ピッチが２のｎ乗だった時の左シフト数( -1 の場合は２のｎ乗ではない )
	COLORDATA				*ColorDataP ;						// カラーフォーマットへのポインタ
	int						UseTransColor ;						// 透過色は有効かどうか( 1:有効  0:無効 )
	unsigned int			TransColor ;						// 透過色(カラータイプ 0,1 の時のみ有効)
//	unsigned int			TransPalette ;						// 透過パレット番号(パレットがある時のみ有効)
	int						ColorType ;							// カラータイプ( 0:16bit(RG6B5 or X8A8R5G6B5)  1:32bit(XRGB8 又は ARGB8)  2:8bit(ブレンド画像)  3:16bit(Ｚバッファ) )
	int						UsePalette ;						// パレット使用の有無( 1:有り  0:無し ) カラータイプ 0,1 の時のみ有効
	int						ColorNum ;							// パレットで使用している色の数
	int						UseAlpha ;							// αチャンネルの有無( 1:有り  0:無し ) カラータイプ 0,1 の時のみ有効
	int						AlphaImageValid ;					// AlphaImage が有効かどうか( 1:有効  0:無効 )(カラータイプ 0,1 の時のみ有効)
	int						AnalysisDataValid ;					// AnalysisData が有効かどうか( 1:有効  0:無効 )(カラータイプ 0,1 の時のみ有効)
	int						UserImageFlag ;						// ユーザーが Image を用意したか、フラグ
	unsigned int			*Palette ;							// パレットへのポインタ、派生時以外は同メンバ配列の PaletteBase を使用する
	unsigned int			*OriginalPalette ;					// オリジナルパレットへのポインタ、派生時以外は同メンバ配列の OriginalPaletteBase を使用する
	unsigned char			*Image ;							// イメージデータへのアドレス
	unsigned char			*AlphaImage ;						// 予めαチャンネルの効果をイメージに適応したイメージ、UseAlpha と AlphaImageValid が１の時のみ有効(カラータイプ 0,1 の時のみ有効)
	unsigned char			*AnalysisData ;						// 透過色の連続回数やαチャンネルが最大値か最小値か、それが何ドット連続しているか等の解析情報、AnalysisDataValid が１の時のみ有効(カラータイプ 0,1 の時のみ有効)
} ;

// システムメモリ上の画像データ
struct MEMIMG
{
	int						InitializeCheck ;					// 初期化チェックデータ
	unsigned int			Width, Height ;						// サイズ
	unsigned char			*UseImage ;							// イメージデータの使用している領域へのアドレス
	unsigned char			*UseAlphaImage ;					// 予めαチャンネルの効果をイメージに適応したイメージの使用している領域へのアドレス、UseAlpha と AlphaImageValid が１の時のみ有効(カラータイプ 0,1 の時のみ有効)
	MEMIMGBASE				*Base ;								// 共通情報へのポインタ
} ;

// MEMIMG 管理構造体
struct MEMIMGMANAGE
{
	BYTE					RateTable16[64][64][64] ;			// 割合テーブル 左から [素値1の割合値(0=0% 63=100%][素値1][素値2]
	DWORD					RateTable[256][256] ;				// 割合テーブル 左から [割合値(0=0% 255=100%)][素値] 
	int						RateTable2[256][512] ;				// 割合テーブル 左から [割合値(0=0% 255=100%)][素値+255] 
	BYTE					BlendGraphTable[256] ;				// ブレンドイメージ適応時用テーブル
	int						DefaultColorType ;					// デフォルトのカラータイプ
	RECT					DrawArea ;							// 描画可能範囲

	int						BlendMode ;							// ブレンドモード
	int						BlendParam ;						// ブレンドパラメータ
	union
	{
		RGBCOLOR			DrawBright ;							// 描画輝度
		DWORD				bDrawBright ;
	} ;
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern MEMIMGMANAGE MemImgManage ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// メモリ画像データを処理する関数( DxMemImg.cpp )
extern	int		InitializeMemImgManage( void ) ;																													// MEMIMG 管理データの初期化

extern	int		SetMemImgDefaultColorType( int ColorType ) ;																										// デフォルトのカラータイプをセットする
extern	int		SetMemImgDrawArea( const RECT *DrawArea ) ;																											// MEMIMG用の描画範囲を設定する
extern	int		SetMemImgDrawBlendMode( int BlendMode, int BlendParam ) ;																							// MEMIMG用のブレンドモードとブレンドパラメータの設定を行う
extern	int		SetMemImgDrawBright( DWORD Bright ) ;																												// MEMIMG用の描画輝度

extern	int		LoadImageToMemImg( const wchar_t *FilePath, MEMIMG *MemImg, DWORD TransColor, int ColorType = -1/*-1=画面に合ったフォーマット*/, int UsePaletteFormat = 0 ) ;			// (補助関数)MEMIMG 構造体に画像を読み込む( これを呼んだ場合 InitializeMemImg は必要なし、あとMemImgは０クリアしておく必要は無い )( 戻り値: -1=失敗  0=成功 )
extern	int		MakeMemImgScreen( MEMIMG *Img, int Width, int Height, int ColorType = -1/*-1=画面に合ったフォーマット*/  ) ;										// (補助関数)描画対象用のメモリ画像を作成する( InitializeMemImg を簡略化しただけのもの、MemImgは０クリアしておく必要は無い )
extern	int		MakeMemImgZBuffer( MEMIMG *Img, int Width, int Height ) ;																							// (補助関数)Ｚバッファのメモリイメージを作成する( InitializeMemImg を簡略化しただけのもの、MemImgは０クリアしておく必要は無い )
extern	void	BltMemImg( MEMIMG *DestImg, const MEMIMG *SrcImg, const RECT *SrcRect/* NULL:全体 */, int DestX, int DestY ) ;										// (補助関数)MEMIMG 間でデータの転送を行う
extern	void	DerivationMemImg( MEMIMG *DestImg, MEMIMG *SrcImg, int SrcX, int SrcY, int Width, int Height ) ;													// ある MEMIMG の一部を使用する MEMIMG の情報を準備する(派生元が無効になったら派生 MEMIMG も使用不可になる、というか、一瞬だけ他の画像を間借りしたいとき位にしか使えない)

extern	int		InitializeMemImg( MEMIMG *Img, int Width, int Height, int Pitch/*規定値:-1*/, DWORD TransColor, int ColorType, int UsePalette, int UseAlpha, int AnalysisFlag = TRUE, const void *UserImage = NULL ) ;		// メモリ画像を初期化する、構造体はゼロ初期化されている必要がある( 戻り値: -1=失敗  0=成功 )
extern	int		CheckValidMemImg( const MEMIMG *Img ) ;																												// MEMIMG が有効かどうかを取得する
extern	void	TerminateMemImg( MEMIMG *Img ) ;																													// メモリ画像の後始末をする( 自前で初期化した場合は実行する必要なし( イメージ領域の解放だけなので ) )
extern	void	BltBaseImageToMemImg( const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, MEMIMG *MemImg, int SrcX, int SrcY, int Width, int Height, int DestX, int DestY, int UseTransColorConvAlpha = TRUE ) ;	// BASEIMAGE イメージから MEMIMG イメージに画像データを転送する( パレットも勝手に転送される )
extern	void	BltMemImgToBaseImage( BASEIMAGE *BaseImage, const MEMIMG *MemImg, int SrcX, int SrcY, int Width, int Height, int DestX, int DestY, DWORD TransColor = 0, int TransFlag = FALSE ) ;	// MEMIMG イメージから BASEIMAGE イメージに画像データを転送する
extern	void	ClearMemImg(            MEMIMG *MemImg, const RECT *FillArea = NULL, unsigned int Color = 0 ) ;																		// イメージを初期化する
extern	void	DrawMemImg(             MEMIMG *DestImg, const MEMIMG *SrcImg, int DestX, int DestY,                                              int TransFlag = TRUE, const MEMIMG *BlendImg = NULL ) ;							// イメージを描画する
extern	void	DrawTurnMemImg(         MEMIMG *DestImg, const MEMIMG *SrcImg, int XTurnFlag, int YTurnFlag, int DestX, int DestY,                int TransFlag = TRUE, const MEMIMG *BlendImg = NULL ) ;
extern	void	DrawEnlargeMemImg(      MEMIMG *DestImg, const MEMIMG *SrcImg, const RECT *DestRect,                                              int TransFlag = TRUE, const MEMIMG *BlendImg = NULL ) ;							// イメージを拡大描画する
extern	void	DrawEnlargeMemImg(      MEMIMG *DestImg, const MEMIMG *SrcImg, int x1, int y1, int x2, int y2,                                    int TransFlag = TRUE, const MEMIMG *BlendImg = NULL ) ;							// イメージを拡大描画する
extern	void 	DrawRotationMemImg(     MEMIMG *DestImg, const MEMIMG *SrcImg, int x, int y, float Angle, float EnlageRateX, float EnlageRateY,   int TransFlag = TRUE, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE, const MEMIMG *BlendImg = NULL ) ;		// イメージを回転描画する
extern	void	DrawBasicPolygonMemImg( MEMIMG *DestImg, const MEMIMG *SrcImg, const DX_POINTDATA *pos3,                                          int TransFlag = TRUE, const MEMIMG *BlendImg = NULL, unsigned int Color = 0xffffffff ) ;	// イメージを使用してポリゴンを描画する( ２Ｄ空間に画像を変形して描画するのが目的 )
extern	void	DrawTransformMemImg(    MEMIMG *DestImg, const MEMIMG *SrcImg, const POINT *pos4,                                                 int TransFlag = TRUE, const MEMIMG *BlendImg = NULL ) ;							// イメージを変形描画する
#ifndef DX_NON_GRAPHICS
extern	void	DrawPolygonMemImg(      MEMIMG *DestImg, MEMIMG *ZImg, const MEMIMG *SrcImg, const struct tagVERTEX_2D *Vertex3,                  int TransFlag = TRUE, const MEMIMG *BlendImg = NULL, int ZWriteFlag = TRUE, int GouraudShadeMode = TRUE, int PerspectiveEnable = TRUE, int ScissorTest = TRUE ) ;	// イメージを使用してポリゴンを描画する( ３Ｄ空間に３Ｄポリゴンを描画するのが目的 )
#endif // DX_NON_GRAPHICS

extern	DWORD	GetPixelColorMemImg( const MEMIMG *MemImg, int x, int y ) ;																								// イメージ中の指定座標の色を取得
extern	void	PaintMemImg(        MEMIMG *DestImg, int x, int y,                   unsigned int FillColor, ULONGLONG BoundaryColor ) ;								// 指定点から境界色があるところまで塗りつぶす
extern	void	DrawPixelMemImg(    MEMIMG *DestImg, int x, int y,                   unsigned int Color ) ;																// イメージに点を描画する
extern	void	DrawFillBoxMemImg(  MEMIMG *DestImg, const RECT *FillRect,           unsigned int Color ) ;																// イメージに中身のあるボックスを描画する
extern	void	DrawFillBoxMemImg(  MEMIMG *DestImg, int x1, int y1, int x2, int y2, unsigned int Color ) ;																// イメージに中身のあるボックスを描画する
extern	void	DrawLineMemImg(     MEMIMG *DestImg, int x1, int y1, int x2, int y2, unsigned int Color ) ;																// イメージに線を描画する
extern	void	DrawLineBoxMemImg(  MEMIMG *DestImg, int x1, int y1, int x2, int y2, unsigned int Color ) ;																// イメージに枠だけのボックスを描画する
extern 	void	DrawPixelSetMemImg( MEMIMG *DestImg, const POINTDATA *PointData, int Num ) ;																			// イメージに点の集合を描画する
extern	void	DrawLineSetMemImg(  MEMIMG *DestImg, const LINEDATA *LineData,   int Num ) ;																			// イメージに線の集合を描画する
extern	void	DrawCircleMemImg(   MEMIMG *DestImg, int x, int y, int r,            unsigned int Color, int FillFlag, int Rx_One_Minus = FALSE, int Ry_One_Minus = FALSE ) ;	// イメージに円を描画する
extern	void	DrawOvalMemImg(     MEMIMG *DestImg, int x, int y, int rx, int ry,   unsigned int Color, int FillFlag, int Rx_One_Minus = FALSE, int Ry_One_Minus = FALSE ) ;	// イメージに楕円を描画する

extern	void	SetBlendGraphParamMemImg( int BorderParam, int BorderRange ) ;																							// ブレンドグラフィック処理に必要なテーブルのセットアップを行う
extern	COLORDATA *GetMemImgColorData( int ColorType, int UseAlpha, int UsePalette ) ;																					// 指定のフォーマットのカラーデータを得る
//#ifdef __WINDOWS__
//extern	int		CreatePixelFormat(	D_DDPIXELFORMAT *PixelFormatBuf, int ColorBitDepth, DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask ) ;			// DDPIXELFORMATデータを作成する
//extern	D_DDPIXELFORMAT *GetMemImgPixelFormat( int ColorType, int UseAlpha, int UsePalette ) ;																		// 指定のフォーマットのカラーデータを得る
//#endif // __WINDOWS__

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif
