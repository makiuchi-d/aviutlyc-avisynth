/*****************************************************************************/
/** Convert To AviUtlYC Fix ver.1.00  by MakKi
 * @mainpage
 *
 * Interconversion of AviUtlYC and YUY2/RGB24/RGB32.
 *
 * Function:
 *  - ConvertYUY2toAviUtlYCFix(clip)  (backward compatibility)
 *  - ConvertToAviUtlYCFix(clip)
 *  - ConvertAviUtlYCtoYUY2Fix(clip)
 *  - ConvertAviUtlYCtoRGBFix(clip)
 *  - ConvertAviUtlYCtoRGBAFix(clip)
 *
 * License (GPL):
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
 *  http://www.gnu.org/copyleft/gpl.html .
 *
 * @file
 *  Main source.
 */
#include <windows.h>
#include "avisynth.h"
#include "filter.h"
#include "aviutlyc.h"


/*===========================================================================*/
/* Calculation */

/*---------------------------------------------------------------------------*/
/** YUV to YC.
 */
class YUVtoYC {
	static const short tY[];
	static const short tC[];
public:
	inline static short Y(unsigned char y){ return tY[y]; }
	inline static short C(unsigned char c){ return tC[c]; }
};

const short YUVtoYC::tY[] = { T_YUVtoYC_Y };
const short YUVtoYC::tC[] = { T_YUVtoYC_C };

/*---------------------------------------------------------------------------*/
/** YC to YUV.
 */
class YCtoYUV {
	static const unsigned char tY[];
	static const unsigned char tC[];
	static const unsigned char *pY;
	static const unsigned char *pC;
public:
	static unsigned char Y(int y){ return pY[SATURATE(-299,4470,y)]; }
	static unsigned char C(int c){ return pC[SATURATE(-2340,2322,c)]; }
};

const unsigned char YCtoYUV::tY[] = { T_YCtoYUV_Y };
const unsigned char YCtoYUV::tC[] = { T_YCtoYUV_C };
const unsigned char *YCtoYUV::pY = &YCtoYUV::tY[299];
const unsigned char *YCtoYUV::pC = &YCtoYUV::tC[2340];

/*---------------------------------------------------------------------------*/
/** RGB to YC.
 */
class RGBtoYC {
	static const short tRtoY[];
	static const short tGtoY[];
	static const short tBtoY[];
	static const short tRtoCB[];
	static const short tGtoCB[];
	static const short tGtoCR[];
	static const short tBtoCR[];
	static const short tBRtoCBCR[];

public:
	static short Y(unsigned char b,unsigned char g,unsigned char r){
		return tRtoY[r] + tGtoY[g] + tBtoY[b];
	}
	static short Cb(unsigned char b,unsigned char g,unsigned char r){
		return tRtoCB[r] + tGtoCB[g] + tBRtoCBCR[b];
	}
	static short Cr(unsigned char b,unsigned char g,unsigned char r){
		return tBRtoCBCR[r] + tGtoCR[g] + tBtoCR[b];
	}
};

const short RGBtoYC::tRtoY[] = { T_RGBtoYC_RtoY };
const short RGBtoYC::tGtoY[] = { T_RGBtoYC_GtoY };
const short RGBtoYC::tBtoY[] = { T_RGBtoYC_BtoY };
const short RGBtoYC::tRtoCB[] = { T_RGBtoYC_RtoCB };
const short RGBtoYC::tGtoCB[] = { T_RGBtoYC_GtoCB };
const short RGBtoYC::tGtoCR[] = { T_RGBtoYC_GtoCR };
const short RGBtoYC::tBtoCR[] = { T_RGBtoYC_BtoCR };
const short RGBtoYC::tBRtoCBCR[] = { T_RGBtoYC_BtoCB_RtoCR };

/*---------------------------------------------------------------------------*/
/** YC to RGB.
 *
 * Don't use array caluclations. They are too slow.
 */
