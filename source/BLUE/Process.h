#pragma once

namespace BLUE
{

class CProcess
{
	friend class CProcessManager;

protected:
	int	m_type;
	bool m_bKill;
	bool m_bActive;
	bool m_bPaused;
	bool m_bInitialUpdate;
	CProcess *m_pNext;

private:
	UINT m_processFlags;

public:
	CProcess(int type, UINT order = 0);
	virtual ~CProcess(void);

public:
	bool IsDead() const { return m_bKill; }
	int GetType() const { return m_type; }

	bool IsActive() const { return m_bActive; }
	void SetActive(bool bActive) { m_bActive = bActive; }
	bool IsAttached()const;
	void SetAttached(bool wantAttached);

	bool IsPaused() const { return m_bPaused; }

	bool IsInitialized() const { return !m_bInitialUpdate; }
	CProcess * const GetNext() const { return m_pNext; }
	void SetNext(CProcess * pNext);

	virtual void OnUpdate(int deltaMilliseconds);
	virtual void OnInitialize() {}
	virtual void Kill();
	virtual void TogglePause() { m_bPaused = !m_bPaused; }	
};

inline void CProcess::OnUpdate(int deltaMilliseconds)
{
	if( m_bInitialUpdate )
	{
		OnInitialize();
		m_bInitialUpdate = false;
	}
}

} // end namespace BLUE