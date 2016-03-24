/******************************************************************************
*	ConvertYUY2ToAviUtlYCFix
*	ConvertAviUtlYCToYUY2Fix
******************************************************************************/
#include <windows.h>
#include "avisynth.h"
#include "filter.h"

#include <stdio.h>

/*=============================================================================
*	ConvertYUY2ToAviUtlYCFix
*		Yout = ((Yin * 1197)>>6) - 299;
*		Cout = ((Cin - 128)*4681 + 164)>>8;
*============================================================================*/
class ConvertYUY2ToAviUtlYCFix : public GenericVideoFilter
{
	ConvertYUY2ToAviUtlYCFix(const PClip& clip,IScriptEnvironment *env)
		: GenericVideoFilter(clip)
	{
		vi.width *= 3;
		child->SetCacheHints(CACHE_NOTHING, 0);
	}

	static AVSValue __cdecl Create(AVSValue args, void *user_data, IScriptEnvironment *env)
	{
		PClip child(args[0].AsClip());
		const VideoInfo& vi = child->GetVideoInfo();

		if(!vi.IsYUY2())
			env->ThrowError("%s: YUY2ê—p", GetName());

		return new ConvertYUY2ToAviUtlYCFix(child, env);
	}

public:
	static const char *GetName()
	{ return "ConvertYUY2ToAviUtlYCFix"; }

	static void PluginInit(IScriptEnvironment *env)
	{ env->AddFunction(GetName(), "c", Create, 0); }

	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment *env)
	{
		PVideoFrame srcFrame(child->GetFrame(n, env));
		PVideoFrame dstFrame(env->NewVideoFrame(vi, sizeof(PIXEL_YC)));

		Convert(reinterpret_cast<PIXEL_YC *>(dstFrame->GetWritePtr()),dstFrame->GetPitch(),
				srcFrame->GetReadPtr(),srcFrame->GetPitch(),vi.width / 3, vi.height);

		return dstFrame;
	}

private:
	// Yout = ((Yin * 1197)>>6) - 299;
	// Cout = ((Cin - 128)*4681 + 164)>>8;
	int YtoY(int y){ return ((y * 1197) >>6) - 299; }
	int UVtoC(int uv){ return (uv*4681 - 128*4681 + 164)>>8; }

	void Convert(PIXEL_YC *yc,int ycPitch,const BYTE *yuv,int yuvPitch,int width,int height)
	{
		for(int h=height; h; --h){
			PIXEL_YC *_yc = yc;
			const BYTE *_yuv = yuv;

			_yc[0].y = YtoY(_yuv[0]);
			_yc[1].y = YtoY(_yuv[2]);
			_yc[0].cb = UVtoC(_yuv[1]);
			_yc[0].cr = UVtoC(_yuv[3]);
			_yc  += 2;
			_yuv += 4;

			for(int w=width/2-1; w; --w){
				_yc[0].y = YtoY(_yuv[0]);
				_yc[1].y = YtoY(_yuv[2]);
				_yc[0].cb = UVtoC(_yuv[1]);
				_yc[0].cr = UVtoC(_yuv[3]);
				_yc[-1].cb = (_yc[-2].cb + _yc[0].cb)>>1;
				_yc[-1].cr = (_yc[-2].cr + _yc[0].cr)>>1;
				_yc += 2;
				_yuv += 4;
			}
			_yc[-1].cb = _yc[-2].cb;
			_yc[-1].cr = _yc[-2].cr;

			reinterpret_cast<BYTE *&>(yc) += ycPitch;
			yuv += yuvPitch;
		}
	}
};

/*=============================================================================
*	ConvertAviUtlYCToYUY2Fix
*		Yout = ((Yin*219 + 383)>>12) + 16;
*		Cout = (((Cin + 2048)*7 + 66)>>7) + 16; 
*============================================================================*/
class ConvertAviUtlYCToYUY2Fix : public GenericVideoFilter
{
	ConvertAviUtlYCToYUY2Fix(const PClip& clip, IScriptEnvironment *env)
		: GenericVideoFilter(clip)
	{
		vi.width /= 3;
		child->SetCacheHints(CACHE_NOTHING, 0);
	}

	static AVSValue __cdecl Create(AVSValue args, void *user_data, IScriptEnvironment *env) {
		PClip child(args[0].AsClip());
		const VideoInfo& vi = child->GetVideoInfo();

		if(!vi.IsYUY2())
			env->ThrowError("%s: YUY2ê—p", GetName());

		return new ConvertAviUtlYCToYUY2Fix(child, env);
	}


public:
	static const char *GetName()
	{ return "ConvertAviUtlYCToYUY2Fix"; }

	static void PluginInit(IScriptEnvironment *env)
	{ env->AddFunction(GetName(), "c", Create, 0); }

	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment *env) {
		PVideoFrame srcFrame(child->GetFrame(n, env));
		PVideoFrame dstFrame(env->NewVideoFrame(vi));

		Convert(dstFrame->GetWritePtr(), dstFrame->GetPitch(),
			reinterpret_cast<const PIXEL_YC *>(srcFrame->GetReadPtr()),
			srcFrame->GetPitch(), vi.width, vi.height);

		return dstFrame;
	}

private:
	int Clamp(int n, int l, int h)
	{ return (n < l) ? l : (n > h) ? h : n; }

//		Yout = ((Yin*219 + 383)>>12) + 16;
//		Cout = (((Cin + 2048)*7 + 66)>>7) + 16; 
	int YtoY(int y){ return Clamp((y*219 + 383 + (16<<12))>>12,0,255); }
	int CtoUV(int c){ return Clamp((c*7 + 2048*7 + 66 + (16<<7))>>7,0,255); }

	void Convert(BYTE *yuv,int yuvPitch,const PIXEL_YC *yc,int ycPitch,int width, int height)
	{
		for(int h=height; h; --h){
			BYTE *_yuv = yuv;
			const PIXEL_YC *_yc = yc;

			for(int w=width/2; w; --w){
				_yuv[0] = YtoY(_yc[0].y);
				_yuv[1] = CtoUV(_yc[0].cb);
				_yuv[2] = YtoY(_yc[1].y);
				_yuv[3] = CtoUV(_yc[0].cr);
				_yc  += 2;
				_yuv += 4;
			}
			yuv += yuvPitch;
			reinterpret_cast<const BYTE *&>(yc) += ycPitch;
		}
	}
};



/******************************************************************************
*	AvisynthPluginInit2
******************************************************************************/
extern "C" __declspec(dllexport)
const char * __stdcall AvisynthPluginInit2(IScriptEnvironment *env)
{
	ConvertYUY2ToAviUtlYCFix::PluginInit(env);
	ConvertAviUtlYCToYUY2Fix::PluginInit(env);
	return NULL;
}