class YCtoRGB {

public:
	static unsigned char R(int y,int cb,int cr){
		return SATURATE(0,255,YCtoRGB_R(y,cb,cr));
	}
	static unsigned char G(int y,int cb,int cr){
		return SATURATE(0,255,YCtoRGB_G(y,cb,cr));
	}
	static unsigned char B(int y,int cb,int cr){
		return SATURATE(0,255,YCtoRGB_B(y,cb,cr));
	}
};


/*===========================================================================*/
/* Filter Classes */

/** Base class for Convert to AviUtlYC.
 * @param PIXEL_TYPE Color format of the source. VideoInfo::CS_YUY2, VideoInfo::CS_BGR24 or VideoInfo::CS_BGR32.
 */
template <int PIXEL_TYPE>
class ConvertToAviUtlYCFix : public GenericVideoFilter {

protected:
	unsigned int width;
	unsigned int height;

	void Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch);

public:
	ConvertToAviUtlYCFix(const PClip& clip,IScriptEnvironment *env)
		: GenericVideoFilter(clip)
	{
		height  = vi.height;
		width = vi.width;
		vi.width *= 3;
		vi.pixel_type = VideoInfo::CS_YUY2;
		child->SetCacheHints(CACHE_NOTHING, 0);
	}
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment *env)
	{
		PVideoFrame srcFrame(child->GetFrame(n, env));
		PVideoFrame dstFrame(env->NewVideoFrame(vi,sizeof(PIXEL_YC)));

		Convert(dstFrame->GetWritePtr(),dstFrame->GetPitch(),
				srcFrame->GetReadPtr(),srcFrame->GetPitch());

		return dstFrame;
	}
};

/** Base class for Convert from AviUtlYC.
 * @param PIXEL_TYPE Color format of the destination. VideoInfo::CS_YUY2, VideoInfo::CS_BGR24 or VideoInfo::CS_BGR32.
 */
template <int PIXEL_TYPE>
class ConvertFromAviUtlYCFix : public GenericVideoFilter {

protected:
	unsigned int width;
	unsigned int height;

	void Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch);

public:
	ConvertFromAviUtlYCFix(const PClip& clip,IScriptEnvironment *env)
		: GenericVideoFilter(clip)
	{
		vi.pixel_type = PIXEL_TYPE;
		height = vi.height;
		width = vi.width /= 3;
		child->SetCacheHints(CACHE_NOTHING, 0);
	}
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment *env)
	{
		PVideoFrame srcFrame(child->GetFrame(n, env));
		PVideoFrame dstFrame(env->NewVideoFrame(vi));

		Convert(dstFrame->GetWritePtr(),dstFrame->GetPitch(),
				srcFrame->GetReadPtr(),srcFrame->GetPitch());

		return dstFrame;
	}
};

/*---------------------------------------------------------------------------*/
/* Specialization */

/** Convert YUY2 to AviUtlYC.
 */
template<>
void ConvertToAviUtlYCFix<VideoInfo::CS_YUY2>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	for(int h=height;h;--h){
		PIXEL_YC *yc = reinterpret_cast<PIXEL_YC*>(dst);
		const BYTE *yuv = src;

		yc[0].y  = YUVtoYC::Y(yuv[0]);
		yc[0].cb = YUVtoYC::C(yuv[1]);
		yc[0].cr = YUVtoYC::C(yuv[3]);
		yc[1].y  = YUVtoYC::Y(yuv[2]);
		yc += 2;
		yuv += 4;
		for(int w=width/2-1;w;--w){
			yc[0].y  = YUVtoYC::Y(yuv[0]);
			yc[0].cb = YUVtoYC::C(yuv[1]);
			yc[0].cr = YUVtoYC::C(yuv[3]);
			yc[1].y  = YUVtoYC::Y(yuv[2]);
			yc[-1].cb = (yc[-2].cb + yc[0].cb) >>1;
			yc[-1].cr = (yc[-2].cr + yc[0].cr) >>1;
			yc += 2;
			yuv += 4;
		}
		yc[-1].cb = yc[-2].cb;
		yc[-1].cr = yc[-2].cr;

		dst += dst_pitch;
		src += src_pitch;
	}
}

