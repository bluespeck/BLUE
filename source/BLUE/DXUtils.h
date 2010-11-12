#pragma once

#ifndef UNICODE
#error "BLUE requires a Unicode build."
#endif

#include <windows.h>
#include <d3d11.h>
#include "Vector2.h"
#include "Vector3.h"

namespace BLUE
{

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
	HRESULT hr = (x);                                      \
	if(FAILED(hr))                                         \
		{                                                      \
		DXTrace(__FILE__, (DWORD)__LINE__, hr, _T(#x), true); \
		exit(1);	\
		}                                                      \
	}
#endif
#else
#define HR(x) { x; }
#endif

#define SAFE_DX_RELEASE(x) {if(x) { x->Release(); x = NULL;}}

#define MAX_STRING_LENGTH 256

struct DXInitDesc
{
	struct DXRational
	{
		unsigned int numerator;
		unsigned int denominator;
	};

	struct DXBufferDesc
	{
		unsigned int width;
		unsigned int height;
		DXRational refreshRate;
		int format;
		int scanlineOrdering;
		int scaling;
	} bufferDesc;

	struct DXSampleDesc
	{
		unsigned int count;
		unsigned int quality;
	} sampleDesc;

	unsigned int bufferUsage;
	unsigned int bufferCount;
	HWND hOutputWindow;
	BOOL bWindowed;
	unsigned int swapEffect;
	UINT flags;
};

enum VertexBufferTypes
{
	VBT_POSITION0,
	VBT_COLOR0,
	VBT_NORMAL0,
	VBT_TEXCOORD0,
	VBT_TEXCOORD1,
	VBT_COUNT
};

//HRESULT CompileShaderFromFile( TCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const D3D_SHADER_MACRO* pDefines = NULL );

}
