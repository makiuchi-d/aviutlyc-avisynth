/******************************************************************************/
/** Array initialization macros for convert AviUtl YC48.
 * @file
 */
#ifndef ___AVIUTLYC_H
#define ___AVIUTLYC_H


#define SATURATE(min,max,x)  (((x)<(min))?(min):((max)<(x))?(max):(x))

#define ARR1(f,x)    (f(x))
#define ARR2(f,x)    ARR1(f,(x)),ARR1(f,(x+1))
#define ARR4(f,x)    ARR2(f,(x)),ARR2(f,(x+2))
#define ARR8(f,x)    ARR4(f,(x)),ARR4(f,(x+4))
#define ARR16(f,x)   ARR8(f,(x)),ARR8(f,(x+8))
#define ARR32(f,x)   ARR16(f,(x)),ARR16(f,(x+16))
#define ARR64(f,x)   ARR32(f,(x)),ARR32(f,(x+32))
#define ARR128(f,x)  ARR64(f,(x)),ARR64(f,(x+64))
#define ARR256(f,x)  ARR128(f,(x)),ARR128(f,(x+128))
#define ARR512(f,x)  ARR256(f,(x)),ARR256(f,(x+256))
#define ARR1024(f,x) ARR512(f,(x)),ARR512(f,(x+512))
#define ARR2048(f,x) ARR1024(f,(x)),ARR1024(f,(x+1024))
#define ARR4096(f,x) ARR2048(f,(x)),ARR2048(f,(x+2048))

// 4770 = 2 +32 +128 +512 +4096
#define ARR4770(f,x)  ARR2(f,(x)),ARR32(f,(x+2)),ARR128(f,(x+2+32)),\
		ARR512(f,(x+2+32+128)),ARR4096(f,(x+2+32+128+512))

// 4663 = 1 +2 +4 +16 +32 +512 +4096
#define ARR4663(f,x)  ARR1(f,(x)),ARR2(f,(x+1)),ARR4(f,(x+1+2)),\
		ARR16(f,(x+1+2+4)),ARR32(f,(x+1+2+4+16)),ARR512(f,(x+1+2+4+16+32)),\
		ARR4096(f,(x+1+2+4+16+32+512))

// 6144 = 4096+2048
#define ARR6144(f,x)  ARR4096(f,(x)),ARR2048(f,(x+4096))


/*===========================================================================*/
/**@defgroup YUVtoYC48
 *
 * @code
 *  y = ((Y * 1197)>>6) - 299;
 *  c = ((UV - 128)*4681 + 164) >> 8;
 * @endcode
 *  - Input range (Y,U,V): 0..255
 *  - Table type (y,c): signed short (more than 16 bits)
 *@{
 */
#define YUVtoYC_Y(y) ((((y) * 1197)>>6) - 299)
#define YUVtoYC_C(c) (((c)*4681 - 128*4681 + 164) >>8)
#define T_YUVtoYC_Y ARR256(YUVtoYC_Y,0)
#define T_YUVtoYC_C ARR256(YUVtoYC_C,0)
/*@}*/

/*===========================================================================*/
/**@defgroup YC48toYUV
 *
 * @code
 *   Y  = ((y*219 + 383)>>12) + 16;
 *   UV = (((c + 2048)*7 + 66)>>7) + 16;
 * @endcode
 *  - Input range (y): -299..4470 (4770)
 *  - Input range (c): -2340..2322 (4663)
 *  - Table type (Y,U,V): unsigned char (more than 8 bits)
 *@{
 */
#define YCtoYUV_Y(y) ((y*219 + 383 + (16<<12))>>12)
#define YCtoYUV_C(c) ((c*7 + 2048*7 + 66 + (16<<7))>>7)
#define T_YCtoYUV_Y ARR4770(YCtoYUV_Y,-299)
#define T_YCtoYUV_C ARR4663(YCtoYUV_C,-2340)
/*@}*/

/*===========================================================================*/
/**@defgroup RGBtoYC48
 *
 * @code
 *   y  = (( 4918*R+354)>>10)+(( 9655*G+585)>>10)+(( 1875*B+523)>>10);
 *   cb = ((-2775*R+240)>>10)+((-5449*G+515)>>10)+(( 8224*B+256)>>10);
 *   cr = (( 8224*R+256)>>10)+((-6887*G+110)>>10)+((-1337*B+646)>>10);
 * @endcode
 *  - Input range (R,G,B): 0..255
 *  - Table type: signed short (more than 16 bits)
 *
 * calculation:
 * @code
 *   y  = table_RtoY[R]  + table_GtoY[G]  + table_BtoY[B];
 *   cb = table_RtoCB[R] + table_GtoCB[G] + table_BtoCB[B];
 *   cr = table_RtoCR[R] + table_GtoCR[G] + table_BtoCR[B];
 * @endcode
 *
 * @note
 *  BtoCB and RtoCR are same table.
 *@{
 */