/** Convert AviUtlYC to YUY2.
 */
template<>
void ConvertFromAviUtlYCFix<VideoInfo::CS_YUY2>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	for(int h=height;h;--h){
		BYTE *yuv = dst;
		const PIXEL_YC *yc = reinterpret_cast<const PIXEL_YC*>(src);

		for(int w=width/2;w;--w){
			yuv[0] = YCtoYUV::Y(yc->y);
			yuv[1] = YCtoYUV::C(yc->cb);
			yuv[2] = YCtoYUV::Y((yc+1)->y);
			yuv[3] = YCtoYUV::C(yc->cr);
			yc += 2;
			yuv += 4;
		}

		dst += dst_pitch;
		src += src_pitch;
	}
}

/** Convert 24bit-RGB to AviUtlYC.
 */
template<>
void ConvertToAviUtlYCFix<VideoInfo::CS_BGR24>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	src += (height-1) * src_pitch;
	for(int h=height;h;--h){
		PIXEL_YC *yc = reinterpret_cast<PIXEL_YC*>(dst);
		const BYTE *rgb = src;

		for(int w=width;w;--w){
			yc->y  = RGBtoYC::Y(rgb[0],rgb[1],rgb[2]);
			yc->cb = RGBtoYC::Cb(rgb[0],rgb[1],rgb[2]);
			yc->cr = RGBtoYC::Cr(rgb[0],rgb[1],rgb[2]);
			++yc;
			rgb += 3;
		}
		dst += dst_pitch;
		src -= src_pitch;
	}
}

/** Convert 24bit-RGB to AviUtlYC.
 */
template<>
void ConvertFromAviUtlYCFix<VideoInfo::CS_BGR24>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	dst += (height-1) * dst_pitch;
	for(int h=height;h;--h){
		BYTE *rgb = dst;
		const PIXEL_YC *yc = reinterpret_cast<const PIXEL_YC*>(src);

		for(int w=width;w;--w){
			rgb[0] = YCtoRGB::B(yc->y,yc->cb,yc->cr);
			rgb[1] = YCtoRGB::G(yc->y,yc->cb,yc->cr);
			rgb[2] = YCtoRGB::R(yc->y,yc->cb,yc->cr);
			rgb += 3;
			++yc;
		}
		dst -= dst_pitch;
		src += src_pitch;
	}
}


/** Convert 32bit-RGB to AviUtlYC.
 */
template<>
void ConvertToAviUtlYCFix<VideoInfo::CS_BGR32>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	src += (height-1) * src_pitch;
	for(int h=height;h;--h){
		PIXEL_YC *yc = reinterpret_cast<PIXEL_YC*>(dst);
		const BYTE *rgb = src;

		for(int w=width;w;--w){
			yc->y  = RGBtoYC::Y(rgb[0],rgb[1],rgb[2]);
			yc->cb = RGBtoYC::Cb(rgb[0],rgb[1],rgb[2]);
			yc->cr = RGBtoYC::Cr(rgb[0],rgb[1],rgb[2]);
			++yc;
			rgb += 4;
		}
		dst += dst_pitch;
		src -= src_pitch;
	}
}

/** Convert AviUtlYC to 32bit-RGB.
 */
template<>
void ConvertFromAviUtlYCFix<VideoInfo::CS_BGR32>::Convert(BYTE *dst,int dst_pitch,const BYTE *src,int src_pitch)
{
	dst += (height-1) * dst_pitch;
	for(int h=height;h;--h){
		BYTE *rgb = dst;
		const PIXEL_YC *yc = reinterpret_cast<const PIXEL_YC*>(src);

		for(int w=width;w;--w){
			rgb[0] = YCtoRGB::B(yc->y,yc->cb,yc->cr);
			rgb[1] = YCtoRGB::G(yc->y,yc->cb,yc->cr);
			rgb[2] = YCtoRGB::R(yc->y,yc->cb,yc->cr);
			rgb[3] = 0;	// alpha
			rgb += 4;
			++yc;
		}
		dst -= dst_pitch;
		src += src_pitch;
	}
}

