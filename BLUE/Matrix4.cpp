
#include "stdafx.h"

#include <cmath>
#include <cstring>

#include "Matrix4.h"
#include "Vector3.h"


using namespace std;

namespace BLUE
{

CMatrix4::CMatrix4(void)
{
	memset((void *) &mat, 0, sizeof(CMatrix4));
	_11 = _22 = _33 = _44 = 1.0f;
}

CMatrix4::CMatrix4(float *pMat)
{
	memcpy((void *) &mat, pMat, sizeof(CMatrix4));
}

CMatrix4::CMatrix4(const float _11, const float _12, const float _13, const float _14,
				   const float _21, const float _22, const float _23, const float _24,
				   const float _31, const float _32, const float _33, const float _34,
				   const float _41, const float _42, const float _43, const float _44)
:	_11(_11), _12(_12), _13(_13), _14(_14),
	_21(_21), _22(_22), _23(_23), _24(_24),
	_31(_31), _32(_32), _33(_33), _34(_34),
	_41(_41), _42(_42), _43(_43), _44(_44)
{

}

CMatrix4::~CMatrix4(void)
{
}

CMatrix4::operator float *()
{
	return &_11;
}


void CMatrix4::Copy(CMatrix4 &matDest, const CMatrix4 &matSrc)
{
	memcpy_s((void *)&matDest, sizeof(CMatrix4), (void *)&matSrc, sizeof(CMatrix4));
}

CMatrix4 CMatrix4::operator * (const CMatrix4 &a) const
{
	CMatrix4 res(
		_11 * a._11 + _12 * a._21 + _13 * a._31 + _14 * a._41,
		_11 * a._12 + _12 * a._22 + _13 * a._32 + _14 * a._42,
		_11 * a._13 + _12 * a._23 + _13 * a._33 + _14 * a._43,
		_11 * a._14 + _12 * a._24 + _13 * a._34 + _14 * a._44,
		
		_21 * a._11 + _22 * a._21 + _23 * a._31 + _24 * a._41,
		_21 * a._12 + _22 * a._22 + _23 * a._32 + _24 * a._42,
		_21 * a._13 + _22 * a._23 + _23 * a._33 + _24 * a._43,
		_21 * a._14 + _22 * a._24 + _23 * a._34 + _24 * a._44,

		_31 * a._11 + _32 * a._21 + _33 * a._31 + _34 * a._41,
		_31 * a._12 + _32 * a._22 + _33 * a._32 + _34 * a._42,
		_31 * a._13 + _32 * a._23 + _33 * a._33 + _34 * a._43,
		_31 * a._14 + _32 * a._24 + _33 * a._34 + _34 * a._44,

		_41 * a._11 + _42 * a._21 + _43 * a._31 + _44 * a._41,
		_41 * a._12 + _42 * a._22 + _43 * a._32 + _44 * a._42,
		_41 * a._13 + _42 * a._23 + _43 * a._33 + _44 * a._43,
		_41 * a._14 + _42 * a._24 + _43 * a._34 + _44 * a._44);

	return res;
}

void CMatrix4::MatZero(CMatrix4 &mat)
{
	memset((void *)&mat, 0, sizeof(CMatrix4));
}

void CMatrix4::MatIdentity(CMatrix4 &mat)
{
	memset((void *) &mat, 0, sizeof(CMatrix4));
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
}

void CMatrix4::MatRotateX(CMatrix4 &mat, const float fAngleX)
{
	const float c = cos(fAngleX);
	const float s = sin(fAngleX);
	
	mat._22 = c;
	mat._23 = s;
	mat._32 = -s;
	mat._33 = c;

	mat._12 = mat._13 = mat._14 = mat._21 = mat._24 = mat._31 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
	mat._11 = mat._44 = 1.0f;		
}

void CMatrix4::MatRotateY(CMatrix4 &mat, const float fAngleY)
{
	const float c = cos(fAngleY);
	const float s = sin(fAngleY);
	
	mat._11 = c;	mat._13 = -s;
	mat._31 = s;	mat._33 = c;

	mat._12 = mat._14 = mat._21 = mat._23 = mat._24 = mat._32 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
	mat._22 = mat._44 = 1.0f;		
}

void CMatrix4::MatRotateZ(CMatrix4 &mat, const float fAngleZ)
{
	const float c = cos(fAngleZ);
	const float s = sin(fAngleZ);
	
	mat._11 = c;	mat._12 = s;
	mat._21 = -s;	mat._22 = c;

	mat._13 = mat._14 = mat._23 = mat._24 = mat._31 = mat._32 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
	mat._33 = mat._44 = 1.0f;		
}

// Rotation about an arbitrary axis
void CMatrix4::MatRotate( CMatrix4 &mat, const float fAngle, const CVector3 &vecAxisOfRotation)
{
	float x = vecAxisOfRotation.x, y = vecAxisOfRotation.y, z = vecAxisOfRotation.z;
	float length = powf( x * x + y * y + z * z, 0.5f );
	if( fabsf(length) > 1e-6f )
	{
		float fInvLength = 1 / length;
		x *= fInvLength;
		y *= fInvLength;
		z *= fInvLength;
	}
	const float c = cos( fAngle );
	const float c1 = 1 - c;
	const float s = sin( fAngle );

	mat._11 = c + c1 * x * x;			mat._12 = c1 * x * y + s * z;	mat._13 = c1 * x * z - s * y;
	mat._21 = c1 * x * y - s * z;		mat._22 = c + c1 * y * y;		mat._23 = c1 * y * z + s * x;
	mat._31 = c1 * x * z + s * y;		mat._32 = c1 * y * z - s * x;	mat._33 = c + c1 * z * z;

	mat._14 = mat._24 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
	mat._44 = 1.0f;		
}

// Rotation about an arbitrary axis
void CMatrix4::MatRotate( CMatrix4 &mat, const float fAngle, const float fX, const float fY, const float fZ )
{
	float x = fX, y = fY, z = fZ;
	float length = powf( x * x + y * y + z * z, 0.5f );
	if( fabsf(length) > 1e-6f )
	{
		x /= length;
		y /= length;
		z /= length;
	}
	const float c = cos( fAngle );
	const float c1 = 1 - c;
	const float s = sin( fAngle );

	mat._11 = c + c1 * x * x;			mat._12 = c1 * x * y + s * z;	mat._13 = c1 * x * z - s * y;
	mat._21 = c1 * x * y - s * z;		mat._22 = c + c1 * y * y;		mat._23 = c1 * y * z + s * x;
	mat._31 = c1 * x * z + s * y;		mat._32 = c1 * y * z - s * x;	mat._33 = c + c1 * z * z;

	mat._14 = mat._24 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
	mat._44 = 1.0f;		
}

void CMatrix4::MatScale( CMatrix4 &mat, const float fX, const float fY, const float fZ )
{
	memset( (void *)&mat, 0, sizeof(CMatrix4) );
	mat._11 = fX;	mat._22 = fY;	mat._33 = fZ;	mat._44 = 1;	
}

void CMatrix4::MatTranslate(CMatrix4 &mat, const float fX, const float fY, const float fZ)
{
	memset((void *)&mat, 0, sizeof(CMatrix4));
	mat._41 = fX;
	mat._42 = fY;
	mat._43 = fZ;
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;

}

CMatrix4 CMatrix4::operator + (const CMatrix4 &a) const
{
	return CMatrix4(_11 + a._11, _12 + a._12, _13 + a._13, _14 + a._14,
					_21 + a._21, _22 + a._22, _23 + a._23, _24 + a._24,
					_31 + a._31, _32 + a._32, _33 + a._33, _34 + a._34,
					_41 + a._41, _42 + a._42, _43 + a._43, _44 + a._44);
}

CMatrix4 CMatrix4::operator - (const CMatrix4 &a) const
{
	return CMatrix4(_11 - a._11, _12 - a._12, _13 - a._13, _14 - a._14,
					_21 - a._21, _22 - a._22, _23 - a._23, _24 - a._24,
					_31 - a._31, _32 - a._32, _33 - a._33, _34 - a._34,
					_41 - a._41, _42 - a._42, _43 - a._43, _44 - a._44);
}

CMatrix4 CMatrix4::operator * (const float fValue) const
{
	return CMatrix4(_11 * fValue, _12 * fValue, _13 * fValue, _14 * fValue,
					_21 * fValue, _22 * fValue, _23 * fValue, _24 * fValue,
					_31 * fValue, _32 * fValue, _33 * fValue, _34 * fValue,
					_41 * fValue, _42 * fValue, _43 * fValue, _44 * fValue);
}

void CMatrix4::Multiply(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2)
{
	matResult._11 = mat1._11 * mat2._11 + mat1._12 * mat2._21 + mat1._13 * mat2._31 + mat1._14 * mat2._41;
	matResult._12 = mat1._11 * mat2._12 + mat1._12 * mat2._22 + mat1._13 * mat2._32 + mat1._14 * mat2._42;
	matResult._13 = mat1._11 * mat2._13 + mat1._12 * mat2._23 + mat1._13 * mat2._33 + mat1._14 * mat2._43;
	matResult._14 = mat1._11 * mat2._14 + mat1._12 * mat2._24 + mat1._13 * mat2._34 + mat1._14 * mat2._44;

	matResult._21 = mat1._21 * mat2._11 + mat1._22 * mat2._21 + mat1._23 * mat2._31 + mat1._24 * mat2._41;
	matResult._22 = mat1._21 * mat2._12 + mat1._22 * mat2._22 + mat1._23 * mat2._32 + mat1._24 * mat2._42;
	matResult._23 = mat1._21 * mat2._13 + mat1._22 * mat2._23 + mat1._23 * mat2._33 + mat1._24 * mat2._43;
	matResult._24 = mat1._21 * mat2._14 + mat1._22 * mat2._24 + mat1._23 * mat2._34 + mat1._24 * mat2._44;

	matResult._31 = mat1._31 * mat2._11 + mat1._32 * mat2._21 + mat1._33 * mat2._31 + mat1._34 * mat2._41;
	matResult._32 = mat1._31 * mat2._12 + mat1._32 * mat2._22 + mat1._33 * mat2._32 + mat1._34 * mat2._42;
	matResult._33 = mat1._31 * mat2._13 + mat1._32 * mat2._23 + mat1._33 * mat2._33 + mat1._34 * mat2._43;
	matResult._34 = mat1._31 * mat2._14 + mat1._32 * mat2._24 + mat1._33 * mat2._34 + mat1._34 * mat2._44;

	matResult._41 = mat1._41 * mat2._11 + mat1._42 * mat2._21 + mat1._43 * mat2._31 + mat1._44 * mat2._41;
	matResult._42 = mat1._41 * mat2._12 + mat1._42 * mat2._22 + mat1._43 * mat2._32 + mat1._44 * mat2._42;
	matResult._43 = mat1._41 * mat2._13 + mat1._42 * mat2._23 + mat1._43 * mat2._33 + mat1._44 * mat2._43;
	matResult._44 = mat1._41 * mat2._14 + mat1._42 * mat2._24 + mat1._43 * mat2._34 + mat1._44 * mat2._44;	
}

void CMatrix4::Add(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2)
{
	for(register int i = 0; i < 4; i++)
		for(register int j = 0; j < 4; j++)
		{
			matResult.mat[i][j] = mat1.mat[i][j] + mat2.mat[i][j];
		}

}

void CMatrix4::Subtract(CMatrix4 &matResult, const CMatrix4 &mat1, const CMatrix4 &mat2)
{
	for(register int i = 0; i < 4; i++)
		for(register int j = 0; j < 4; j++)
		{
			matResult.mat[i][j] = mat1.mat[i][j] - mat2.mat[i][j];
		}
}

void CMatrix4::Multiply(CMatrix4 &matResult, const CMatrix4 &mat1, const float fScale)
{
	for(register int i = 0; i < 4; i++)
		for(register int j = 0; j < 4; j++)
		{
			matResult.mat[i][j] = mat1.mat[i][j] * fScale;
		}
}

void CMatrix4::Transpose(CMatrix4 &matResult, const CMatrix4 &a)
{
	matResult._11 = a._11; matResult._12 = a._21; matResult._13 = a._31; matResult._14 = a._41;
	matResult._21 = a._12; matResult._22 = a._22; matResult._23 = a._32; matResult._24 = a._42;
	matResult._31 = a._13; matResult._32 = a._23; matResult._33 = a._33; matResult._34 = a._43;
	matResult._41 = a._14; matResult._42 = a._24; matResult._43 = a._34; matResult._44 = a._44;
}

#define _decl(t1, t2, t3) float _##t1##t2##t3 = a._##t1 * a._##t2 * a._##t3
#define _term(t1, t2, t3) (a._##t1 * a._##t2 * a._##t3)

float CMatrix4::Determinant(const CMatrix4 &a)
{
	return	a._11 * ( _term(22,33,44) + _term(23,34,42) + _term(24,32,43)) + 						
			a._12 * ( _term(21,34,43) + _term(23,31,44) + _term(24,33,41)) +
			a._13 * ( _term(21,32,44) + _term(22,34,41) + _term(24,31,42)) +
			a._14 * ( _term(21,33,42) + _term(22,31,43) + _term(23,32,41)) -
			a._11 * ( _term(22,34,43) + _term(23,32,44) + _term(24,33,42)) -
			a._12 * ( _term(21,33,44) + _term(23,34,41) + _term(24,31,43)) - 
			a._13 * ( _term(21,34,42) + _term(22,31,44) + _term(24,32,41)) -
			a._14 * ( _term(21,32,43) + _term(22,33,41) + _term(23,31,42));
}

void CMatrix4::Inverse(CMatrix4 &mat, const CMatrix4 &a)
{
	_decl(21,32,43); _decl(21,32,44); _decl(21,33,42); _decl(21,33,44);	_decl(21,34,42); _decl(21,34,43);
	_decl(22,31,43); _decl(22,31,44); _decl(22,33,41); _decl(22,33,44); _decl(22,34,41); _decl(22,34,43);
	_decl(23,31,42); _decl(23,31,44); _decl(23,32,41); _decl(23,32,44); _decl(23,34,41); _decl(23,34,42);
	_decl(24,31,42); _decl(24,31,43); _decl(24,32,41); _decl(24,32,43); _decl(24,33,41); _decl(24,33,42);

	const float fDetA =	+ a._11 * ( _223344 + _233442 + _243243)				
					+ a._12 * ( _213443 + _233144 + _243341)
					+ a._13 * ( _213244 + _223441 + _243142)
					+ a._14 * ( _213342 + _223143 + _233241) 
					- a._11 * ( _223443 + _233244 + _243342)
					- a._12 * ( _213344 + _233441 + _243143)
					- a._13 * ( _213442 + _223144 + _243241)
					- a._14 * ( _213243 + _223341 + _233142);

	if(fabsf(fDetA) >= 1e-8f)
	{
		const float fInvDetA = 1.0f / fDetA;
		mat._11 = (_223344  + _233442 + _243243 - _223443 - _233244 - _243342) * fInvDetA;
		mat._12 = (_term(12,34,43) + _term(13,32,44) + _term(14,33,42) 
						- _term(12,33,44) - _term(13,34,42) - _term(14,32,43)) * fInvDetA;					
		mat._13 = (_term(12,23,44) + _term(13,24,42) + _term(14,22,43) 
						- _term(12,24,43) - _term(13,22,44) - _term(14,23,42)) * fInvDetA;
		mat._14 = (_term(12,24,33) + _term(13,22,34) + _term(14,23,32) 
						- _term(12,23,34) - _term(13,24,32) - _term(14,12,33)) * fInvDetA;

		mat._21 = (_213443 + _233144 + _243341 - _213344 - _233441 - _243143) * fInvDetA;				
		mat._22 = (_term(11,33,44) + _term(13,34,41) + _term(14,31,43) 
						- _term(11,34,43) - _term(13,31,44) - _term(14,33,41)) * fInvDetA;
		mat._23 = (_term(11,24,43) + _term(13,21,44) + _term(14,23,41) 
						- _term(11,23,44) - _term(13,24,41) - _term(14,21,43)) * fInvDetA;
		mat._24 = (_term(11,23,34) + _term(13,24,31) + _term(14,21,33) 
						- _term(11,24,33) - _term(13,21,34) - _term(14,23,31)) * fInvDetA;

		mat._31 = (_213244 + _223441 + _243142 - _213442 - _223144 - _243241) * fInvDetA;
		mat._32 = (_term(11,34,42) + _term(12,31,44) + _term(14,32,41) 
						- _term(11,32,44) - _term(12,34,41) - _term(14,31,42)) * fInvDetA;
		mat._33 = (_term(11,22,44) + _term(12,24,41) + _term(14,21,42) 
						- _term(11,24,42) - _term(12,21,44) - _term(14,22,41)) * fInvDetA;
		mat._34 = (_term(11,24,32) + _term(12,21,34) + _term(14,22,31) 
						- _term(11,22,34) - _term(12,24,31) - _term(14,21,32)) * fInvDetA;

		mat._41 = (_213342 + _223143 + _233241 - _213243 - _223341 - _233142) * fInvDetA;
		mat._42 = (_term(11,32,43) + _term(12,33,41) + _term(13,31,42) 
						- _term(11,33,42) - _term(12,31,43) - _term(14,32,41)) * fInvDetA;
		mat._43 = (_term(11,23,42) + _term(12,21,43) + _term(13,22,41) 
						- _term(11,22,43) - _term(12,23,41) - _term(13,21,42)) * fInvDetA;
		mat._44 = (_term(11,22,33) + _term(12,23,31) + _term(13,21,32) 
						- _term(11,23,32) - _term(12,21,33) - _term(13,22,31)) * fInvDetA;
	}
	else
	{
		memset((void *)&mat, 0, sizeof(CMatrix4));
	}
}

#undef _decl
#undef _term

void CMatrix4::MatMouseRotation( CMatrix4 &matResult, unsigned int startx, unsigned int starty, 
								unsigned int endx, unsigned int endy, 
								float centerx, float centery, float fRadius )
{	
	// Shoemake in Graphic Gems IV, p. 176
	CVector3 vecStart, vecEnd;	// start and end coordinates on imaginary sphere

	float modulus;
	float fInvRadius = 1.0f / fRadius;

	vecStart.x = (float)( startx - centerx ) * fInvRadius;
	vecStart.y = -(float)( starty - centery ) * fInvRadius;

	modulus = vecStart.x * vecStart.x + vecStart.y * vecStart.y;

	if (modulus > 1.0f)    	// outside radius!
	{
		CMatrix4::MatIdentity( matResult );
		return;
	}

	vecStart.z = sqrtf( 1.0f - modulus );

	vecEnd.x = (float)( endx - centerx ) * fInvRadius;
	vecEnd.y = -(float)( endy - centery ) * fInvRadius;

	modulus = vecEnd.x * vecEnd.x + vecEnd.y * vecEnd.y;

	if ( modulus > 1.0f )    	// outside radius!
	{
		CMatrix4::MatIdentity( matResult );
		return;
	}

	vecEnd.z = sqrtf( 1.0f - modulus );

	CVector3 vecCross;
	float fDot, fAngle;

	fDot = CVector3::Dot( vecStart, vecEnd );

	if ( fDot == 1.0 ) 
	{
		// Nothing to do
		CMatrix4::MatIdentity(matResult);
		return;	
	}

	CVector3::Cross( vecCross, vecStart, vecEnd); // axis of rotation

	fAngle = 2.0f * acosf( fDot ); // angle of rotation

	CMatrix4::MatRotate( matResult, fAngle, vecCross);		
}

void CMatrix4::SetRow(const CVector3 &vecRow, const unsigned int i)
{
	mat[i][0] = vecRow.x;
	mat[i][1] = vecRow.y;
	mat[i][2] = vecRow.z;
}

void CMatrix4::SetColumn(const CVector3 &vecColumn, const unsigned int i)
{
	mat[0][i] = vecColumn.x;
	mat[1][i] = vecColumn.y;
	mat[2][i] = vecColumn.z;
}


void CMatrix4::GetRow(CVector3 &vecRow, const unsigned int i)
{		
	vecRow.x = mat[i][0];
	vecRow.y = mat[i][1];
	vecRow.z = mat[i][2];	
}

void CMatrix4::GetColumn(CVector3 &vecColumn, const unsigned int i)
{		
	vecColumn.x = mat[0][i];
	vecColumn.y = mat[1][i];
	vecColumn.z = mat[2][i];	
}

}