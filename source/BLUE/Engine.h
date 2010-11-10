#pragma once

#include <tchar.h>

#if defined(BLUE_DX11) || defined(BLUE_DX10)
	#define BLUE_DIRECTX
	#include "DXCore.h"
#endif

#include "Object.h"
#include "MeshObject.h"
#include "Timer.h"
#include "EngineUtils.h"


namespace BLUE
{

class CEngine
{
public:
	static CEngine *GetInstance();
	static void		DestroyInstace();

	bool			Init( HWND hWnd );

	CObject		*	FindObject( const TCHAR *szName );
	bool			LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path);

	void			Run();	// this should be ran in the main loop; it calls engine's Update(dt) using the internal timer	
	void			Pause();
	void			Unpause();
	bool			IsPaused();

	// window message handlers
	void			OnResize(int width, int height);

	void			SetMinimized(bool bMinimized);	

	void			OutputText( const TCHAR *text, float left, float top, DWORD color );
	
protected:
	CObject		*	LoadMeshObjectFromFile(const TCHAR *path);
	CObject		*	RecursiveFindObject(CObject *pObj, const TCHAR *szName);
	CMeshObject *	RecursiveLoadMeshObjectFromFile(const struct aiScene* pScene, const struct aiNode* pNode);

	void			RecursiveUpdate(CObject *pObj, float dt);
	void			RecursiveRender(CObject *pObj, float dt);

	void			Render(float dt);
	void			Update(float dt);

	void			ComputeFPS(float dt);

					CEngine(void);
					virtual ~CEngine(void);

protected:

	static CEngine *g_pEngine;

#ifdef BLUE_DIRECTX
	IDXCore				*	pCore;
#endif

	CTimer					m_timer;
	
	CObject				*	m_pSceneRoot;


	TCHAR					m_szEngineInfo[MAX_STRING_LENGTH];
	bool					m_bMinimized;	

};

}// end namespace BLUE