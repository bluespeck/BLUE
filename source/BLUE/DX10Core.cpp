#include "stdafx.h"

#include <tchar.h>

#include "DX10Core.h"
#include "DXCore.h"
#include "DXUtils.h"
#include <DXErr.h>

namespace BLUE
{

	const D3D10_DEPTH_STENCIL_DESC defaultDepthStencilStateDesc = 
	{
		true,								// DepthEnable					
		D3D10_DEPTH_WRITE_MASK_ALL,			// DepthWriteMask				
		D3D10_COMPARISON_LESS_EQUAL,		// DepthFunc						
											
		false,								// StencilEnable				
		0xff,								// StencilReadMask				
		0xff,								// StencilWriteMask				
		{
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilFailOp		
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilDepthFailOp			
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilPassOp	
			D3D10_COMPARISON_LESS_EQUAL		// FrontFace.StencilFunc	
		},
		{	
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilFailOp		
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilDepthFailOp		
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilPassOp	
			D3D10_COMPARISON_LESS_EQUAL		// BackFace.StencilFunc			
		}
	};

IDXCore * CDX10Core::GetInstance()
{
	if( g_pDXCore )
		return g_pDXCore;
	else
		return ( g_pDXCore = new CDX10Core );		
}

CDX10Core::CDX10Core()
: IDXCore()
, m_pFont( NULL )
, m_bShowEngineInfo( false )
{	
	m_pDevice					= NULL;
	m_pSwapChain				= NULL;	
	m_pRenderTargetView			= NULL;	
	m_pDepthStencilView			= NULL;	
}

CDX10Core::~CDX10Core( void )
{
	CleanupDevice();
}

void CDX10Core::CleanupDevice()
{

	SAFE_DX_RELEASE( m_pRenderTargetView )
	SAFE_DX_RELEASE( m_pDepthStencilView );	

	SAFE_DX_RELEASE( m_pFont );

	SAFE_DX_RELEASE( m_pSwapChain )			
	SAFE_DX_RELEASE( m_pDevice ) 
}

void CDX10Core::ResetDevice()
{
	CleanupDevice();
	Init( m_hWnd, m_initDesc );	
}

void *CDX10Core::GetDevice()
{
	return m_pDevice;
}

void *CDX10Core::GetSwapChain()
{
	return m_pSwapChain;
}

bool CDX10Core::Init( HWND hWnd )
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
	sd.BufferDesc.ScanlineOrdering			= ( DXGI_MODE_SCANLINE_ORDER ) m_initDesc.bufferDesc.scanlineOrdering;
	sd.BufferDesc.Scaling					= ( DXGI_MODE_SCALING )m_initDesc.bufferDesc.scaling;
	sd.BufferUsage = m_initDesc.bufferUsage;
	sd.OutputWindow = m_initDesc.hOutputWindow;
	sd.SampleDesc.Count = m_initDesc.sampleDesc.count;
	sd.SampleDesc.Quality = m_initDesc.sampleDesc.quality;
	sd.Windowed = m_initDesc.bWindowed;
	sd.Flags = m_initDesc.flags;
	sd.SwapEffect = ( DXGI_SWAP_EFFECT ) m_initDesc.swapEffect;
		
	UINT createDeviceFlags	= 0;
//#if defined(DEBUG) || defined(_DEBUG)  
//	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
//#endif

	HR( D3D10CreateDeviceAndSwapChain1( 
		NULL, 
		D3D10_DRIVER_TYPE_HARDWARE, 
		NULL, 
		createDeviceFlags, 
		D3D10_FEATURE_LEVEL_10_1, 
		D3D10_1_SDK_VERSION, 
		&sd, 
		&m_pSwapChain, 
		&m_pDevice ) );

	InitRenderAndDepthStencilTargets( sd.BufferDesc.Width, sd.BufferDesc.Height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = sd.BufferDesc.Width;
	vp.Height   = sd.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );
	
	InitDepthStencilState();

	InitFont();

	m_bInitialized = true;
	return true;
}

