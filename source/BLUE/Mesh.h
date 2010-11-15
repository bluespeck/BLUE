#pragma once

#include "Material.h"
#include "Vector3.h"
#include "Defines.h"

namespace BLUE
{

class CMesh
{
public:
	CMesh(void);
	~CMesh(void);


	// Configuration methods; must be called before InitMesh
	void SetNumVertices(UINT numVertices);
	void SetHasNormals(bool bHasNormals);
	void SetNumTexCoordsPerVertex(UINT numTexCoordsPerVertex);
	void SetNumMaterials(UINT numMaterials);
	
	// Allocates memory for all the buffers inside this mesh
	void InitMesh();

public:
	CVector3	*m_pVertices;
	CVector3	*m_pNormals;
	CVector3	*m_pTexCoords[BLUE_MAX_TEXTURE_COORDS];

	UINT		*m_pIndices;
	UINT		*m_pMaterials;
	UINT		*m_pMaterialRanges;		// {mat(1)_start, mat(2)_start, ... mat(m_numMaterials)_start}

	bool		m_bHasNormals;

	UINT		m_numVertices;
	UINT		m_numIndices;
	UINT		m_numTexCoordsPerVertex;	
	UINT		m_numMaterials;
};

}// end namespace BLUE