/*===========================================================================*/
/* Create Functions */

AVSValue __cdecl Create_ConvertToAviUtlYCFix(AVSValue args, void *user_data, IScriptEnvironment *env)
{
	PClip child(args[0].AsClip());
	const VideoInfo &vi = child->GetVideoInfo();
	int pixel_type = reinterpret_cast<int>(user_data);

	if(pixel_type==VideoInfo::CS_YUY2){
		if(!vi.IsYUY2()){
			env->ThrowError("ConvertYUY2toAviUtlYCFix: Input video must be in YUY2.");
		}
		return new ConvertToAviUtlYCFix<VideoInfo::CS_YUY2>(child,env);
	}

	if(vi.IsYUY2()){
		return new ConvertToAviUtlYCFix<VideoInfo::CS_YUY2>(child,env);
	}
	if(vi.IsRGB24()){
		return new ConvertToAviUtlYCFix<VideoInfo::CS_BGR24>(child,env);
	}
	if(vi.IsRGB32()){
		return new ConvertToAviUtlYCFix<VideoInfo::CS_BGR32>(child,env);
	}

	env->ThrowError("ConvertToAviUtlYCFix: Input video must be in YUY2, RGB24 or RGB32.");
	return 0;	// dummy
}


AVSValue __cdecl Create_ConvertFromAviUtlYCFix(AVSValue args, void *user_data, IScriptEnvironment *env)
{
	PClip child(args[0].AsClip());
	const VideoInfo &vi = child->GetVideoInfo();
	int pixel_type = reinterpret_cast<int>(user_data);

	// AviUtlYC has width of the multipul of 3, and its pixel type is YUY2.
	// See ConvertToAviUtlYC::ConvertToAviUtlYC()
	if(!vi.IsYUY2() || vi.width%3!=0){
		env->ThrowError("ConvertAviUtlYCto%sFix: Input video must be in AviUtlYC.",
						pixel_type==VideoInfo::CS_YUY2?"YUY2":pixel_type==VideoInfo::CS_BGR24?"RGB24":"RGB32");
	}

	switch(pixel_type){
	case VideoInfo::CS_YUY2:
		return new ConvertFromAviUtlYCFix<VideoInfo::CS_YUY2>(child,env);
	case VideoInfo::CS_BGR24:
		return new ConvertFromAviUtlYCFix<VideoInfo::CS_BGR24>(child,env);
	case VideoInfo::CS_BGR32:
		return new ConvertFromAviUtlYCFix<VideoInfo::CS_BGR32>(child,env);
	default:
		return 0; // dummy
	}
}


/*****************************************************************************/
/** Export Function.
 * Add functions to Avisynth.
 */
extern "C" __declspec(dllexport)
const char * __stdcall AvisynthPluginInit2(IScriptEnvironment *env)
{
	env->AddFunction("ConvertYUY2toAviUtlYCFix","c",
					 Create_ConvertToAviUtlYCFix,reinterpret_cast<void*>(VideoInfo::CS_YUY2));
	env->AddFunction("ConvertToAviUtlYCFix","c",
					 Create_ConvertToAviUtlYCFix,0);

	env->AddFunction("ConvertAviUtlYCtoYUY2Fix","c",
					 Create_ConvertFromAviUtlYCFix,reinterpret_cast<void*>(VideoInfo::CS_YUY2));
	env->AddFunction("ConvertAviUtlYCtoRGB24Fix","c",
					 Create_ConvertFromAviUtlYCFix,reinterpret_cast<void*>(VideoInfo::CS_BGR24));
	env->AddFunction("ConvertAviUtlYCtoRGB32Fix","c",
					 Create_ConvertFromAviUtlYCFix,reinterpret_cast<void*>(VideoInfo::CS_BGR32));
	return NULL;
}

