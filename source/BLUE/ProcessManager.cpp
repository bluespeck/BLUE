#include "stdafx.h"

#include <list>

#include "ProcessManager.h"

namespace BLUE
{

CProcessManager::~CProcessManager(void)
{
}

void CProcessManager::UpdateProcesses(int dt)
{
	ProcessList::iterator it = m_processList.begin();
	ProcessList::iterator end = m_processList.begin();
	CProcess *pNext;
	while( it != end )
	{
		CProcess *p = *it;
		if(p->IsDead())
		{
			pNext = p->GetNext();
			if(pNext)
			{
				p->SetNext(NULL);
				Attach(pNext);
			}
			Detach(p);
		}
		else if(p->IsActive() && !p->IsPaused())
		{
			p->OnUpdate( dt );
		}
		++it;
	}
}

} // end namespace BLUE