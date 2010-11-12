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
	virtual void Render( CObject *pObject, float dt );
	
	virtual void BeginDraw();
	virtual void EndDraw();
	
	virtual void OutputText( const TCHAR *text, float left, float top, DWORD color );

	virtual void ApplyRasterizerState();
	virtual void ApplyDepthStencilState();

	virtual void SetWireframe(bool bWireframe);

protected:
	CDX10Core();
	virtual ~CDX10Core( void );

	

	

	void RenderMeshObject(class CMeshObject *pObj, float dt);

	

	void InitMatrices(UINT width, UINT height);
	void InitFont();

	void InitBasicEffects();

	void InitRasterizerState();
	void InitDepthStencilState();
	void InitRenderAndDepthStencilTargets( int widht, int height );	
	
	void InitializeBuffers();
	void FillBuffers(CMeshObject *pObj);
	
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

	ID3D10Effect				*m_pBasicEffect;
	ID3D10EffectTechnique		*m_pBasicTechnique;
	ID3D10EffectMatrixVariable	*m_pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable	*m_pProjectionMatrixEffectVariable;
	ID3D10EffectMatrixVariable	*m_pWorldMatrixEffectVariable;

	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProjection;

	ID3D10Buffer *m_pVertexBuffers[VBT_COUNT];
	ID3D10Buffer *m_pIndexBuffer;
	ID3D10InputLayout *m_pVertexLayout;

	UINT currentVBPos, currentIBPos;
};

} // end namespace