// ******************************************************************************** //
// OrTypeDef.h																		//
// ===========																		//
// This file is part of the OrBaseLib.												//
//																					//
// Author: Johannes Jendersie														//
//																					//
// Here is a quite easy licensing as open source:									//
// http://creativecommons.org/licenses/by/3.0/										//
// If you use parts of this project, please let me know what the purpose of your	//
// project is. You can do this by writing a comment at github.com/Jojendersie/.		//
//																					//
// For details on this project see: Readme.txt										//
// ******************************************************************************** //
//	Declarations of all "primitive" types											//
// ******************************************************************************** //

#pragma once

#include <limits.h>
#include <cmath>
#include <algorithm>
#include "Helpers.h"
#undef min
#undef max

#if(UCHAR_MAX == 0xff)
	typedef unsigned char uint8;
	typedef unsigned char byte;
#else
	#error "sizeof(char) is not 1"
#endif

#if(SCHAR_MAX == 0x7f)
	typedef signed char int8;
#else
	#error "sizeof(char) is not 1"
#endif

#if(USHRT_MAX == 0xffff)
	typedef unsigned short uint16;
#else
	#if(UINT_MAX == 0xffff)
		typedef unsigned int uint16;
	#else
		#error "cannot support 16 bit unsigned integer on this plattform"
	#endif
#endif

#if(SHRT_MAX == 0x7fff)
	typedef signed short int16;
#else
	#if(INT_MAX == 0x7fff)
		typedef signed int int16;
	#else
		#error "cannot support 16 bit signed integer on this plattform"
	#endif
#endif

#if(USHRT_MAX == 0xffffffff)
	typedef unsigned short uint32;
#else
	#if(UINT_MAX == 0xffffffff)
		typedef unsigned int uint32;
	#else
		#if(ULONG_MAX == 0xffffffff)
			typedef unsigned long uint32;
		#else
			#error "cannot support 32 bit unsigned integer on this plattform"
		#endif
	#endif
#endif

#if(SHRT_MAX == 0x7fffffff)
	typedef signed short int32;
#else
	#if(INT_MAX == 0x7fffffff)
		typedef signed int int32;
		#else
			#if(LONG_MAX == 0x7fffffff)
			typedef signed long int32;
		#else
			#error "cannot support 32 bit signed integer on this plattform"
		#endif
	#endif
#endif

#if(ULLONG_MAX == 0xffffffffffffffff)
	typedef unsigned long long uint64;
#else
	#if(UINT_MAX == 0xffffffffffffffff)
		typedef unsigned int uint64;
	#else
		#if(ULONG_MAX == 0xffffffffffffffff)
			typedef unsigned long uint64;
		#else
			#error "cannot support 64 bit unsigned integer on this plattform"
		#endif
	#endif
#endif

#if(LLONG_MAX == 0x7fffffffffffffff)
	typedef signed long long int64;
#else
	#if(INT_MAX == 0x7fffffffffffffff)
		typedef signed int int64;
		#else
			#if(LONG_MAX == 0x7fffffffffffffff)
			typedef signed long int64;
		#else
			#error "cannot support 64 bit signed integer on this plattform"
		#endif
	#endif
#endif


typedef unsigned int uint;

// ******************************************************************************** //
// An integral type which has always the size of (void*)
template <int BYTES> struct PtrToInt;
template <> struct PtrToInt<2> {typedef uint16 int_t;};
template <> struct PtrToInt<4> {typedef uint32 int_t;};
template <> struct PtrToInt<8> {typedef uint64 int_t;};
typedef PtrToInt<sizeof(void*)>::int_t uiptr;




// ******************************************************************************** //
// The class for 3D vectors (element of R^3)
class Vec3
{
public:
	// Variables
	union
	{
		struct
		{
			float x;	// Coordinates
			float y;
			float z;
		};

		struct
		{
			float u;	// Nomenclature for texture coordinates
			float v;
			float w;
		};

		float c[3];		// Array interpretation from the vector
	};

