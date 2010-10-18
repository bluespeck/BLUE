#include "stdafx.h"

#include <tchar.h>

#include "DX11Core.h"
#include "DXCore.h"
#include "DXUtils.h"


namespace BLUE
{

IDXCore * CDX11Core::GetInstance()
{
	if( g_pDXCore )
		return g_pDXCore;
	else
		return ( g_pDXCore = new CDX11Core );		
}

CDX11Core::CDX11Core() : IDXCore()
{	
	m_pDevice					= NULL;
	m_pSwapChain				= NULL;
	m_pDeviceContext			= NULL;
	m_pRenderTargetView			= NULL;
	
	m_bShowEngineInfo			= false;
}

CDX11Core::~CDX11Core( void )
{
	CleanupDevice();
}

void CDX11Core::CleanupDevice()
{
	if( m_pDeviceContext ) 
		m_pDeviceContext->ClearState();

	SAFE_DX_RELEASE( m_pRenderTargetView )
	SAFE_DX_RELEASE( m_pSwapChain )		
	SAFE_DX_RELEASE( m_pDeviceContext ) 		
	SAFE_DX_RELEASE( m_pDevice ) 
}

void CDX11Core::ResetDevice()
{
	CleanupDevice();
	Init( m_hWnd, m_initDesc );	
}

void *CDX11Core::GetDevice()
{
	return m_pDevice;
}

void *CDX11Core::GetSwapChain()
{
	return m_pSwapChain;
}

void *CDX11Core::GetDeviceContext()
{
	return m_pDeviceContext;
}

bool CDX11Core::Init( HWND hWnd )
{
	m_hWnd = hWnd;

	SetDefaultSettings();

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = m_initDesc.bufferCount;
	sd.BufferDesc.Width = m_initDesc.bufferDesc.width;
	sd.BufferDesc.Height = m_initDesc.bufferDesc.height;
	sd.BufferDesc.Format = ( DXGI_FORMAT ) m_initDesc.bufferDesc.format;
	sd.BufferDesc.RefreshRate.Numerator = m_initDesc.bufferDesc.refreshRate.numerator;
	sd.BufferDesc.RefreshRate.Denominator = m_initDesc.bufferDesc.refreshRate.denominator;
	sd.BufferUsage = m_initDesc.bufferUsage;
	sd.OutputWindow = m_initDesc.hOutputWindow;
	sd.SampleDesc.Count = m_initDesc.sampleDesc.count;
	sd.SampleDesc.Quality = m_initDesc.sampleDesc.quality;
	sd.Windowed = m_initDesc.bWindowed;
	sd.Flags = m_initDesc.flags;
	sd.SwapEffect = ( DXGI_SWAP_EFFECT ) m_initDesc.swapEffect;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, &m_featureLevel, &m_pDeviceContext );
	if( FAILED( hr ) )
	{
		m_bInitialized = false;
		return false; 
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
	{
		m_bInitialized = false;
		return false;
	}

	hr = m_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) )
	{
		m_bInitialized = false;
		return false;
	}

	m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );

	InitFont();

	m_bInitialized = true;
	return true;
}

void CDX11Core::InitFont()
{
	// no font support for DX11 yet
}

bool CDX11Core::Init( HWND hWnd, const DXInitDesc &initDesc )
{
	// Save new window handle for future use
	m_hWnd = hWnd;

	// Save new init description for future use
	m_initDesc = initDesc;

	// Use init description in building specific init description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = initDesc.bufferCount;
	sd.BufferDesc.Width = initDesc.bufferDesc.width;
	sd.BufferDesc.Height = initDesc.bufferDesc.height;
	sd.BufferDesc.Format = ( DXGI_FORMAT )initDesc.bufferDesc.format;
	sd.BufferDesc.RefreshRate.Numerator = initDesc.bufferDesc.refreshRate.numerator;
	sd.BufferDesc.RefreshRate.Denominator = initDesc.bufferDesc.refreshRate.denominator;
	sd.BufferUsage = initDesc.bufferUsage;
	sd.OutputWindow = initDesc.hOutputWindow;
	sd.SampleDesc.Count = initDesc.sampleDesc.count;
	sd.SampleDesc.Quality = initDesc.sampleDesc.quality;
	sd.Windowed = initDesc.bWindowed;

	// Create d3d device, swap chain and device immediate context
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext );
	if( FAILED( hr ) )
	{
		m_bInitialized = false;
		return false; 
	}

	InitFont();

	m_bInitialized = true;
	return true;
}

void CDX11Core::BeginDraw()
{
	// Just clear the backbuffer
	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 0.5f }; //red,green,blue,alpha
	m_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, fClearColor );
		
}

void CDX11Core::EndDraw()
{
	m_pSwapChain->Present( 0, 0 );
}

void CDX11Core::Update( float dt )
{
	if( dt == 0 )
		return;

}

void CDX11Core::Render( float dt )
{
	if( dt == 0)
		return;
	
	BeginDraw();
	EndDraw();

	OutputText( m_szEngineInfo, 10, 10, 0xffffffff );
}

void CDX11Core::OutputText( const TCHAR *text, float left, float top, unsigned int color )
{
	RECT rc;
	SetRect( &rc, (int)left, (int)top, 0, 0 );
	
}

void CDX11Core::OnResize(int width, int height)
{

}

}//end namespace