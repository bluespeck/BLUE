#pragma once
#include "WindowInput.h"
#include <windows.h>

namespace BLUE
{

class CAppWindow
{
public:	

	~CAppWindow( void );

	static CAppWindow *GetInstance(){ return &sAppWindow; }	
	BOOL CreateAppWindow( HINSTANCE hInstance, const TCHAR *szTitle, unsigned int nWidth = 1024, unsigned int nHeight = 768 );
	void ShowAppWindow( int nCmdShow = 1 );	

	LRESULT MessageHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	CWindowInput *GetWindowInput();
	HWND GetWindowHandle();	

	void Bind3DEngine(class CEngine *pEngine);

private:
	static CAppWindow sAppWindow;
	HWND m_hWnd;
	CWindowInput m_input;
	class CEngine *m_pEngine;

	CAppWindow( void );	

};

}