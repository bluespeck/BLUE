#pragma once
#include "object.h"
#include "EngineUtils.h"

namespace BLUE
{

class CPointObject : public CObject
{
public:
	CPointObject(void);
	~CPointObject(void);

public:
	virtual ObjectType GetObjectType(){ return OT_POINT; }	// the type of the object

protected:
	
	// color
};

} // end namespace BLUE
