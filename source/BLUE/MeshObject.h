#pragma once
#include "Object.h"
#include "Mesh.h"

namespace BLUE
{

class CMeshObject :	public CObject
{
public:
	CMeshObject(void);
	virtual ~CMeshObject(void);

public:

	CMesh *m_pMesh;
};

} // end namespace BLUE