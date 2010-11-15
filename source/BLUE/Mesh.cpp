#include "stdafx.h"
#include "Mesh.h"

namespace BLUE
{

CMesh::CMesh(void)
: m_pVertices(NULL)
, m_pIndices(NULL)
, m_pNormals(NULL)
, m_pMaterials(NULL)
, m_pMaterialRanges(NULL)
, m_numIndices(0)
, m_numVertices(0)
, m_numTexCoordsPerVertex(0)
, m_bHasNormals(false)

{
	for(UINT i = 0; i < BLUE_MAX_TEXTURE_COORDS; ++i)
	{
		m_pTexCoords[i] = NULL;
	}
}

CMesh::~CMesh(void)
{
	if(m_pVertices)
		delete[] m_pVertices;
	if(m_pIndices)
		delete[] m_pIndices;
	if(m_pNormals)
		delete[] m_pNormals;
	if(m_pMaterials)
		delete[] m_pMaterials;
	if(m_pMaterialRanges)
		delete[] m_pMaterialRanges;
	for(UINT i = 0; i < m_numTexCoordsPerVertex; ++i)
		if(m_pTexCoords[i])
			delete[] m_pTexCoords[i];
}

void CMesh::SetNumVertices(UINT numVertices)
{
	m_numVertices = numVertices;
}

void CMesh::SetHasNormals(bool bHasNormals)
{
	m_bHasNormals = bHasNormals;
}

void CMesh::SetNumTexCoordsPerVertex(UINT numTexCoordsPerVertex)
{
	m_numTexCoordsPerVertex = numTexCoordsPerVertex;
}

void CMesh::SetNumMaterials(UINT numMaterials)
{
	m_numMaterials = numMaterials;
}

// Allocates memory for all the buffers inside this mesh
void CMesh::InitMesh()
{
	if(m_numVertices)
	{
		m_pVertices = new CVector3[m_numVertices];
		memset(m_pVertices, 0, m_numVertices * sizeof(CVector3));
	}

	if(m_numIndices)
	{	
		m_pIndices = new UINT[m_numIndices];
		memset(m_pIndices, 0, m_numIndices * sizeof(UINT));
	}
	
	if(m_numMaterials)
	{
		m_pMaterials = new UINT[m_numMaterials];
		m_pMaterialRanges = new UINT[m_numMaterials];
		
	}

	if(m_bHasNormals)
	{
		m_pNormals = new CVector3[m_numVertices];
		memset(m_pNormals, 0, m_numVertices * sizeof(CVector3));
	}

	if(m_numTexCoordsPerVertex > 0)
	{
		for(UINT i = 0; i < m_numTexCoordsPerVertex; ++i)
		{
			m_pTexCoords[i] = new CVector3[m_numVertices];
			memset(m_pTexCoords[i], 0, m_numVertices * sizeof(CVector3));
		}
	}
}

} //end namespace BLUE
