#include "stdafx.h"
#include <tchar.h>
#include <cstring>

#include "AppWindow.h"
#include "DXCore.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

namespace BLUE
{

CAppWindow CAppWindow::sAppWindow;

LRESULT CAppWindow::MessageHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static int currWidth = 0, currHeight = 0;
	static bool bResizing = false;

	switch( message )
	{
	case WM_KEYDOWN:
		{
			m_input.SetKeyDown( ( unsigned int )wParam );
			break;
		}
	case WM_KEYUP:
		{			
			m_input.SetKeyUp( ( unsigned int )wParam );
			break;
		}
	case WM_SIZE:
		{
			currWidth = LOWORD( lParam );
			currHeight = HIWORD( lParam );
			bResizing = true;
			if( m_pDXCore )
				if( wParam == SIZE_MINIMIZED )
					m_pDXCore->SetMinimized( true );
				else if( wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED )
				{
					m_pDXCore->SetMinimized( false );
					m_pDXCore->OnResize( LOWORD( lParam ), HIWORD( lParam ) );
				}
			
			break;
		}
	case WM_EXITSIZEMOVE:
		{
			if( m_pDXCore && bResizing )
				m_pDXCore->OnResize( currWidth, currHeight );
			break;
		}
	default:
		{
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
	}

	return 0;	
}

CAppWindow::CAppWindow( void )
{
}

CAppWindow::~CAppWindow( void )
{
}

BOOL CAppWindow::CreateAppWindow( HINSTANCE hInstance, const TCHAR *szTitle, unsigned int nWidth , unsigned int nHeight )
{
	static TCHAR szWindowClass[] = _T( "AppWindow" );
	
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon( 0, IDI_APPLICATION );
	wc.hCursor       = LoadCursor( 0, MAKEINTRESOURCE( IDC_ARROW ) );
	wc.hbrBackground = ( HBRUSH ) GetStockObject( NULL_BRUSH );
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	if ( !RegisterClass( &wc ) )
    {
        MessageBox( NULL,
            _T( "Call to RegisterClass failed!" ),
            szTitle,
            NULL );

        return FALSE;
    }

	static TCHAR szWindowTitle[64];
	_tcscpy_s( szWindowTitle, 64, szTitle );
	
	m_hWnd = CreateWindow( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
						nWidth, nHeight, GetDesktopWindow(), NULL, hInstance, NULL );
	if( !m_hWnd )
	{
		TCHAR errorStr[256];
		_stprintf_s(errorStr, _T("Call to CreateWindow failed: %d"), GetLastError());
		MessageBox( NULL,
			errorStr,
			szTitle,
			NULL );

		return FALSE;
	}

	return TRUE;
}

void CAppWindow::ShowAppWindow( int nCmdShow )
{
	ShowWindow( m_hWnd, nCmdShow );
	UpdateWindow( m_hWnd );
}

CWindowInput *CAppWindow::GetWindowInput()
{
	return &m_input;
}

HWND CAppWindow::GetWindowHandle()
{
	return m_hWnd;
}

void CAppWindow::BindDXCore(class IDXCore *pDXCore)
{
	m_pDXCore = pDXCore;
}


}// end namespace BLUE

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}

		// Check if the window is being closed.
	case WM_CLOSE:
		{
			PostQuitMessage( 0 );		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
	default:
		{
			return BLUE::CAppWindow::GetInstance()->MessageHandler( hWnd, message, wParam, lParam );
		}
	}

	return 0;
}