	// Constructors
	Vec3()																						{}
	Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z)												{}
	Vec3(const float f) : x(f), y(f), z(f)														{}
	Vec3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z)					{}
	Vec3(const float* pfComponent) : x(pfComponent[0]), y(pfComponent[1]), z(pfComponent[2])	{}

	// Casting-operators
	operator float* ()												{return (float*)(c);}
	operator const float* () const									{return (const float*)(c);}

	// Casting to "color" with alpha channel==255
	operator uint32 () const										{return (byte(clamp(x*255.0, 0.0, 255.0))<<24) | (byte(clamp(y*255.0, 0.0, 255.0))<<16) | (byte(clamp(z*255.0, 0.0, 255.0))<<8) | 255;}

	// Assignment operators
	Vec3& operator = (const Vec3& v)								{x = v.x; y = v.y; z = v.z; return *this;}
	Vec3& operator = (const float f)								{x = f; y = f; z = f; return *this;}
	Vec3& operator += (const Vec3& v)								{x += v.x; y += v.y; z += v.z; return *this;}
	Vec3& operator -= (const Vec3& v)								{x -= v.x; y -= v.y; z -= v.z; return *this;}
	Vec3& operator *= (const Vec3& v)								{x *= v.x; y *= v.y; z *= v.z; return *this;}
	Vec3& operator *= (const float f)								{x *= f; y *= f; z *= f; return *this;}
	Vec3& operator /= (const Vec3& v)								{x /= v.x; y /= v.y; z /= v.z; return *this;}
	Vec3& operator /= (float f)										{f = 1/f; x *= f; y *= f; z *= f; return *this;}

	// ******************************************************************************** //
	// Arithmetic operators
	inline Vec3 operator + (const Vec3& b) const					{return Vec3(x + b.x, y + b.y, z + b.z);}
	inline Vec3 operator - (const Vec3& b) const					{return Vec3(x - b.x, y - b.y, z - b.z);}
	inline Vec3 operator - () const									{return Vec3(-x, -y, -z);}
	inline Vec3 operator * (const Vec3& b) const					{return Vec3(x*b.x, y*b.y, z*b.z);}
	inline Vec3 operator * (const float f) const					{return Vec3(x * f, y * f, z * f);}
	inline Vec3 operator / (const Vec3& b) const					{return Vec3(x / b.x, y / b.y, z / b.z);}
	inline Vec3 operator / (float f) const							{f = 1/f; return Vec3(x * f, y * f, z * f);}

	// ******************************************************************************** //
	// Comparison operators
	#define Vector_EPSILON	0.00001f
	inline bool operator == (const Vec3& b) const					{if(abs(x - b.x) > Vector_EPSILON) return false; if(abs(y - b.y) > Vector_EPSILON) return false; return abs(z - b.z) < Vector_EPSILON;}
	inline bool operator != (const Vec3& b) const					{if(abs(x - b.x) > Vector_EPSILON) return true; if(abs(y - b.y) > Vector_EPSILON) return true; return abs(z - b.z) > Vector_EPSILON;}

	// ******************************************************************************** //
	// Vector3 functions
	// The non-static and non-const functions always change the calling object
	inline float		Length() const								{return sqrt(x * x + y * y + z * z);}
	inline float		LengthSq() const							{return x * x + y * y + z * z;}
	inline float		LengthInv() const							{return 1.0f/sqrt(x * x + y * y + z * z);}
	inline const Vec3&	Normalize()									{*this *= 1.0f/sqrt(x * x + y * y + z * z); return *this;}
	inline static Vec3	Normalize(const Vec3& v)					{return v * 1.0f/sqrt(v.x * v.x + v.y * v.y + v.z * v.z);}
	inline const Vec3&	NormalizeEx()								{*this *= std::min(1.0f/sqrt(x * x + y * y + z * z), 100000000000000000.0f); return *this;}
	inline static Vec3	NormalizeEx(const Vec3& v)					{float fL = 1.0f/sqrt(v.x * v.x + v.y * v.y + v.z * v.z); return v * std::min(fL, 100000000000000000.0f);}
	inline Vec3			Cross(const Vec3& v) const					{return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);}
	inline static Vec3	Cross(const Vec3& v1, const Vec3& v2)		{return Vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
	inline float		Dot(const Vec3& v) const					{return x * v.x + y * v.y + z * v.z;}
	inline float		Angle(const Vec3& v) const					{return acos((x * v.x + y * v.y + z * v.z) * 1.0f/sqrt((x * x + y * y + z * z) * (v.x * v.x + v.y * v.y + v.z * v.z)));}

	// Rotate a vector around some axis. This transformation uses quaternion
	// math. Use this if you want to rotate only one vector. For the rotation
	// of multiple vectors a matrix is more efficient!
	// Speed: Debug 480, Release 176
	Vec3			Rotate(const Vec3& vAxis, float fAngle) const;
};

// ******************************************************************************** //
inline Vec3 operator * (const float f, const Vec3& v)				{return Vec3(v.x * f, v.y * f, v.z * f);}

// ******************************************************************************** //
// Lerp, Abs, Min and Max have to be in global scope to override the default template
inline Vec3	Min(const Vec3& v1, const Vec3& v2)						{return Vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));}
inline Vec3	Max(const Vec3& v1, const Vec3& v2)						{return Vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));}
inline Vec3	Abs(const Vec3& v1)										{return Vec3(abs(v1.x), abs(v1.y), abs(v1.z));}
inline Vec3	Lerp(const Vec3& v1, const Vec3& v2, const float t)		{return Vec3(v1.x+t*(v2.x-v1.x), v1.y+t*(v2.y-v1.y), v1.z+t*(v2.z-v1.z));}


// *************************************EOF**************************************** //