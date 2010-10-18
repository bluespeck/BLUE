#pragma once

#include <windows.h>

#include "dxsdkver.h"
#if ( _DXSDK_PRODUCT_MAJOR < 9 || _DXSDK_BUILD_MAJOR < 1949 )
#error "The installed DXSDK is out of date!"
#endif

#include "DXUtils.h"
#include "Timer.h"

namespace BLUE
{

// Entry point for Direct X operations
class IDXCore
{
public:	
	static void DestroyInstace();		

	virtual void ResetDevice( ) = 0;
	virtual void CleanupDevice() = 0;

	virtual void *GetDevice(){ return NULL; }
	// only used for DX10 or higher
	virtual void *GetSwapChain(){ return NULL; }
	// only used for DX11 or higher
	virtual void *GetDeviceContext(){ return NULL; }
	
// [[ Initialization
	// Initialize Direct X using default settings
	virtual bool Init( HWND hWnd );	
	virtual bool Init( HWND hWnd, const DXInitDesc &initDesc );
	virtual bool UpdateConfiguration( const DXInitDesc &initDesc );
	
	void SetInitBufferSize( unsigned int width, unsigned int height );

	void SetDefaultSettings();
// ]]

	void Update();	// this should be ran in the main loop; it calls engine's Update(dt) using the internal timer	
	void Pause();
	void Unpause();
	bool IsPaused();

	// window message handlers
	virtual void OnResize( int width, int height ) = 0;

	void SetMinimized( bool bMinimized );
protected:
	virtual void BeginDraw() = 0;
	virtual void EndDraw() = 0;	
	
	virtual void Update( float dt ) = 0;
	virtual void Render( float dt ) = 0;
	IDXCore();
	virtual ~IDXCore( void );

	void ComputeFPS( float dt );

protected:
	// single instance of DXCore
	static IDXCore			*g_pDXCore;

	DXInitDesc				m_initDesc;
	HWND					m_hWnd;

	bool					m_bInitialized;
	
	CTimer					m_timer;

	TCHAR					m_szEngineInfo[MAX_STRING_LENGTH];

	bool					m_bMinimized;



};

} // end namespace