#define RGBtoYC_RtoY(r)  ((4918*r +354)>>10)
#define RGBtoYC_GtoY(g)  ((9655*g +585)>>10)
#define RGBtoYC_BtoY(b)  ((1875*b +523)>>10)
#define RGBtoYC_RtoCB(r) ((-2775*r +240)>>10)
#define RGBtoYC_GtoCB(g) ((-5449*g +515)>>10)
#define RGBtoYC_GtoCR(g) ((-6887*g +110)>>10)
#define RGBtoYC_BtoCR(b) ((-1337*b +646)>>10)
#define RGBtoYC_BtoCB_RtoCR(x) (( 8224*x +256)>>10)

#define T_RGBtoYC_RtoY   ARR256(RGBtoYC_RtoY,0)
#define T_RGBtoYC_GtoY   ARR256(RGBtoYC_GtoY,0)
#define T_RGBtoYC_BtoY   ARR256(RGBtoYC_BtoY,0)
#define T_RGBtoYC_RtoCB  ARR256(RGBtoYC_RtoCB,0)
#define T_RGBtoYC_GtoCB  ARR256(RGBtoYC_GtoCB,0)
#define T_RGBtoYC_GtoCR  ARR256(RGBtoYC_GtoCR,0)
#define T_RGBtoYC_BtoCR  ARR256(RGBtoYC_BtoCR,0)
#define T_RGBtoYC_BtoCB_RtoCR ARR256(RGBtoYC_BtoCB_RtoCR,0)

#define RGBtoYC_Y(r,g,b)  (RGBtoYC_RtoY(r) + RGBtoYC_GtoY(g) + RGBtoYC_BtoY(b))
#define RGBtoYC_CB(r,g,b) (RGBtoYC_RtoCB(r) + RGBtoYC_GtoCB(g) + RGBtoYC_BtoCB_RtoCR(b))
#define RGBtoYC_CR(r,g,b) (RGBtoYC_BtoCB_RtoCR(r) + RGBtoYC_GtoCR(g) + RGBtoYC_BtoCR(b))
/*@}*/

/*===========================================================================*/
/**@defgroup YC48toRGB
 *
 * @code
 *   R = (255*y +(((22881*cr>>16)+3)<<10))>>12;
 *   G = (255*y +(((-5616*cb>>16)+(-11655*cr>>16)+3)<<10))>>12;
 *   B = (255*y +(((28919*cb>>16)+3)<<10))>>12;
 * @endcode
 * - Input range (y):  -1024..5119 (saturate before calculation)
 * - Input range (cb,cr): -3072..3071 (saturate before calculation)
 * - Table type: signed long (more than 32 bits)
 *
 * calculation:
 * @code
 *  R = (table_YtoRGB[y] + table_CRtoR[cr]) >>12;
 *  G = (table_YtoRGB[y] + table_CBtoG[cb] + table_CRtoG[cr]) >>12;
 *  B = (table_YtoRGB[y] + table_CBtoB[cb]) >>12;
 * @endcode
 *  Calculated result can be overflow from the range of unsigned char.
 *  Results must be saturated (0..255).
 *
 * @note
 *  Originally AviUtl doesn't saturate yc values before calculation.
 *  However, when using table, allocating impossibly out from range is wasteful.
 *  These macros limit the range with enough amount (-1024 and +1023),
 *  which is much wider than YUV extra range.
 *  Saturate like these:
 *   - y: SATURATE(0,6143,y+1024)
 *   - cb,cr: SATURATE(0,1643,cbcr+3072)
 *@{
 */
#define YCtoRGB_YtoRGB(y)  (255*y)
#define YCtoRGB_CRtoR(cr)  ((( 22881*cr>>16)+3)<<10)
#define YCtoRGB_CBtoG(cb)  (( -5616*cb>>16)<<10)
#define YCtoRGB_CRtoG(cr)  (((-11655*cr>>16)+3)<<10)
#define YCtoRGB_CBtoB(cb)  ((( 28919*cb>>16)+3)<<10)

#define T_YCtoRGB_YtoRGB  ARR6144(YCtoRGB_YtoRGB,-1024)
#define T_YCtoRGB_CRtoR   ARR6144(YCtoRGB_CRtoR,-2048-1024)
#define T_YCtoRGB_CBtoG   ARR6144(YCtoRGB_CBtoG,-2048-1024)
#define T_YCtoRGB_CRtoG   ARR6144(YCtoRGB_CRtoG,-2048-1024)
#define T_YCtoRGB_CBtoB   ARR6144(YCtoRGB_CBtoB,-2048-1024)

#define YCtoRGB_R(y,cb,cr) ((YCtoRGB_YtoRGB(y) + YCtoRGB_CRtoR(cr)) >>12)
#define YCtoRGB_G(y,cb,cr) ((YCtoRGB_YtoRGB(y) + YCtoRGB_CBtoG(cb) + YCtoRGB_CRtoG(cr)) >>12)
#define YCtoRGB_B(y,cb,cr) ((YCtoRGB_YtoRGB(y) + YCtoRGB_CBtoB(cb)) >>12)
/*@}*/


#endif
