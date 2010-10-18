#include <tchar.h>
#include <assert.h>

#include "../blue/AppWindow.h"
#include "../blue/DX10Core.h"
#include "../blue/DX11Core.h"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// Initialize windowed application
	BLUE::CAppWindow *pAppWindow = BLUE::CAppWindow::GetInstance();
	pAppWindow->CreateAppWindow( hInstance, _T( "BLUE Test" ), 1024, 768 );
	pAppWindow->ShowAppWindow( nCmdShow );	

	// Initialize DX core
	BLUE::IDXCore *pDXCore = BLUE::CDX10Core::GetInstance();
	if( !pDXCore->Init( pAppWindow->GetWindowHandle() ) )
	{
		assert(0 && "Could not initialize DirectX 10");
		exit(1);
	}
	
	pAppWindow->BindDXCore( pDXCore );

	MSG msg = { 0 };
	while( msg.message != WM_QUIT )
	{
		// If there are Window messages then process them.
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do engine stuff.
		else
		{	
			pDXCore->Update();			
		}
	}

	// Delete DX core
	BLUE::CDX10Core::DestroyInstace();

	return msg.wParam;
	
}