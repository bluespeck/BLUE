#pragma once

namespace BLUE
{

class CMatrix4
{
public:
	CMatrix4(void);
	CMatrix4(float *pMat);
	CMatrix4(const float _11, const float _12, const float _13, const float _14,
			 const float _21, const float _22, const float _23, const float _24,
			 const float _31, const float _32, const float _33, const float _34,
			 const float _41, const float _42, const float _43, const float _44);
	~CMatrix4(void);

	operator float *();

	// (OUT, IN)
	static void		Copy(CMatrix4 &matDest, const CMatrix4 &matSrc);

	// (OUT)
	static void		MatZero(CMatrix4 &mat);

	static void		MatIdentity(CMatrix4 &mat);
	
	static void		MatRotateX(CMatrix4 &mat, const float fAngleX);// (OUT, IN)	
	static void		MatRotateY(CMatrix4 &mat, const float fAngleY);// (OUT, IN)	
	static void		MatRotateZ(CMatrix4 &mat, const float fAngleZ);// (OUT, IN)

	// Rotation about an arbitrary axis (OUT, IN, IN)
	static void		MatRotate(CMatrix4 &mat, const float fAngle, const class CVector3 &vecAxisOfRotation);
	// Rotation about an arbitrary axis (OUT, IN, IN, IN, IN)	
	static void		MatRotate(CMatrix4 &mat, const float fAngle, const float fX, const float fY, const float fZ);	
	static void		MatScale(CMatrix4 &mat, const float fScaleX, const float fScaleY = 1.0f, const float fScaleZ = 1.0f);	// (OUT, IN[, IN, IN])	
	static void		MatTranslate(CMatrix4 &mat, const float fX, const float fY, const float fZ);// (OUT, IN, IN, IN)	
	static void		Transpose(CMatrix4 &mat, const CMatrix4 &a);// (OUT, IN)	
	static void		Inverse(CMatrix4 &mat, const CMatrix4 &a);// (OUT, IN)	
	static float	Determinant(const CMatrix4 &a);// (IN):OUT	
	static void		Multiply(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2);// (OUT, IN, IN)	
	static void		Add(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2);// (OUT, IN, IN)	
	static void		Subtract(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2);// (OUT, IN, IN)	
	static void		Multiply(CMatrix4 &matResult, const CMatrix4 &mat1, const float fScale);// (OUT, IN, IN)
	// Rotation matrix given by two sets of mouse coordinates projected onto a sphere
	// This gets read of mouse rotation hysteresis
	static void		MatMouseRotation( CMatrix4 &matResult, unsigned int startx, unsigned int starty, 
										unsigned int endx, unsigned int endy, 
										float centerx, float centery, float fRadius );
	
	CMatrix4 operator * (const CMatrix4 &) const;
	CMatrix4 operator + (const CMatrix4 &) const;
	CMatrix4 operator - (const CMatrix4 &) const;
	CMatrix4 operator * (const float) const;

	void SetRow(const CVector3 &vecRow, const unsigned int index);	
	void GetRow(CVector3 &vecRow, const unsigned int index);
	void SetColumn(const CVector3 &vecColumn, const unsigned int index);
	void GetColumn(CVector3 &vecColumn, const unsigned int index);

	union
	{		
		struct  
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float mat[4][4];
	};
};

}