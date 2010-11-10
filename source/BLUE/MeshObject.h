#pragma once
#include "Object.h"
#include "Mesh.h"
#include "EngineUtils.h"

namespace BLUE
{

class CMeshObject :	public CObject
{
public:
	CMeshObject(void);
	virtual ~CMeshObject(void);

public:
	virtual ObjectType GetObjectType(){ return OT_MESH; }

	CMesh *m_pMesh;
};

} // end namespace BLUE