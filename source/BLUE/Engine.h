#pragma once

#include <tchar.h>

#if defined(BLUE_DX11) || defined(BLUE_DX10)
	#define BLUE_DIRECTX
	#include "DXCore.h"
#endif

#include "Object.h"
#include "MeshObject.h"
#include "Timer.h"


namespace BLUE
{

class CEngine
{
public:
	enum ObjectTypes
	{
		OT_BASIC,
		OT_MESH,
		OT_POINT,
		OT_VOLUMETRIC
	};
	static CEngine *GetInstance();
	static void		DestroyInstace();

	bool			Init( HWND hWnd );

	CObject		*	FindObject( const TCHAR *szName );
	bool			LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectTypes objType, const TCHAR *path);

	void			Update();	// this should be ran in the main loop; it calls engine's Update(dt) using the internal timer	
	void			Pause();
	void			Unpause();
	bool			IsPaused();

	// window message handlers
	void			OnResize( int width, int height );

	void			SetMinimized( bool bMinimized );	
	
protected:
	CObject		*	FindObject( CObject *pObj, const TCHAR *szName );
	CObject		*	LoadMeshObjectFromFile(const TCHAR *path);
	CMeshObject *	RecursiveLoadMeshObjectFromFile(const class aiScene* pScene, const class aiNode* pNode);

	CEngine(void);
	virtual ~CEngine(void);

protected:

	static CEngine *g_pEngine;

#ifdef BLUE_DIRECTX
	IDXCore *pCore;
#endif

	CTimer					m_timer;
	
	CObject	*pSceneRoot;

	bool					m_bMinimized;	

};

}// end namespace BLUE