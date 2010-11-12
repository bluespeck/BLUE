#include "stdafx.h"

#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"

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
	m_pSceneRoot = new CObject();
	m_pSceneRoot->SetName(_T("scene_root"));	

	_tcscpy_s(m_szEngineInfo, _T("FPS: 0"));
}

CEngine::~CEngine(void)
{
	pCore->DestroyInstace();
}

bool CEngine::Init(HWND hwnd)
{
	return pCore->Init(hwnd);
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
	//for(UINT i = 0, s = pObject->m_pControllers.size(); i < s; ++i)
	//	pObject->m_pControllers[i]->Update(dt);
	pCore->Render( pObject, dt );
	// recursively update all children
	CObject *pObj = pObject->m_pChild;
	while(pObj)
	{
		RecursiveRender(pObj, dt);
		pObj = pObj->m_pNextBrother;
	}
}

void CEngine::Update(float dt)
{
	ComputeFPS( dt );
	RecursiveUpdate(m_pSceneRoot, dt);
}

void CEngine::Render(float dt)
{
	pCore->BeginDraw();
	
	pCore->SetWireframe(true);
	pCore->ApplyRasterizerState();

	RecursiveRender(m_pSceneRoot, dt );	
	
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

CObject *CEngine::RecursiveFindObject( CObject *pObj, const TCHAR *szName )
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
			pFound = RecursiveFindObject( p, szName );
			if( pFound != NULL )
				return pFound;
		}
		return NULL;
	}
}

CObject *CEngine::FindObject( const TCHAR *szName )
{
	if( szName == NULL || !*szName || m_pSceneRoot->GetName() == NULL )
		return NULL;

	return RecursiveFindObject( m_pSceneRoot, szName );
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

CMeshObject *CEngine::RecursiveLoadMeshObjectFromFile(const aiScene* pScene, const aiNode* pNode)
{
	CMeshObject *pObj = new CMeshObject();

	memcpy(pObj->m_matLocal.mat, &pNode->mTransformation, sizeof(pObj->m_matLocal));

	pObj->m_pMesh = new CMesh;
	for (UINT i = 0; i < pNode->mNumMeshes; ++i)
	{
		const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		pObj->m_pMesh->m_numVertices += pMesh->mNumVertices;
		pObj->m_pMesh->m_numIndices += pMesh->mNumFaces * 3;	// only for triangles
	}

	if(pObj->m_pMesh->m_numVertices)
	{
		// No materials for now
		pObj->m_pMesh->AllocSpace(pObj->m_pMesh->m_numVertices, pObj->m_pMesh->m_numIndices, 0);

		// copy vertex and index data from aimesh
		BYTE *pCurrentMeshVertex	= (BYTE *)pObj->m_pMesh->m_pVertices;
		BYTE *pCurrentMeshIndex		= (BYTE *)pObj->m_pMesh->m_pIndices;
		for (UINT i = 0; i < pNode->mNumMeshes; ++i)
		{
			const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
			UINT bytesForVertices = pMesh->mNumVertices * sizeof(CVector3);		
			memcpy(pCurrentMeshVertex, pMesh->mVertices, bytesForVertices);
			pCurrentMeshVertex += bytesForVertices;
			for(UINT faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex)
			{
				UINT bytesForIndices = pMesh->mFaces[faceIndex].mNumIndices * sizeof(UINT);
				memcpy(pCurrentMeshIndex, pMesh->mFaces[faceIndex].mIndices, bytesForIndices);
				pCurrentMeshIndex += bytesForIndices;
			}
		}
	}

	if(pNode->mNumChildren > 0)
	{
		pObj->m_pChild = RecursiveLoadMeshObjectFromFile(pScene, pNode->mChildren[0]);
		CObject *pLastBrother = pObj->m_pChild;
		pLastBrother->m_pParent = pObj;
		for (UINT i = 1; i < pNode->mNumChildren; ++i)
		{			
			pLastBrother->m_pNextBrother = RecursiveLoadMeshObjectFromFile(pScene, pNode->mChildren[i]);
			pLastBrother = pLastBrother->m_pNextBrother;
			pLastBrother->m_pParent = pObj;
		}
	}
	return pObj;
}

CObject *CEngine::LoadMeshObjectFromFile(const TCHAR *path)
{	
	const aiScene* pScene = NULL;
	aiSetImportPropertyInteger("AI_CONFIG_PP_SBP_REMOVE", aiPrimitiveType_POINT | aiPrimitiveType_LINE );
	UINT importFlag = aiProcess_ConvertToLeftHanded 
		| aiProcess_Triangulate
		| aiProcess_SortByPType;
#ifdef UNICODE
	char szPath[MAX_PATH]={0};
	wcstombs(szPath, path, MAX_PATH);
	szPath[MAX_PATH - 1] = 0;

	pScene = aiImportFile(szPath, importFlag);
#else
	pScene = aiImportFile(path, importFlag);
#endif
	
	if(pScene)
	{
		CMeshObject *pNewObject = RecursiveLoadMeshObjectFromFile(pScene, pScene->mRootNode);
		aiReleaseImport(pScene);
		return pNewObject;
	}
	else
	{
		aiReleaseImport(pScene);
		return NULL;
	}
}

bool CEngine::LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path)
{
	CObject *pNewObject = NULL;
	switch(objType)
	{
	case OT_BASIC:
		// ignore path for basic objects for now; also ignore objects with no name
		{
			pNewObject = new CObject();
			if(szName && *szName)
				pNewObject->SetName(szName);
			if(!szParentName || !*szParentName)
				pNewObject->m_pParent = m_pSceneRoot;
			else
				pNewObject->m_pParent = FindObject(szParentName);
			if(!m_pSceneRoot->m_pChild)
				m_pSceneRoot->m_pChild = pNewObject;
			else
			{
				CObject *pLastBrother = m_pSceneRoot->m_pChild;
				while(pLastBrother->m_pNextBrother)
					pLastBrother = pLastBrother->m_pNextBrother;
				pLastBrother->m_pNextBrother = pNewObject;
			}
			return true;
		}		
		break;
	case OT_MESH:
		// load a mesh object
		{
			CObject *pNewObject = LoadMeshObjectFromFile(path);
			if(pNewObject)
			{
				if(!szParentName || !*szParentName)
					pNewObject->m_pParent = m_pSceneRoot;
				else
				{
					pNewObject->m_pParent = FindObject(szParentName);
				}
				if(!m_pSceneRoot->m_pChild)
					m_pSceneRoot->m_pChild = pNewObject;
				else
				{
					CObject *pLastBrother = m_pSceneRoot->m_pChild;
					while(pLastBrother->m_pNextBrother)
						pLastBrother = pLastBrother->m_pNextBrother;
					pLastBrother->m_pNextBrother = pNewObject;
				}
			}
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
