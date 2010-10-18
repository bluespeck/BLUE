#pragma once
#include "Matrix4.h"
#include <fstream>

namespace BLUE
{

class CVector3
{
public:
	CVector3(void);
	CVector3(float *vfCoords);
	CVector3(const float fX, const float fY, const float fZ);
	~CVector3(void);

	operator float *();
	
	// Dot product	(IN, IN):OUT
	static float	Dot(const CVector3 &vec1, const CVector3 &vec2);
	// Cross product (OUT, IN, IN)
	static void		Cross(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2);
	
	static void		Copy(CVector3 &vecDest, const CVector3 &vecSrc);// (OUT, IN)	
	static void		Add(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2);	// (OUT, IN, IN)
	static void		Subtract(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2);	// (OUT, IN, IN)
	// Component-wise multiply (OUT, IN, IN)
	static void		Multiply(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2);
	// Scale second vector then add result with first vector (OUT, IN, IN, IN)
	static void		MAdd(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2, const float fScale);
	// Negates a vector (OUT, IN)
	static void		Negate(CVector3 &vecResult, const CVector3 &vec);
	// Normalizez a vector (OUT, IN)
	static float	Normalize(CVector3 &vecResult, const CVector3 &vec);
	
	static void		Transform(CVector3 &vecResult, const CVector3 &vec, const CMatrix4 &mat);

	// Maximize / minimize components of max/min vectors with the components of vec
	static void		Maximize(CVector3 &vecMax, const CVector3 &vec);
	static void		Minimize(CVector3 &vecMin, const CVector3 &vec);

	float	GetLength();

	bool operator==(const CVector3 &vec);
	bool operator!=(const CVector3 &vec);

	union
	{
		struct  
		{
			float x, y, z;
		};
		float vec[3];
	};

	friend std::ostream &operator <<(std::ostream &os, const CVector3 &vec);
	friend std::istream &operator >>(std::istream &is, CVector3 &vec);
};

std::ostream &operator <<(std::ostream &os, const CVector3 &vec);
std::istream &operator >>(std::istream &is, CVector3 &vec);

}