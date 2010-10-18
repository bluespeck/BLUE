#include "stdafx.h"

#include <cstdio>
#include <cstring>
#include <cassert>

#include <fstream>
#include <set>
#include <algorithm>

//#include <D3DX10mesh.h>
#include <tchar.h>
#include "Object.h"
//#include "OBB.h"
//#include "MiscDefines.h"

namespace BLUE
{

unsigned long CObject::m_objCount = 0;

// Define the input layout
//const D3D10_INPUT_ELEMENT_DESC g_objLayout[] =
//{
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//} ;

//UINT g_numLayoutElements = sizeof( g_objLayout ) / sizeof( g_objLayout[0] );

CObject::CObject( void ):
m_pParent(NULL), 
m_pChild(NULL), 
m_pNextBrother(NULL), 
//m_pAttribTable(NULL),
//m_pMesh(NULL), 
m_bDirty(false), 
m_vecScaleFactors(1.0f, 1.0f, 1.0f), 
m_bHighlight(false)
{	
	m_objCount++;

	m_szName = new TCHAR[16];
	_tcprintf_s( m_szName, 15, _T("obj_%ld"), m_objCount );		
	
	CMatrix4::MatIdentity( m_matGlobal );
	CMatrix4::MatIdentity( m_matLocal );	

	//m_pPhysics = NULL;	
	
}

CObject::~CObject( void )
{
	m_pParent = NULL;
	m_pChild = NULL;
	m_pNextBrother = NULL;
	delete[] m_szName;

	// Free space occupied by controllers
	for( int i = 0, s = m_pControllers.size(); i < s; i++ )
		m_pControllers[i]->DeleteMe();
	m_pControllers.clear();
	
	CleanUp();
}

void CObject::DeleteMe()
{
	delete this;
}

void CObject::DeleteHierarchy()
{
	for( CObject * pObj = m_pChild; pObj; )
	{
		CObject *pNextObj = pObj->m_pNextBrother;
		pObj->DeleteHierarchy();		
		pObj = pNextObj;
	}
	DeleteMe();
}

void CObject::SetName( const TCHAR *szName )
{
	if( szName != NULL || szName[0] == 0)
	{
		if(m_szName != NULL)
			delete[] m_szName;

		unsigned int size = _tcslen( szName ) + 1;
		m_szName = new TCHAR[size];
		_tcscpy_s( m_szName, size, szName );
	}	
}

//--------------------------------------------------------------------------------------
DWORD CObject::AddVertex( UINT hash, CVertex* pVertex )
{
	bool bFound = false;
	DWORD index = 0;
/*
	if( (UINT)m_vertexCache.size() > hash )
	{
		CCacheEntry* pEntry = m_vertexCache[hash];
		while( pEntry != NULL )
		{
			CVertex* pCacheVertex = ( (CVertex*)&*m_vertices.begin() ) + pEntry->m_index;

			// vertex already in the list... index buffer should point here
			if( 0 == memcmp( pVertex, pCacheVertex, sizeof( CVertex ) ) )
			{
				bFound = true;
				index = pEntry->m_index;
				break;
			}

			pEntry = pEntry->m_pNext;
		}
	}

	// Vertex not found in the list. Create new entry and add vertex to vertices list
	if( !bFound )
	{
		// Add new entry to the vertices list
		index = m_vertices.size();
		m_vertices.push_back( *pVertex );

		// Create new cache entry and add it to the hash table
		CCacheEntry* pNewEntry = new CCacheEntry;
		if( pNewEntry == NULL )
			return (DWORD)E_OUTOFMEMORY;

		pNewEntry->m_index = index;
		pNewEntry->m_pNext = NULL;

		// Grow the cache if needed
		while( (UINT)m_vertexCache.size() <= hash )
		{
			m_vertexCache.push_back( NULL );
		}

		// Add to the end of the linked list
		CCacheEntry* pEntry = m_vertexCache[hash];
		if( pEntry == NULL )
		{
			// This is the head element for this hash
			m_vertexCache[hash] = pNewEntry;
		}
		else
		{
			// Find the tail
			while( pEntry->m_pNext != NULL )
			{
				pEntry = pEntry->m_pNext;
			}

			pEntry->m_pNext = pNewEntry;
		}
	}
*/
	return index;
}

void CObject::FreeCache()
{
/*
	// Iterate through all the elements in the cache and subsequent linked lists
	for( int i = 0, s = m_vertexCache.size(); i < s; i++ )
	{
		CCacheEntry* pEntry = m_vertexCache[i];
		while( pEntry != NULL )
		{
			CCacheEntry* pNext = pEntry->m_pNext;
			delete pEntry;						
			pEntry = pNext;
		}
	}
	m_vertexCache.clear();
	*/
}

void CObject::LoadGeometryFromOBJFile(const TCHAR *szFileName)
{
	TCHAR strMaterialFilename[MAX_PATH] = {0};	
		
	std::vector <CVector3> vPositions;
	std::vector <CVector3> vTexCoords;	// TODO: temporary; should be vector2
	std::vector <CVector3> vNormals;	
	
	// The first subset uses the default material
	CMaterial* pMaterial = new CMaterial();
	assert(pMaterial != NULL);

	_tcscpy_s( pMaterial->m_name, _T("default") );
	m_materials.push_back( pMaterial );
	
	unsigned long currentSubset = 0;

	// File input
	TCHAR strCommand[256] = {0};
#ifdef UNICODE
	std::wifstream fInput( szFileName );
#else
	std::ifstream fInput( szFileName );
#endif

	assert(fInput);

	for(; ; )
	{
		fInput >> strCommand;
		if( !fInput )
			break;

		if( 0 == _tcscmp( strCommand, _T("#") ) )
		{
			// Comment
		}
		else if( 0 == _tcscmp( strCommand, _T("v") ) )
		{
			// Vertex position
			float x, y, z;
			fInput >> x >> y >> z;
			vPositions.push_back( CVector3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vt") ) )
		{
			// Vertex texture coords
			float u, v;
			fInput >> u >> v;
			vTexCoords.push_back( CVector3( u, v, 0 ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vn") ) )
		{
			// Vertex normal
			float x, y, z;
			fInput >> x >> y >> z;
			vNormals.push_back( CVector3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("f") ) )
		{
			// Face
			UINT iPosition, iTexCoord, iNormal;
			CVertex vertex;

			for( UINT iFace = 0; iFace < 3; iFace++ )
			{
				ZeroMemory( &vertex, sizeof( CVertex ) );

				// OBJ format uses 1-based arrays
				fInput >> iPosition;
				vertex.position = vPositions[ iPosition - 1 ];

				if( _T('/') == fInput.peek() )
				{
					fInput.ignore();

					if( _T('/') != fInput.peek() )
					{
						// Optional texture coordinate
						fInput >> iTexCoord;
						vertex.texcoord = vTexCoords[ iTexCoord - 1 ];
					}

					if( _T('/') == fInput.peek() )
					{
						fInput.ignore();

						// Optional vertex normal
						fInput >> iNormal;
						vertex.normal = vNormals[ iNormal - 1 ];
					}
				}
				// Add the vertex to vertices list if it isn't already there
				DWORD index = AddVertex( iPosition, &vertex );
				m_indices.push_back( index);									

				// Build bounding box
				//aabb.Add( (float *)vertex.position );			
				
				//m_indices.push_back( m_vertices.size());
				//m_vertices.push_back(vertex);

			}
			m_attributes.push_back( currentSubset );
			
		}
		else if( 0 == _tcscmp( strCommand, _T("mtllib") ) )
		{			
			// Material library
			fInput >> strMaterialFilename;

			// In case the path is between double quotes
			if( _T('\"') == strMaterialFilename[0] )
			{
				TCHAR acc[MAX_PATH];
				fInput.getline(acc, MAX_PATH - 1);
				_tcscpy_s(strMaterialFilename, strMaterialFilename + 1);
				_tcscat_s(strMaterialFilename, acc);
				int lastpos = _tcslen( strMaterialFilename ) - 1;
				if( _T('\"') == strMaterialFilename[ lastpos ] )
				{
					strMaterialFilename[lastpos] = 0;
				}
			}
		}
		else if( 0 == _tcscmp( strCommand, _T("usemtl") ) )
		{
			// Material
			TCHAR strName[MAX_PATH] = {0};
			fInput >> strName;

			bool bMaterialFound = false;
			for( int iMaterial = 0, s = m_materials.size(); iMaterial < s; iMaterial++ )
			{
				CMaterial* pCurrentMaterial = m_materials[iMaterial];
				if( 0 == _tcscmp( pCurrentMaterial->m_name, strName ) )
				{
					bMaterialFound = true;
					currentSubset = iMaterial;
					break;
				}
			}

			if( !bMaterialFound )
			{
				pMaterial = new CMaterial();				
				assert( pMaterial != NULL );

				currentSubset = m_materials.size();				
				_tcscpy_s( pMaterial->m_name, MAX_PATH - 1, strName );

				m_materials.push_back( pMaterial );
			}
		}		

		fInput.ignore( 1000, _T('\n') );
	}

	fInput.close();	
	FreeCache();

	// A material was found. Read it as well
	if( strMaterialFilename[0] )
	{	
		if(!_tcschr(strMaterialFilename, _T('\\')) && !_tcschr(strMaterialFilename, _T('/')))
		{

			// Find complete path of material file
			const TCHAR * pFName = _tcsrchr( szFileName, _T('\\') );
			TCHAR szCompletePath[MAX_PATH] = {0};
			if(!pFName)
				pFName = _tcsrchr( szFileName, _T('/') );		
			if(pFName)
				_tcsncpy_s( szCompletePath, szFileName, pFName - szFileName + 1 );		
			
			_tcscat_s( szCompletePath, strMaterialFilename );
			LoadMaterialsFromMTLFile( szCompletePath );
		}
		else
		{
			LoadMaterialsFromMTLFile( strMaterialFilename );
		}
		
	}
}

void CObject::LoadMaterialsFromMTLFile( const TCHAR *szFileName )
{	
	// File input
	TCHAR strCommand[256] = {0};
#ifdef UNICODE
	std::wifstream fIn( szFileName );
#else
	std::ifstream fIn( szFileName );
#endif

	assert(fIn);	
	
	CMaterial* pMaterial = NULL;

	for( ; ; )
	{
		fIn >> strCommand;
		if( !fIn )
			break;

		if( 0 == _tcscmp( strCommand, _T("newmtl") ) )
		{
			// Switch active material
			TCHAR strName[MAX_PATH] = {0};
			fIn >> strName;

			pMaterial = NULL;
			for( int i = 0, s = m_materials.size(); i < s; i++ )
			{
				CMaterial* pMat = m_materials[i];
				if( 0 == _tcscmp( pMat->m_name, strName ) )
				{
					pMaterial = pMat;
					break;
				}
			}
		}

		// The rest of the commands rely on an active material
		if( pMaterial == NULL )
			continue;

		if( 0 == _tcscmp( strCommand, _T("#") ) )
		{
			// Comment
		}
		else if( 0 == _tcscmp( strCommand, _T("Ka") ) )
		{
			// Ambient color
			float r, g, b;
			fIn >> r >> g >> b;
			pMaterial->m_vAmbient = CVector3( r, g, b );
		}
		else if( 0 == _tcscmp( strCommand, _T("Kd") ) )
		{
			// Diffuse color
			float r, g, b;
			fIn >> r >> g >> b;
			pMaterial->m_vDiffuse = CVector3( r, g, b );
		}
		else if( 0 == _tcscmp( strCommand, _T("Ks") ) )
		{
			// Specular color
			float r, g, b;
			fIn >> r >> g >> b;
			pMaterial->m_vSpecular = CVector3( r, g, b );
		}
		else if( 0 == _tcscmp( strCommand, _T("d") ) ||
			0 == _tcscmp( strCommand, _T("Tr") ) )
		{
			// Alpha
			fIn >> pMaterial->m_fAlpha;
		}
		else if( 0 == _tcscmp( strCommand, _T("Ns") ) )
		{
			// Shininess			
			fIn >> pMaterial->m_nShininess;			
		}
		else if( 0 == _tcscmp( strCommand, _T("illum") ) )
		{
			// Specular on/off
			int illumination;
			fIn >> illumination;
			pMaterial->m_bSpecular = ( illumination == 2 );
		}
		else if( 0 == _tcscmp( strCommand, _T("map_Kd") ) )
		{
			// Texture
			fIn >> pMaterial->m_texturePath;

			// In case the path is between double quotes
			if( _T('\"') == pMaterial->m_texturePath[0] )
			{
				TCHAR acc[MAX_PATH];
				fIn.getline( acc, MAX_PATH - 1 );
				_tcscpy_s( pMaterial->m_texturePath, pMaterial->m_texturePath + 1 );
				_tcscat_s( pMaterial->m_texturePath, acc );

				int lastpos = _tcslen( pMaterial->m_texturePath ) - 1;
				if( _T('\"') == pMaterial->m_texturePath[ lastpos ] )
				{
					pMaterial->m_texturePath[lastpos] = 0;
				}
			}
			if( !_tcschr(pMaterial->m_texturePath, _T('\\') ) && !_tcschr( pMaterial->m_texturePath, _T('/') ) )
			{

				// Find complete path of material file
				const TCHAR * pFName = _tcsrchr( szFileName, _T('\\') );
				TCHAR szCompletePath[MAX_PATH] = {0};
				if( !pFName )
					pFName = _tcsrchr( szFileName, _T('/') );		
				if( pFName )
					_tcsncpy_s( szCompletePath, szFileName, pFName - szFileName + 1 );		

				_tcscat_s( szCompletePath, pMaterial->m_texturePath );	
				_tcscpy_s( pMaterial->m_texturePath, MAX_PATH - 1, szCompletePath );
			}
		}

		fIn.ignore( 1000, _T('\n') );
	}

	fIn.close();
}

/*
void CObject::CreateObjectFromFile(ID3D10Device *pDevice, const char *szFileName)
{
	if(pDevice)
	{
		CleanUp();
		LoadGeometryFromOBJFile( szFileName );

		// Load material textures
		for ( int iMaterial = 0, s = m_materials.size(); iMaterial < s; iMaterial ++ )
		{
			CMaterial *pMaterial = m_materials[iMaterial];
			if ( pMaterial->m_texturePath[0] )
			{            
				pMaterial->m_pTexture = (ID3D10ShaderResourceView*)1;

				HR( D3DX10CreateShaderResourceViewFromFileA( pDevice, pMaterial->m_texturePath, 0, 0,
						&pMaterial->m_pTexture, false ) ) ;			
			}
			int a = 1;
		}


		// Create the corresponding mesh
		ID3DX10Mesh *pMesh = NULL;

		if(m_vertices.size())
		{

			HR( D3DX10CreateMesh( pDevice,
				g_objLayout,
				g_numLayoutElements,
				g_objLayout[0].SemanticName,
				m_vertices.size(),
				m_indices.size() / 3,
				D3DX10_MESH_32_BIT,
				&pMesh ) );

			// Set the vertex data
			
			pMesh->SetVertexData( 0, (void*)&*m_vertices.begin() );
			m_vertices.clear();

			// Set the index data
			pMesh->SetIndexData( (void*)&*m_indices.begin(), m_indices.size() );
			m_indices.clear();

			// Set the attribute data
			pMesh->SetAttributeData( (UINT*)&*m_attributes.begin() );
			m_attributes.clear();


			// Reorder the vertices according to subset and optimize the mesh for this graphics 
			// card's vertex cache. When rendering the mesh's triangle list the vertices will 
			// cache hit more often so it won't have to re-execute the vertex shader.
			HR( pMesh->GenerateAdjacencyAndPointReps( 1e-6f ) );
			HR( pMesh->Optimize( D3DX10_MESHOPT_ATTR_SORT | D3DX10_MESHOPT_VERTEX_CACHE, NULL, NULL ) );

			pMesh->GetAttributeTable( NULL, &m_numAttribTableEntries );
			m_pAttribTable = new D3DX10_ATTRIBUTE_RANGE[m_numAttribTableEntries];
			pMesh->GetAttributeTable( m_pAttribTable, &m_numAttribTableEntries );
			
			HR( pMesh->CommitToDevice() );

		}
		m_pMesh = pMesh;

		
	}
}
*/
void CObject::CleanUp()
{
	for ( int iMaterial = 0, s = m_materials.size(); iMaterial < s; ++iMaterial )
	{
		CMaterial *pMaterial = m_materials[iMaterial];

/*
		if ( pMaterial->m_pTexture && (DWORD)pMaterial->m_pTexture != 1 )
		{
			ID3D10Resource* pRes = NULL;

			pMaterial->m_pTexture->GetResource( &pRes );
			ReleaseCOM( pRes );
			ReleaseCOM( pRes );   // do this twice, because GetResource adds a ref

			ReleaseCOM( pMaterial->m_pTexture );
		}
*/
		if(pMaterial)
		{
			delete pMaterial;
			pMaterial = NULL;
		}
	}

	m_materials.clear();
	m_vertices.clear();
	m_indices.clear();
	m_attributes.clear();
/*
	if(m_pAttribTable)
	{
		delete[] m_pAttribTable;
		m_pAttribTable = NULL;
	}
	m_numAttribTableEntries = 0;

	ReleaseCOM( m_pMesh );
*/
}

void CObject::AddChild(CObject *pObj)
{
	if(m_pChild == NULL)
	{
		// This object has no children. Add pObj as its only child
		m_pChild = pObj;
	}
	else
	{
		// This object has children. Add pObj at the end of the children list
		CObject *p = m_pChild;
		while(p->m_pNextBrother != NULL)
		{
			p = p->m_pNextBrother;
		}
		p->m_pNextBrother = pObj;		
	}
	
	// Link child to its parent
	pObj->m_pParent = this;

}

unsigned long CObject::GetNumSubsets()
{
	return 0;//m_numAttribTableEntries; 
}

bool CObject::GetDirty()
{
	return m_bDirty;
}

void CObject::SetDirty(bool bDirty)
{
	m_bDirty = bDirty;
}

CMaterial * CObject::GetSubsetMaterial(unsigned long iSubset)
{
	return NULL;//m_materials[m_pAttribTable[iSubset].AttribId];
}

void CObject::SetHighlight(bool bHighlight)
{
	m_bHighlight = bHighlight;
}
bool CObject::GetHighlight()
{
	return m_bHighlight;
}

void CObject::GetGlobalPosition( CVector3 &vecPos )
{
	m_matGlobal.GetRow( vecPos, 3 );
}
/*
const CAABB &CObject::GetAABB()
{
	return aabb;
}

void CObject::GetAABB( CVector3 &vecLBN, CVector3 &vecRTF)
{
	vecLBN = aabb.m_vecLBN;
	vecRTF = aabb.m_vecRTF;
}
*/
CObject *CObject::GetRootObj()
{
	CObject *pParent = this;
	while( pParent->m_pParent != NULL )
	{
		pParent = pParent->m_pParent;
	}

	return pParent;
}

CObject *CObject::CollisionDetect( const CVector3 &vecTranslate)
{
	/*
	CVector3 axes[3];
	m_matGlobal.GetRow(axes[0], 0);
	m_matGlobal.GetRow(axes[1], 1);
	m_matGlobal.GetRow(axes[2], 2);

	CVector3::Normalize( axes[0], axes[0] );
	CVector3::Normalize( axes[1], axes[1] );
	CVector3::Normalize( axes[2], axes[2] );

	// axes[0].x = axes[1].y = axes[2].z = 1.0f;
	
	// Create OBB from AABB using orientation from global matrix
	COBB obbA( aabb, axes[0], axes[1], axes[2] );
	
	CVector3 vecTransToWorld;
	m_matGlobal.GetRow( vecTransToWorld, 3 );
	CVector3::Add( obbA.m_vecPosition, obbA.m_vecPosition, vecTransToWorld );
	CVector3::Add( obbA.m_vecPosition, obbA.m_vecPosition, vecTranslate );
	
	// scale to object size
	obbA.m_vecExtents.x *= m_vecScaleFactors.x;
	obbA.m_vecExtents.y *= m_vecScaleFactors.y;
	obbA.m_vecExtents.z *= m_vecScaleFactors.z;
	

	for( CObject *pObj = GetRootObj()->m_pChild; pObj != NULL; pObj = pObj->m_pNextBrother )
	{
		CObject *pObjCollision = CollisionDetect( pObj, obbA );
		if( pObjCollision != NULL )
			return pObjCollision;
	}
	*/
	return NULL;
}
/*
CObject *CObject::CollisionDetect( CObject *pObj, const COBB &obbA)
{
	
	CVector3 axes[3];

	pObj->m_matGlobal.GetRow(axes[0], 0);
	pObj->m_matGlobal.GetRow(axes[1], 1);
	pObj->m_matGlobal.GetRow(axes[2], 2);

	CVector3::Normalize( axes[0], axes[0] );
	CVector3::Normalize( axes[1], axes[1] );
	CVector3::Normalize( axes[2], axes[2] );


//	axes[0].x = axes[1].y = axes[2].z = 1.0f;


	// Create OBB from AABB using orientation from global matrix
	COBB obbB( pObj->aabb, axes[0], axes[1], axes[2] );

	CVector3 vecTransToWorld;
	pObj->m_matGlobal.GetRow( vecTransToWorld, 3 );
	CVector3::Add( obbB.m_vecPosition, obbB.m_vecPosition, vecTransToWorld );	

	// scale to object size
	obbB.m_vecExtents.x *= pObj->m_vecScaleFactors.x;
	obbB.m_vecExtents.y *= pObj->m_vecScaleFactors.y;
	obbB.m_vecExtents.z *= pObj->m_vecScaleFactors.z;
	

	if( obbA.TestIntersect( obbB ) == true )
		return pObj;

	for( CObject *pObjChild = pObj->m_pChild; pObjChild != NULL; pObjChild = pObjChild->m_pNextBrother )
	{
		CObject *pObjCollision = CollisionDetect( pObjChild, obbA );
		if( pObjCollision != NULL )
			return pObjCollision;
	}

	return NULL;
}
*/

/*
CObject *CObject::CollisionDetect( CObject *pObj, const CVector3 &vecLBN, const CVector3 &vecRTF)
{
	if( pObj != this )	// prevent comparison to self
	{

		// Transform vecLBN and vecRTF into this object's model space
		CMatrix4 matInvGlobal;	// matrix to transport coordinates from world space into object model space
		CVector3 vecLocalLBN, vecLocalRTF;
		CVector3 vecMovingObjExtents( vecRTF.x - vecLBN.x, vecRTF.y - vecLBN.y, vecRTF.z - vecLBN.z );
		CVector3 vecMovingObjPos(	vecRTF.x + 0.5f * vecMovingObjExtents.x,
									vecRTF.y + 0.5f * vecMovingObjExtents.y,
									vecRTF.z + 0.5f * vecMovingObjExtents.z );
		CMatrix4::Inverse( matInvGlobal, pObj->m_matGlobal );
		

		// Test if one of the corners of the tested bounding box is inside of the object BB

		if(
			( vecLocalLBN.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalLBN.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalLBN.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalLBN.x <= pObj->m_vecRightTopFar.x &&
			vecLocalLBN.y <= pObj->m_vecRightTopFar.y &&
			vecLocalLBN.z <= pObj->m_vecRightTopFar.z )			
			||			
			( vecLocalLBN.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalLBN.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalRTF.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalLBN.x <= pObj->m_vecRightTopFar.x &&
			vecLocalLBN.y <= pObj->m_vecRightTopFar.y &&
			vecLocalRTF.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalLBN.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalRTF.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalLBN.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalLBN.x <= pObj->m_vecRightTopFar.x &&
			vecLocalRTF.y <= pObj->m_vecRightTopFar.y &&
			vecLocalLBN.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalLBN.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalRTF.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalRTF.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalLBN.x <= pObj->m_vecRightTopFar.x &&
			vecLocalRTF.y <= pObj->m_vecRightTopFar.y &&
			vecLocalRTF.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalRTF.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalLBN.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalLBN.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalRTF.x <= pObj->m_vecRightTopFar.x &&
			vecLocalLBN.y <= pObj->m_vecRightTopFar.y &&
			vecLocalLBN.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalRTF.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalLBN.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalRTF.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalRTF.x <= pObj->m_vecRightTopFar.x &&
			vecLocalLBN.y <= pObj->m_vecRightTopFar.y &&
			vecLocalRTF.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalRTF.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalRTF.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalLBN.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalRTF.x <= pObj->m_vecRightTopFar.x &&
			vecLocalRTF.y <= pObj->m_vecRightTopFar.y &&
			vecLocalLBN.z <= pObj->m_vecRightTopFar.z )
			||
			( vecLocalRTF.x >= pObj->m_vecLeftBottomNear.x &&
			vecLocalRTF.y >= pObj->m_vecLeftBottomNear.y &&
			vecLocalRTF.z >= pObj->m_vecLeftBottomNear.z &&
			vecLocalRTF.x <= pObj->m_vecRightTopFar.x &&
			vecLocalRTF.y <= pObj->m_vecRightTopFar.y &&
			vecLocalRTF.z <= pObj->m_vecRightTopFar.z )
			)

		{
			return pObj;
		}
		
		if( !(vecLocalRTF.x < pObj->m_vecLeftBottomNear.x ||
			vecLocalRTF.y < pObj->m_vecLeftBottomNear.y ||
			vecLocalRTF.z < pObj->m_vecLeftBottomNear.z ||
			vecLocalLBN.x > pObj->m_vecRightTopFar.x ||
			vecLocalLBN.y > pObj->m_vecRightTopFar.y ||
			vecLocalLBN.z > pObj->m_vecRightTopFar.z ) )
		{
			return pObj;
		}
	}

	for( CObject *pObjChild = pObj->m_pChild; pObjChild != NULL; pObjChild = pObjChild->m_pNextBrother )
	{
		CObject *pObjCollision = CollisionDetect( pObjChild, vecLBN, vecRTF );
		if( pObjCollision != NULL )
			return pObjCollision;
	}
	return NULL;
}

*/

}