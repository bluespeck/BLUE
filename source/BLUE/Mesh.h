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

	void AllocSpace(UINT numVertices, UINT numIndices, UINT numMaterials);

public:
	CVector3	*m_pVertices;
	CVector3	*m_pNormals;
	CVector3	*m_pTexCoords[BLUE_MAX_TEXTURE_COORDS];

	UINT		*m_pIndices;
	CMaterial	*m_pMaterials;

	UINT		m_numVertices;
	UINT		m_numIndices;
};

}// end namespace BLUE