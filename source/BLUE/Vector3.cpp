#include "stdafx.h"
#include <cstring>
#include <cmath>

#include "Vector3.h"

namespace BLUE
{

CVector3::CVector3( void ) : x( 0.0f ), y( 0.0f ), z( 0.0f ) { }
CVector3::CVector3( float *vfCoords ) : x( vfCoords[0] ), y( vfCoords[1] ), z( vfCoords[2] ) { }
CVector3::CVector3( const float fX, const float fY, const float fZ ) : x( fX ), y( fY ), z( fZ ) { }
CVector3::~CVector3( void ) { }

CVector3::operator float *()
{
	return (float *) &x;
}


void CVector3::Copy(CVector3 &vecDest, const CVector3 &vecSrc)
{
	memcpy_s((void *)&vecDest, sizeof(CVector3), (void*)&vecSrc, sizeof(CVector3));
}

float CVector3::Dot(const CVector3 &vec1, const CVector3 &vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

void CVector3::Cross(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2)
{
	vecResult = CVector3(vec1.y * vec2.z - vec1.z * vec2.y,
		vec1.z * vec2.x - vec1.x * vec2.z,
		vec1.x * vec2.y - vec1.y * vec2.z);
	
}

void CVector3::Add(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2)
{
	vecResult.x = vec1.x + vec2.x;
	vecResult.y = vec1.y + vec2.y;
	vecResult.z = vec1.z + vec2.z;
}

void CVector3::Subtract(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2)
{
	vecResult.x = vec1.x - vec2.x;
	vecResult.y = vec1.y - vec2.y;
	vecResult.z = vec1.z - vec2.z;
}

void CVector3::MAdd(CVector3 &vecResult, const CVector3 &vec1, const CVector3 &vec2, const float fScale)
{
	vecResult.x = vec1.x + vec2.x * fScale;
	vecResult.y = vec1.y + vec2.y * fScale;
	vecResult.z = vec1.z + vec2.z * fScale;
}

void CVector3::Transform(CVector3 &vecResult, const CVector3 &vec, const CMatrix4 &mat)
{	
	float fNormalize = 1.0f / (vec.x * mat._14 + vec.y * mat._24 + vec.z * mat._34 + mat._44);
	float fX = ( vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + mat._41 ) * fNormalize;
	float fY = ( vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + mat._42 ) * fNormalize;
	float fZ = ( vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + mat._43 ) * fNormalize;
	vecResult.x = fX;
	vecResult.y = fY;
	vecResult.z = fZ;
}

float CVector3::GetLength()
{
	return powf(x * x + y * y + z * z, 0.5f);
}

float CVector3::Normalize(CVector3 &vecResult, const CVector3 &vec)
{
	float length = powf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z, 0.5f);
	float invLength = 1.0f / length;
	vecResult.x = vec.x * invLength;
	vecResult.y = vec.y * invLength;
	vecResult.z = vec.z * invLength;
	return length;
}

void CVector3::Maximize(CVector3 &vecMax, const CVector3 &vec)
{
	if(vecMax.x < vec.x)
		vecMax.x = vec.x;
	if(vecMax.y < vec.y)
		vecMax.y = vec.y;
	if(vecMax.z < vec.z)
		vecMax.z = vec.z;
}

void CVector3::Minimize(CVector3 &vecMin, const CVector3 &vec)
{
	if(vecMin.x > vec.x)
		vecMin.x = vec.x;
	if(vecMin.y > vec.y)
		vecMin.y = vec.y;
	if(vecMin.z > vec.z)
		vecMin.z = vec.z;

}

bool CVector3::operator==(const CVector3 &vec)
{
	return x == vec.x && y == vec.y && z == vec.z;
}

bool CVector3::operator!=(const CVector3 &vec)
{
	return x != vec.x || y != vec.y || z != vec.z;
}

std::ostream &operator <<(std::ostream &os, const CVector3 &vec)
{
	os << vec.x << vec.y << vec.z;
	return os;
}

std::istream &operator >>(std::istream &is, CVector3 &vec)
{
	is >> vec.x >> vec.y >> vec.z;
	return is;
}

}