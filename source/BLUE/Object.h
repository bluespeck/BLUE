
#pragma once

#include <windows.h>

#include <vector>

#include <tchar.h>

#include "Matrix4.h"
#include "Vector3.h"
#include "IController.h"

//#include "Physics.h"

#define MAX_PATH 256

namespace BLUE
{

class CObject
{
public:
	CObject( void ); 	
	virtual ~CObject( void );
	
	CObject *m_pParent;
	CObject *m_pChild;
	CObject *m_pNextBrother;

	CMatrix4 m_matGlobal, m_matLocal;
	CVector3 m_vecScaleFactors;	// model scale factors
	std::vector< IController *> m_pControllers;	

	//CPhysics *m_pPhysics;	// Physics module [optional]

	void GetGlobalPosition( CVector3 &vecPos );

	void SetDirty(bool bDirty);	
	bool GetDirty();

	void SetHighlight(bool bHighlight);
	bool GetHighlight();
	
	virtual void DeleteMe();
	virtual void DeleteHierarchy();
	
	void SetName(const TCHAR *s);

	void AddChild(CObject *pObj);

	CObject *GetRootObj();

protected:
	static unsigned long m_objCount;
	bool m_bDirty;	// flag that determines when the global matrix needs 
					// readjusting (based on local matrix and parent global matrix)
	bool m_bHighlight;	// true if the object is highlighted

	TCHAR *m_szName;	// Object name

};

}