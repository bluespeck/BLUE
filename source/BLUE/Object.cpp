#include "stdafx.h"

#include <cstring>
#include <cassert>

#include <fstream>
#include <set>
#include <algorithm>
#include <tchar.h>

#include "Object.h"

namespace BLUE
{

unsigned long CObject::m_objCount = 0;

CObject::CObject( void ):
m_pParent( NULL ), 
m_pChild( NULL ), 
m_pNextBrother( NULL ),
m_bDirty(false), 
m_vecScaleFactors( 1.0f, 1.0f, 1.0f ), 
m_bHighlight( false ),
m_eType( OT_BASIC )

{	
	m_objCount++;

	m_szName = new TCHAR[16];
	_tcprintf_s( m_szName, 15, _T( "obj_%ld" ), m_objCount );		
	
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

const TCHAR *CObject::GetName()
{
	return m_szName;
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

bool CObject::GetDirty()
{
	return m_bDirty;
}

void CObject::SetDirty(bool bDirty)
{
	m_bDirty = bDirty;
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

CObject *CObject::GetRootObj()
{
	CObject *pParent = this;
	while( pParent->m_pParent != NULL )
	{
		pParent = pParent->m_pParent;
	}

	return pParent;
}

}