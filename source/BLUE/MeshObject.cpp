#include "stdafx.h"

#include <assert.h>

#include "MeshObject.h"

namespace BLUE
{

//--------------------------------------------------------------------------------------
CMeshObject::CMeshObject(void)
: m_pMesh(NULL)
{
}

CMeshObject::~CMeshObject(void)
{	
	delete m_pMesh;
}

} // end namespace BLUE