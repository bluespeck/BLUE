
#pragma once

//#include <windows.h>

//#include <d3d10.h>
//#include <d3dx10.h>
//#include <dxerr.h>

#include <vector>

#include "Matrix4.h"
#include "Vector3.h"
#include "IController.h"
//#include "Physics.h"
//#include "AABB.h"
//#include "OBB.h"

namespace BLUE
{

// Vertex format
struct CVertex
{
	CVector3 position;	
	CVector3 normal;
	CVector3 texcoord;

	bool operator == (const CVertex &vertex)
	{
		return position == vertex.position && normal == normal && texcoord == vertex.texcoord;
	}	
};

// Material properties in a mesh subset
struct CMaterial
{
	TCHAR m_name[MAX_PATH];

	CVector3 m_vAmbient;
	CVector3 m_vDiffuse;
	CVector3 m_vSpecular;

	int m_nShininess;
	float m_fAlpha;

	bool m_bSpecular;

	TCHAR m_texturePath[MAX_PATH];
	//ID3D10ShaderResourceView* m_pTexture;
	//ID3D10EffectTechnique*  m_pTechnique;

public:
	CMaterial()
	{		
		m_vAmbient = CVector3( 0.2f, 0.2f, 0.2f );
		m_vDiffuse = CVector3( 0.8f, 0.8f, 0.8f );
		m_vSpecular = CVector3( 1.0f, 1.0f, 1.0f );
		m_nShininess = 0;
		m_fAlpha = 1.0f;
		m_bSpecular = false;
		//m_pTexture = NULL;
		//m_pTechnique = NULL;
		m_texturePath[0] = 0;
		m_name[0] = 0;
	}
};

// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CCacheEntry
{
	UINT m_index;
	CCacheEntry* m_pNext;
};

class CObject
{
public:
	CObject( void ); 	
	virtual ~CObject( void );
	
	CObject *m_pParent;
	CObject *m_pChild;
	CObject *m_pNextBrother;

	CMatrix4 m_matGlobal, m_matLocal;
	CVector3 m_vecScaleFactors;	// model scale factors
	std::vector< IController *> m_pControllers;	

	// Used to construct the mesh	
	std::vector< CVertex >		m_vertices;
	std::vector< DWORD >		m_indices;
	std::vector< DWORD >		m_attributes;
	std::vector< CMaterial * >	m_materials;

	//std::vector< CCacheEntry* > m_vertexCache;

	// Used when drawing the mesh
	//unsigned int m_numAttribTableEntries;
	//D3DX10_ATTRIBUTE_RANGE *m_pAttribTable;
	
	//ID3DX10Mesh* m_pMesh;	// Used when drawing the object

	//CPhysics *m_pPhysics;	// Physics module [optional]

	void SetDirty(bool bDirty);	
	bool GetDirty();

	void SetHighlight(bool bHighlight);
	bool GetHighlight();

	void GetGlobalPosition( CVector3 &vecPos );

	void GetAABB( CVector3 &vecLBN, CVector3 &vecRTF);
	//const CAABB &GetAABB();

	virtual void DeleteMe();
	virtual void DeleteHierarchy();
	
	void SetName(const TCHAR *s);

	//void CreateObjectFromFile(ID3D10Device *pDevice, const char *szfileName);
	
	void AddChild(CObject *pObj);

	unsigned long GetNumSubsets();
	CMaterial * GetSubsetMaterial(unsigned long iSubset);

	
	CObject *GetRootObj();
	// Detect collisions between given bounding box and other objects bounding boxes
	// vecTranslate is added to the corners of the bb prior to detection
	// Coordinates must be given in world space
	CObject *	CollisionDetect( const CVector3 &vecTranslate );
	//CObject *	CollisionDetect( CObject *pObj, const COBB &obbA );
	//CObject *	CollisionDetect( CObject *pObj, const CVector3 &vecLBN, const CVector3 &vecRTF );

private:
	void LoadGeometryFromOBJFile(const TCHAR *szFileName);
	void LoadMaterialsFromMTLFile(const TCHAR *szFileName);
	DWORD AddVertex( UINT hash, CVertex* pVertex );
	void FreeCache();
	void CleanUp();

private:
	static unsigned long m_objCount;
	bool m_bDirty;	// flag that determines when the global matrix needs 
					// readjusting (based on local matrix and parent global matrix)
	bool m_bHighlight;	// true if the object is highlighted

	TCHAR *m_szName;	// Object name

	//CAABB aabb;

};

}