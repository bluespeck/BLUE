#pragma once

#include <tchar.h>

#if defined(BLUE_DX11) || defined(BLUE_DX10)
	#define BLUE_DIRECTX
	#include "DXCore.h"
#endif

#include "Object.h"


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


	CEngine(void);
	virtual ~CEngine(void);

	CObject *FindObject( const TCHAR *szName );
	bool LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectTypes objType, TCHAR *path);
	
	
protected:
	CObject *FindObject( CObject *pObj, const TCHAR *szName );

#ifdef BLUE_DIRECTX
	IDXCore *pCore;
#endif

	CObject	*pSceneRoot;
};

}// end namespace BLUE