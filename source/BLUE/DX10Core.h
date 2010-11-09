#pragma once

#include <d3d10_1.h>

#include <d3dx10.h>

#include "DXCore.h"
#include "DXUtils.h"

namespace BLUE
{

class CDX10Core: public IDXCore
{
public:
	static IDXCore *GetInstance();

	// overrides from CDXCore
	virtual void ResetDevice();
	virtual void CleanupDevice();

	virtual void *GetDevice();
	virtual void *GetSwapChain();	

	virtual bool CDX10Core::Init( HWND hWnd );
	virtual bool CDX10Core::Init( HWND hWnd, const DXInitDesc &initDesc );

	virtual void OnResize( int width, int height );
	virtual void Update( float dt );
	virtual void Render( float dt );
protected:
	CDX10Core();
	virtual ~CDX10Core( void );

	

	virtual void BeginDraw();
	virtual void EndDraw();

	void OutputText( const TCHAR *TEXT, float left, float top, unsigned int color );

	void InitFont();	
	void InitRasterizerState();
	void InitDepthStencilState();
	void InitRenderAndDepthStencilTargets( int widht, int height );	
	
	void OutputEngineInfo();

	void DrawCoordAxes();

protected:
	IDXGISwapChain			*m_pSwapChain;
	ID3D10Device1			*m_pDevice;
	

	ID3D10RenderTargetView	*m_pRenderTargetView;
	ID3D10DepthStencilView	*m_pDepthStencilView;
	D3D_FEATURE_LEVEL		m_featureLevel;			// the feature level for which the device is created
	
	bool					m_bShowEngineInfo;
	ID3DX10Font				*m_pFont;

	ID3D10RasterizerState	*m_pRasterizerState;
	ID3D10DepthStencilState *m_pDepthStencilState;

};

} // end namespace