#pragma once

namespace BLUE
{

class IController
{
public:
	IController(class CObject *);
	virtual ~IController(void);
	virtual void DeleteMe();

	virtual	void Update(float dt);

	class CObject *m_pObj;	// Points to the object on which this controller is placed
};

}