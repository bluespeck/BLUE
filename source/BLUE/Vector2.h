#pragma once
#include "Matrix4.h"
#include <fstream>

namespace BLUE
{

class CVector2
{
public:
	CVector2( void );
	CVector2( float *vfCoords );
	CVector2( const float fX, const float fY );
	~CVector2( void );

	operator float *();
	
	// Dot product	(IN, IN):OUT
	static float	DotProduct( const CVector2 &vec1, const CVector2 &vec2 );
	
	static void		Copy( CVector2 &vecDest, const CVector2 &vecSrc );// (OUT, IN)	
	static void		Add( CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2 );	// (OUT, IN, IN)
	static void		Subtract( CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2 );	// (OUT, IN, IN)
	
	// Multiply a vector with a scalar
	static void		Multiply( CVector2 &vecResult, const CVector2 &vec, float fScale);
	// Scale second vector then add result with first vector (OUT, IN, IN, IN)
	static void		MAdd( CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2, float fScale );
	// Negates a vector (OUT, IN)
	static void		Negate( CVector2 &vecResult, const CVector2 &vec );
	// Normalizez a vector (OUT, IN)
	static float	Normalize( CVector2 &vecResult, const CVector2 &vec );
	
	static void		Transform(CVector2 &vecResult, const CVector2 &vec, const CMatrix4 &mat);

	// Maximize / minimize components of max/min vectors with the components of vec
	static void		Maximize(CVector2 &vecMax, const CVector2 &vec);
	static void		Minimize(CVector2 &vecMin, const CVector2 &vec);

	float	GetLength();

	bool operator==(const CVector2 &vec);
	bool operator!=(const CVector2 &vec);

	union
	{
		struct  
		{
			float x, y;
		};
		float vec[2];
	};

	friend std::ostream &operator <<(std::ostream &os, const CVector2 &vec);
	friend std::istream &operator >>(std::istream &is, CVector2 &vec);
};

std::ostream &operator <<(std::ostream &os, const CVector2 &vec);
std::istream &operator >>(std::istream &is, CVector2 &vec);

}