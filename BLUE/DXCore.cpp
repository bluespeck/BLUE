#include "stdafx.h"
#include <tchar.h>

#include "DXCore.h"
#include "DXUtils.h"

namespace BLUE
{

#define DEFAULT_DESC_BUFFERCOUNT						1
#define DEFAULT_DESC_BUFFERDESC_FORMAT					28		//DXGI_FORMAT_R8G8B8A8_UNORM
#define DEFAULT_DESC_BUFFERDESC_WIDTH					1024
#define DEFAULT_DESC_BUFFERDESC_HEIGHT					768
#define DEFAULT_DESC_BUFFERDESC_REFRESHRATE_NUMERATOR	60
#define DEFAULT_DESC_BUFFERDESC_REFRESHRATE_DENOMINATOR 1
#define DEFAULT_DESC_BUFFERDESC_SCANLINEORDERING		0		//DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED
#define DEFAULT_DESC_BUFFERDESC_SCALING					0		//DXGI_MODE_SCALING_UNSPECIFIED
#define DEFAULT_DESC_BUFFERUSAGE						32		//DXGI_USAGE_RENDER_TARGET_OUTPUT
#define DEFAULT_DESC_FLAGS								0		//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
#define DEFAULT_DESC_SAMPLEDESC_COUNT					1
#define DEFAULT_DESC_SAMPLEDESC_QUALITY					0
#define DEFAULT_DESC_SWAP_EFFECT						0		//DXGI_SWAP_EFFECT_DISCARD
#define DEFAULT_DESC_WINDOWED							true

// The only instance of a DXCore class (or derived)
IDXCore *IDXCore::g_pDXCore = NULL;

void IDXCore::DestroyInstace()
{
	if( g_pDXCore )
	{
		delete g_pDXCore;
		g_pDXCore = NULL;
	}
}


IDXCore::IDXCore()
{	
	m_timer.Reset();
	m_bInitialized = false;
	
	_tcscpy_s(m_szEngineInfo, _T( "FPS: 0" ) );
	m_bMinimized = false;
}

IDXCore::~IDXCore( void )
{
}

void IDXCore::SetDefaultSettings()
{
	memset( &m_initDesc, 0, sizeof( m_initDesc ) );
	m_initDesc.bufferCount = DEFAULT_DESC_BUFFERCOUNT;
	m_initDesc.bufferDesc.width = DEFAULT_DESC_BUFFERDESC_WIDTH;
	m_initDesc.bufferDesc.height = DEFAULT_DESC_BUFFERDESC_HEIGHT;
	m_initDesc.bufferDesc.format = DEFAULT_DESC_BUFFERDESC_FORMAT;
	m_initDesc.bufferDesc.refreshRate.numerator = DEFAULT_DESC_BUFFERDESC_REFRESHRATE_NUMERATOR;
	m_initDesc.bufferDesc.refreshRate.denominator = DEFAULT_DESC_BUFFERDESC_REFRESHRATE_DENOMINATOR;
	m_initDesc.bufferDesc.scaling = DEFAULT_DESC_BUFFERDESC_SCALING;
	m_initDesc.bufferDesc.scanlineOrdering = DEFAULT_DESC_BUFFERDESC_SCANLINEORDERING;
	m_initDesc.bufferUsage = DEFAULT_DESC_BUFFERUSAGE;
	m_initDesc.hOutputWindow = m_hWnd;
	m_initDesc.sampleDesc.count = DEFAULT_DESC_SAMPLEDESC_COUNT;
	m_initDesc.sampleDesc.quality = DEFAULT_DESC_SAMPLEDESC_QUALITY;
	m_initDesc.bWindowed = DEFAULT_DESC_WINDOWED;
	m_initDesc.flags = DEFAULT_DESC_FLAGS;
	m_initDesc.swapEffect = DEFAULT_DESC_SWAP_EFFECT;

}

void IDXCore::SetInitBufferSize( unsigned int width, unsigned int height )
{
	m_initDesc.bufferDesc.width = width;
	m_initDesc.bufferDesc.height = height;

}

bool IDXCore::Init( HWND hWnd )
{
	m_hWnd = hWnd;

	// Needs dx version to actually initialize
	m_bInitialized = false;
	return false;
}	

bool IDXCore::Init( HWND hWnd, const DXInitDesc &initDesc )
{ 
	m_hWnd = hWnd;

	// Needs dx version to actually initialize
	m_bInitialized = false;
	return false; 
}

bool IDXCore::UpdateConfiguration( const DXInitDesc &initDesc )
{ 
	return false; 
}

void IDXCore::Update()
{
	m_timer.Tick();
	float dt = m_timer.GetDeltaTime();
	
	ComputeFPS( dt );

	Update( dt );
	if( !m_bMinimized )
		Render( dt );
}

void IDXCore::Pause()
{
	m_timer.Pause();
}

void IDXCore::Unpause()
{
	m_timer.Unpause();
}

bool IDXCore::IsPaused()
{
	return m_timer.IsPaused();
}

void IDXCore::ComputeFPS( float dt )
{
	static int frameCount = 0;
	static float fTimeAcc= 0.0f;

	frameCount++;

	// Retrieve FPS (once per second)
	if( fTimeAcc >= 1.0f )
	{			
		_stprintf_s( m_szEngineInfo, _T( "FPS: %d" ), frameCount);		

		frameCount = 0;
		fTimeAcc = 0.0f;		
	}
	else
	{
		fTimeAcc += dt;
	}
}

void IDXCore::SetMinimized( bool bMinimized )
{
	m_bMinimized = bMinimized;
}

}	// end namespace