bool CDX10Core::Init( HWND hWnd, const DXInitDesc &initDesc )
{
	m_bInitialized = false;
	// Save new window handle for future use
	m_hWnd = hWnd;

	// Save new init description for future use
	m_initDesc = initDesc;

	// Use init description in building specific init description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
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

	// Create d3d device, swap chain
	
	HR( D3D10CreateDeviceAndSwapChain1( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0,
		D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice ) );
	
	InitRenderAndDepthStencilTargets( sd.BufferDesc.Width, sd.BufferDesc.Height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = sd.BufferDesc.Width;
	vp.Height   = sd.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );

	
	InitDepthStencilState();

	InitFont();

	m_bInitialized = true;
	return true;
}

void CDX10Core::InitRenderAndDepthStencilTargets(int width, int height)
{
	ID3D10Texture2D* pBuffer = NULL;
	HR( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBuffer ) );
	
	// Create a render target view
	HR( m_pDevice->CreateRenderTargetView( pBuffer, NULL, &m_pRenderTargetView ) );

	pBuffer->Release();	
	
	// Create DepthStencil target view
	D3D10_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
		
	HR( m_pDevice->CreateTexture2D( &depthStencilDesc, 0, &pBuffer ) );	
	HR( m_pDevice->CreateDepthStencilView( pBuffer, 0, &m_pDepthStencilView ) );
	
	pBuffer->Release();
	
	m_pDevice->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

}

void CDX10Core::InitDepthStencilState()
{
	D3D10_DEPTH_STENCIL_DESC depthStencilStateDesc = defaultDepthStencilStateDesc;
	HR( m_pDevice->CreateDepthStencilState( &depthStencilStateDesc, &m_pDepthStencilState ) );
	m_pDevice->OMSetDepthStencilState( m_pDepthStencilState, 0 );

}

void CDX10Core::InitRasterizerState()
{	
}

void CDX10Core::BeginDraw()
{
	// Just clear the backbuffer
	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 0.5f }; //red,green,blue,alpha
	m_pDevice->ClearRenderTargetView( m_pRenderTargetView, D3DXCOLOR(0xffffffff) );			
	m_pDevice->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );
}

void CDX10Core::EndDraw()
{
	HR( m_pSwapChain->Present( 1, 0 ) );
}

void CDX10Core::Update( float dt )
{
	ComputeFPS( dt );

	if( dt == 0 )
		return;	
}

struct CColorVertex
{
	D3DXVECTOR3 position;	
	D3DXCOLOR color;
	bool operator == (const CColorVertex &vertex)
	{
		return position == vertex.position && color == vertex.color;
	}	
};

void CDX10Core::Render( float dt )
{
	if( dt == 0 )
		return;
	
	BeginDraw();
	
	OutputEngineInfo();
	EndDraw();
}

void CDX10Core::OutputEngineInfo()
{
	OutputText( m_szEngineInfo, 5, 5, 0xff000000 );
}

void CDX10Core::OnResize(int width, int height)
{		
	SAFE_DX_RELEASE( m_pRenderTargetView);
	SAFE_DX_RELEASE( m_pDepthStencilView);	
	
	// Resize the swap chain and recreate the render target view.

	HR(m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		
	InitRenderAndDepthStencilTargets( width, height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = width;
	vp.Height   = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );

	
	InitDepthStencilState();

	InitFont();

}

void CDX10Core::InitFont()
{
	// Create font
	if( m_pFont )
		SAFE_DX_RELEASE( m_pFont );

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 25;
	fontDesc.Width           = 12;
	fontDesc.Weight          = 0;
	fontDesc.MipLevels       = 1;
	fontDesc.Italic          = false;
	fontDesc.CharSet         = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality         = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s( fontDesc.FaceName, 32, _T( "Calibri" ) );

	HR( D3DX10CreateFontIndirect( m_pDevice, &fontDesc, &m_pFont ) );
}

void CDX10Core::OutputText( const TCHAR *text, float left, float top, unsigned int color )
{	
	RECT rc ={ ( int )left, ( int )top, 0, 0 };
	
	m_pFont->DrawText( 0, text, -1, &rc, DT_NOCLIP, D3DXCOLOR( color ) );	
}

}//end namespace
