#include "stdafx.h"

#include "Engine.h"

#ifdef BLUE_DX11
	#include "DX11Core.h"
#elif defined(BLUE_DX10)
	#include "DX10Core.h"
#endif

namespace BLUE
{

CEngine::CEngine(void)
{
#ifdef BLUE_DX10
	pCore = CDX10Core::GetInstance();
#elif defined(BLUE_DX11)
	pCore = CDX11Core::GetInstance();
#endif

	pSceneRoot = new CObject();
	pSceneRoot->SetName(L"scene_root");	
}

CEngine::~CEngine(void)
{
	pCore->DestroyInstace();
}

CObject *CEngine::FindObject( CObject *pObj, const TCHAR *szName )
{
	if(pObj == NULL)
		return NULL;
	
	if( 0 == _tcscmp( pObj->GetName(), szName ) )
	{
		return pObj;
	}
	else
	{
		CObject *pFound = NULL;
		for( CObject *p = pObj->m_pChild; p != NULL; p = p->m_pNextBrother )
		{
			pFound = FindObject( p, szName );
			if( pFound != NULL )
				return pFound;
		}
		return NULL;
	}
}

CObject *CEngine::FindObject( const TCHAR *szName )
{
	if( pSceneRoot->GetName() == NULL )
		return NULL;

	return FindObject( pSceneRoot, szName );
}

bool CEngine::LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectTypes objType, TCHAR *path)
{
	CObject *pNewObject = NULL;
	switch(objType)
	{
	case OT_BASIC:
		// ignore path for basic objects for now; also ignore objects with no name
		if(szName && *szName)
		{
			pNewObject = new CObject();
			pNewObject->SetName(szName);
			if(!szParentName || !*szParentName)
			{
				pNewObject->m_pParent = pSceneRoot;
			}
			else
			{				
				pNewObject->m_pParent = FindObject(szParentName);
			}
			return true;
		}		
		break;
	case OT_MESH:
		// load a mesh object
		{
			return true;
		}
		break;
	case OT_POINT:
		// load a point based object
		{
			return true;
		}
		break;
	case OT_VOLUMETRIC:
		// load a volumetric object
		{
			return true;
		}
		break;
	}

	return false;
}

}// end namespace BLUE
