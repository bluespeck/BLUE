#include <tchar.h>
#include <assert.h>

#define BLUE_DX10

#include "../blue/AppWindow.h"
#include "../blue/Engine.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// Initialize windowed application
	BLUE::CAppWindow *pAppWindow = BLUE::CAppWindow::GetInstance();
	pAppWindow->CreateAppWindow( hInstance, _T( "BLUE Test" ), 1024, 768 );
	pAppWindow->ShowAppWindow( nCmdShow );	

	// Initialize DX core
	BLUE::CEngine *pEngine = BLUE::CEngine::GetInstance();
	if( !pEngine->Init( pAppWindow->GetWindowHandle() ) )
	{
		assert(0 && "Could not initialize BLUE");
		exit(1);
	}
	
	pAppWindow->Bind3DEngine( pEngine );
	pEngine->LoadObjectFromFile(L"myObj1", NULL, BLUE::OT_MESH, L"D:\\test\\models\\3ds\\jeep1.3ds");
	//for(int i = 0; i < 10; i++)
	{
		pEngine->LoadObjectFromFile(L"myObj2", NULL, BLUE::OT_MESH, L"D:\\test\\models\\3ds\\pyramob.3ds");
	}

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
			pEngine->Run();			
		}
	}

	// Delete DX core
	BLUE::CEngine::DestroyInstace();
#ifdef DEBUG
	_CrtMemDumpAllObjectsSince(NULL);
#endif
	return msg.wParam;

	
}