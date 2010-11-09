#include "stdafx.h"
#include "Mesh.h"

namespace BLUE
{

CMesh::CMesh(void)
: m_pVertices(NULL)
, m_pIndices(NULL)
, m_pNormals(NULL)
, m_pMaterials(NULL)
, m_numIndices(0)
, m_numVertices(0)
{
}

CMesh::~CMesh(void)
{
}

void CMesh::AllocSpace(UINT numVertices, UINT numIndices, UINT numMaterials)
{
	if(numVertices)
		m_pVertices = new CVector3[numVertices];
	if(numIndices)
		m_pIndices = new UINT[numIndices];
	if(numMaterials)
		m_pMaterials = new CMaterial[numMaterials];
}

} //end namespace BLUE
