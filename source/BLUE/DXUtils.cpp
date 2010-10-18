#include "stdafx.h"
#include "DXUtils.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11async.h>

namespace BLUE
{
	HRESULT CompileShaderFromFile( TCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const D3D_SHADER_MACRO* pDefines )
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		#if defined( DEBUG ) || defined( _DEBUG )
			// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
			// Setting this flag improves the shader debugging experience, but still allows 
			// the shaders to be optimized and to run exactly the way they will run in 
			// the release configuration of this program.
			dwShaderFlags |= D3DCOMPILE_DEBUG;
		#endif

		// compile the shader
		ID3DBlob* pErrorBlob = NULL;
		hr = D3DX11CompileFromFile( szFileName, pDefines, NULL, szEntryPoint, szShaderModel, dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL );
		if( FAILED(hr) )
		{
			if( pErrorBlob != NULL )
				OutputDebugStringA( ( char* )pErrorBlob->GetBufferPointer() );
			SAFE_DX_RELEASE( pErrorBlob );
			return hr;
		}
		SAFE_DX_RELEASE( pErrorBlob );

		return S_OK;   
	}

}