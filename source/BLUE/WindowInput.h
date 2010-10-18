#pragma once

class CWindowInput
{
public:
	CWindowInput(void);
	~CWindowInput(void);

	void SetKeyDown(unsigned int key){m_keys[key] = true;}
	void SetKeyUp(unsigned int key){m_keys[key] = false;}
	bool IsKeyDown(unsigned int key){return m_keys[key];}
private:
	bool m_keys[256];
};
