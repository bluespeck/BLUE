#pragma once

#include <list>

#include "Process.h"

namespace BLUE
{

typedef std::list<CProcess *> ProcessList;

class CProcessManager
{
public:
	void Attach(CProcess *pProcess);

	bool HasProcesses();
	bool IsProcessActive( int type);

	void UpdateProcesses(int deltaMilliseconds);
	
	~CProcessManager(void);

protected:
	ProcessList m_processList;
	
private:
	void Detach(CProcess *pProcess);
};

} // end namespace BLUE