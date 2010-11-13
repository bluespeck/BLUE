#pragma once

#include "Object.h"
#include "MeshObject.h"

namespace BLUE
{

class CScene
{
public:
	CScene(void);
	~CScene(void);


	void Update(float dt);
	
	CObject *GetRootObject(){ return m_pRootObj; }
	CObject *FindObject( const TCHAR *szName);

	CObject *	LoadMeshObjectFromFile(const TCHAR *path);
	
	
	bool		LoadObjectFromFile(const TCHAR *szName, const TCHAR *szParentName, ObjectType objType, const TCHAR *path);

protected:
	void			RecursiveDeleteObjects(CObject *pObj);
	void			RecursiveUpdate(CObject *pObject, float dt);
	CObject		*	RecursiveFindObject( CObject *pObj, const TCHAR *szName );
	CMeshObject *	RecursiveLoadMeshObjectFromFile(const struct aiScene* pScene, const struct aiNode* pNode);
	

protected:
	CObject				*	m_pRootObj;
};

} // end namespace BLUE