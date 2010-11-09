#pragma once
#include <tchar.h>
#include "Vector3.h"

namespace BLUE
{

class CMaterial
{
	TCHAR m_name[MAX_PATH];

	CVector3 m_vAmbient;
	CVector3 m_vDiffuse;
	CVector3 m_vSpecular;

	int m_nShininess;
	float m_fAlpha;

	bool m_bSpecular;

	TCHAR m_texturePath[MAX_PATH];

public:
	CMaterial();
	~CMaterial(void);
};
} //end namespace BLUE