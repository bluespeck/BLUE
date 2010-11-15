#pragma once
#include <tchar.h>
#include "Vector3.h"
#include "Color.h"

namespace BLUE
{

class CMaterial
{
public:
	TCHAR m_name[MAX_PATH];

	CColor m_vAmbient;
	CColor m_vDiffuse;
	CColor m_vSpecular;

	int m_nShininess;
	float m_fAlpha;

	bool m_bSpecular;

public:
	CMaterial();
	~CMaterial(void);
};
} //end namespace BLUE