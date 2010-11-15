#include "stdafx.h"

#include <tchar.h>

#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"

#include "Scene.h"
#include "EngineUtils.h"



namespace BLUE
{

CScene::CScene(void)
{
	m_pRootObj = new CObject();
	m_pRootObj->SetName(_T("scene_root"));	
}

CScene::~CScene(void)
{
	if(m_pRootObj)
	{
		RecursiveDeleteObjects(m_pRootObj);
	}
}


void CScene::RecursiveDeleteObjects(CObject *pObj)
{
	if(pObj)
	{
		// delete children
		if(pObj->m_pChild)
		{
			CObject *pBrother = pObj->m_pChild;
			while(pBrother)
			{
				CObject *pNextBrother = pBrother->m_pNextBrother;
				RecursiveDeleteObjects(pBrother);
				pBrother = pNextBrother;
			}
		}
		// delete current object
		delete pObj;
		pObj = NULL;
	}
}

CObject *CScene::FindObject( const TCHAR *szName)
{
	if(szName)
		return RecursiveFindObject(m_pRootObj, szName);
	else
		return NULL;
}

CObject *CScene::RecursiveFindObject( CObject *pObj, const TCHAR *szName )
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

CMeshObject *CScene::RecursiveLoadMeshObjectFromFile(const aiScene* pScene, const aiNode* pNode)
{
	CMeshObject *pObj = new CMeshObject();

	memcpy(pObj->m_matLocal.mat, &pNode->mTransformation, sizeof(pObj->m_matLocal));

	pObj->m_pMesh = new CMesh;	
	pObj->m_pMesh->SetHasNormals(false);
	for (UINT i = 0; i < pNode->mNumMeshes; ++i)
	{
		const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		if(pMesh->HasNormals())
			pObj->m_pMesh->SetHasNormals(true);
		pObj->m_pMesh->m_numVertices += pMesh->mNumVertices;
		pObj->m_pMesh->m_numIndices += pMesh->mNumFaces * 3;	// only for triangles
	}

	if(pObj->m_pMesh->m_numVertices)
	{
		// No materials for now
		pObj->m_pMesh->SetHasMaterials(false);
		pObj->m_pMesh->SetNumTexCoordsPerVertex(0);
		pObj->m_pMesh->InitMesh();

		// copy vertex and index data from aimesh
		BYTE *pCurrentMeshVertex	= (BYTE *)pObj->m_pMesh->m_pVertices;
		BYTE *pCurrentMeshIndex		= (BYTE *)pObj->m_pMesh->m_pIndices;
		for (UINT i = 0; i < pNode->mNumMeshes; ++i)
		{
			const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
			UINT bytesForVertices = pMesh->mNumVertices * sizeof(CVector3);		
			memcpy(pCurrentMeshVertex, pMesh->mVertices, bytesForVertices);
			if(pMesh->HasNormals())
			{
				BYTE *pCurrentMeshNormal = (BYTE *)pObj->m_pMesh->m_pNormals + (pCurrentMeshVertex - (BYTE *)pObj->m_pMesh->m_pVertices);
				UINT bytesForNormals = bytesForVertices;
				memcpy(pCurrentMeshNormal, pMesh->mNormals, bytesForNormals);
			}			
			if(pMesh->HasNormals())
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

CObject *CScene::LoadMeshObjectFromFile(const TCHAR *path)
{	
	const aiScene* pScene = NULL;
	aiSetImportPropertyInteger("AI_CONFIG_PP_SBP_REMOVE", aiPrimitiveType_POINT | aiPrimitiveType_LINE );
	UINT importFlag = aiProcess_Triangulate
		| aiProcess_SortByPType
		| aiProcess_ConvertToLeftHanded
		//| aiProcess_FlipWindingOrder
		;
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


bool CScene::LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path)
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
				pNewObject->m_pParent = m_pRootObj;
			else
				pNewObject->m_pParent = FindObject(szParentName);
			if(!m_pRootObj->m_pChild)
				m_pRootObj->m_pChild = pNewObject;
			else
			{
				CObject *pLastBrother = m_pRootObj->m_pChild;
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
					pNewObject->m_pParent = m_pRootObj;
				else
				{
					pNewObject->m_pParent = FindObject(szParentName);
				}
				if(!m_pRootObj->m_pChild)
					m_pRootObj->m_pChild = pNewObject;
				else
				{
					CObject *pLastBrother = m_pRootObj->m_pChild;
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


}	// end namespace BLUE