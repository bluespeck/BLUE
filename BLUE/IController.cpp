#include "stdafx.h"

#include "IController.h"

namespace BLUE
{

IController::IController( class CObject * pObj)
:m_pObj( pObj )
{
}

IController::~IController(void)
{
	m_pObj = 0;
}

void IController::Update( float dt )
{

}

void IController::DeleteMe()
{
	delete this;
}

}