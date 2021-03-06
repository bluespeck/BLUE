#pragma once

#include <d3d11.h>
#include <d3dx11.h>

#include "DXCore.h"
#include "DXUtils.h"

namespace BLUE
{

class CDX11Core: public IDXCore
{
public:	
	static IDXCore *GetInstance();

	// overrides from CDXCore
	virtual void ResetDevice();
	virtual void CleanupDevice();

	virtual void *GetDevice();
	virtual void *GetSwapChain();
	virtual void *GetDeviceContext();

	virtual bool CDX11Core::Init( HWND hWnd );
	virtual bool CDX11Core::Init( HWND hWnd, const DXInitDesc &initDesc );

	virtual void Render( CObject *pObj, float dt );
	
	virtual void OnResize(int width, int height);	

	virtual void OutputText( const TCHAR *TEXT, float left, float top, DWORD color );

	virtual void ApplyRasterizerState();
	virtual void ApplyDepthStencilState();

	virtual void SetWireframe(bool bWireframe);
protected:
	CDX11Core();
	virtual ~CDX11Core( void );



	virtual void BeginDraw();
	virtual void EndDraw();

	

	void InitFont();	

protected:
	IDXGISwapChain			*m_pSwapChain;
	ID3D11Device			*m_pDevice;
	ID3D11DeviceContext		*m_pDeviceContext;

	ID3D11RenderTargetView	*m_pRenderTargetView;
	D3D_FEATURE_LEVEL		m_featureLevel;			// the feature level for which the device is created
	
	bool					m_bShowEngineInfo;

};

} // end namespace