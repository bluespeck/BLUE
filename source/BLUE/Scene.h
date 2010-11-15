#pragma once

#include "Object.h"
#include "MeshObject.h"
#include "Material.h"

namespace BLUE
{

class CScene
{
public:
	CScene(void);
	~CScene(void);


	CObject *GetRootObject(){ return m_pRootObj; }
	CObject *FindObject( const TCHAR *szName);

	CObject *	LoadMeshObjectsFromFile(const TCHAR *path);
	
	
	bool		LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path);

protected:
	void			RecursiveDeleteObjects(CObject *pObj);
	CObject		*	RecursiveFindObject( CObject *pObj, const TCHAR *szName );
	CMeshObject *	RecursiveLoadMeshObjectFromFile(const struct aiScene* pScene, const struct aiNode* pNode);
	

protected:
	CObject				*	m_pRootObj;

	std::vector<CMaterial *> m_vecMaterials;

private:
	UINT			m_lastMaterialIndex;	// internal use; used as an offset into the materials vector by meshes
};

} // end namespace BLUE