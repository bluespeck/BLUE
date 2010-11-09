#include "stdafx.h"
#include <cstring>
#include <cmath>

#include "Vector2.h"

namespace BLUE
{

CVector2::CVector2( void ) : x( 0.0f ), y( 0.0f ) { }
CVector2::CVector2( float *vfCoords ) : x( vfCoords[0] ), y( vfCoords[1] ) { }
CVector2::CVector2(const float fX, const float fY ) : x( fX ), y( fY ) { }
CVector2::~CVector2( void ) { }

CVector2::operator float *()
{
	return (float *) &x;
}


void CVector2::Copy(CVector2 &vecDest, const CVector2 &vecSrc)
{
	memcpy_s((void *)&vecDest, sizeof(CVector2), (void*)&vecSrc, sizeof(CVector2));
}

float CVector2::DotProduct(const CVector2 &vec1, const CVector2 &vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

void CVector2::Add(CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2)
{
	vecResult.x = vec1.x + vec2.x;
	vecResult.y = vec1.y + vec2.y;
}

void CVector2::Subtract(CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2)
{
	vecResult.x = vec1.x - vec2.x;
	vecResult.y = vec1.y - vec2.y;
}

void CVector2::Multiply(CVector2 &vecResult, const CVector2 &vec, float fScale)
{
	vecResult.x = vec.x * fScale;
	vecResult.y = vec.y * fScale;
}

void CVector2::MAdd(CVector2 &vecResult, const CVector2 &vec1, const CVector2 &vec2, float fScale)
{
	vecResult.x = vec1.x + vec2.x * fScale;
	vecResult.y = vec1.y + vec2.y * fScale;
}

void CVector2::Transform(CVector2 &vecResult, const CVector2 &vec, const CMatrix4 &mat)
{	
	float fNormalize = 1.0f / (vec.x * mat._14 + vec.y * mat._24 + mat._44);
	float fX = ( vec.x * mat._11 + vec.y * mat._21 + mat._41 ) * fNormalize;
	float fY = ( vec.x * mat._12 + vec.y * mat._22 + mat._42 ) * fNormalize;
	vecResult.x = fX;
	vecResult.y = fY;
}

float CVector2::GetLength()
{
	return powf(x * x + y * y, 0.5f);
}

float CVector2::Normalize(CVector2 &vecResult, const CVector2 &vec)
{
	float length = powf(vec.x * vec.x + vec.y * vec.y, 0.5f);
	float invLength = 1.0f / length;
	vecResult.x = vec.x * invLength;
	vecResult.y = vec.y * invLength;
	return length;
}

void CVector2::Maximize(CVector2 &vecMax, const CVector2 &vec)
{
	if(vecMax.x < vec.x)
		vecMax.x = vec.x;
	if(vecMax.y < vec.y)
		vecMax.y = vec.y;	
}

void CVector2::Minimize(CVector2 &vecMin, const CVector2 &vec)
{
	if(vecMin.x > vec.x)
		vecMin.x = vec.x;
	if(vecMin.y > vec.y)
		vecMin.y = vec.y;
}

bool CVector2::operator==(const CVector2 &vec)
{
	return x == vec.x && y == vec.y;
}

bool CVector2::operator!=(const CVector2 &vec)
{
	return x != vec.x || y != vec.y;
}

std::ostream &operator <<(std::ostream &os, const CVector2 &vec)
{
	os << vec.x << vec.y;
	return os;
}

std::istream &operator >>(std::istream &is, CVector2 &vec)
{
	is >> vec.x >> vec.y;
	return is;
}

}