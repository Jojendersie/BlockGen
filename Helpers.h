#pragma once

// ******************************************************************************** //
// Constants
const float fPi = 3.14159265358979f;
const double dPi = 3.1415926535897932384626433832795;
const float f2Pi = fPi*2.0f;
const double d2Pi = dPi*2.0;
const float fPi_2 = fPi*0.5f;
const double dPi_2 = dPi*0.5;
const float fE = 2.71828182845904f;
const double dE  = 2.71828182845904523536028747135266249;



// ******************************************************************************** //
template<typename T> T lerp( T a, T b, T t )				{ return a+(b-a)*t; }
template<typename T> T clamp( T a, T min, T max )			{ return a<min? min : a>max? max : a; }
__forceinline int Floor(const float a)						{int r=(int)a; return r - (int)((a<0)&&(a-r!=0.0f));}		// Round down
__forceinline int Ceil(const float a)						{int r=(int)a; return r + (int)((a>0)&&(a-r!=0.0f));}		// Round up
__forceinline int Round(const float a)						{return Ceil(a+0.5f);}										// Round