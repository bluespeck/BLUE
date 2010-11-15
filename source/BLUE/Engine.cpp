#include "stdafx.h"

#include "Engine.h"

#ifdef BLUE_DX11
	#include "DX11Core.h"
#elif defined(BLUE_DX10)
	#include "DX10Core.h"
#endif

#include "Object.h"
#include "MeshObject.h"
#include "PointObject.h"

namespace BLUE
{
// The only instance of a CEngine class
CEngine *CEngine::g_pEngine = NULL;

CEngine * CEngine::GetInstance()
{
	if( g_pEngine )
		return g_pEngine;
	else
		return ( g_pEngine = new CEngine );		
}

void CEngine::DestroyInstace()
{
	if( g_pEngine )
	{
		delete g_pEngine;
		g_pEngine = NULL;
	}
}


CEngine::CEngine(void)
{
#ifdef BLUE_DX10
	pCore = CDX10Core::GetInstance();
#elif defined(BLUE_DX11)
	pCore = CDX11Core::GetInstance();
#endif

	m_bMinimized = false;
	m_timer.Reset();

	m_pScene = new CScene();

	_tcscpy_s(m_szEngineInfo, _T("FPS: 0"));
}

CEngine::~CEngine(void)
{
	pCore->DestroyInstace();
	delete m_pScene;
}

bool CEngine::Init(HWND hwnd)
{
	return pCore->Init(hwnd);
}

void CEngine::Update(float dt)
{
	ComputeFPS( dt );
	RecursiveUpdate(m_pScene->GetRootObject(), dt);
}

void CEngine::RecursiveUpdate(CObject *pObject, float dt)
{
	for(UINT i = 0, s = pObject->m_pControllers.size(); i < s; ++i)
		pObject->m_pControllers[i]->Update(dt);
	// recursively update all children
	CObject *pObj = pObject->m_pChild;
	while(pObj)
	{
		RecursiveUpdate(pObj, dt);
		pObj = pObj->m_pNextBrother;
	}
}

void CEngine::RecursiveRender(CObject *pObject, float dt)
{
	pCore->Render( pObject, dt );
	// recursively update all children
	CObject *pObj = pObject->m_pChild;
	while(pObj)
	{
		RecursiveRender(pObj, dt);
		pObj = pObj->m_pNextBrother;
	}
}


void CEngine::Render(float dt)
{
	pCore->BeginDraw();
	
	pCore->SetWireframe(false);
	pCore->ApplyRasterizerState();

	RecursiveRender(m_pScene->GetRootObject(), dt);
	
	pCore->SetWireframe(false);
	pCore->ApplyRasterizerState();

	OutputText( m_szEngineInfo, 5, 5, 0xff000000 );
	
	pCore->EndDraw();
}

void CEngine::OutputText( const TCHAR *text, float left, float top, DWORD color )
{
	pCore->OutputText(text, left, top, color);
	
}

void CEngine::ComputeFPS( float dt )
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

void CEngine::Run()
{
	m_timer.Tick();
	float dt = m_timer.GetDeltaTime();
	
	Update(dt);
	if(!m_bMinimized)
		Render(dt);
}
void CEngine::Pause()
{
	m_timer.Pause();
}

void CEngine::Unpause()
{
	m_timer.Unpause();
}

bool CEngine::IsPaused()
{
	return m_timer.IsPaused();
}

CObject *CEngine::FindObject( const TCHAR *szName )
{
	if( szName == NULL || !*szName )
		return NULL;

	return m_pScene->FindObject( szName );
}

void CEngine::OnResize(int width, int height)
{
	pCore->OnResize(width, height);
}

void CEngine::SetMinimized(bool bMinimized)
{
	pCore->SetMinimized(bMinimized);
	m_bMinimized = bMinimized;
}

bool CEngine::LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path)
{
	return m_pScene->LoadObjectFromFile(szName, szParentName, objType, path);
}

}// end namespace BLUE
