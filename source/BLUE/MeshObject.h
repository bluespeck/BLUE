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
	const ObjectType m_eType;	// the type of the object

	CMesh *m_pMesh;
};

} // end namespace BLUE