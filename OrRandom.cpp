// ******************************************************************************** //
// OrRandom.cpp																		//
// ============																		//
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
// Implementation of different pseudo-random generators.								//
//																					//
// ******************************************************************************** //

#include "OrTypeDef.h"
#include "OrRandom.h"
#include "Helpers.h"
//#include "../include/OrVector2.h"
//#include "../include/OrVector3.h"

using namespace OrE::Algorithm;

// ******************************************************************************** //
// Internal numbers of the random number generator - Mersenne Twister
const int       OR_MT_W		=	34;
const int       OR_MT_N		=	624;
const int       OR_MT_M		=	397;
const int       OR_MT_R		=	31;
const unsigned	OR_MT_A[2]	=	{ 0, 0x9908B0DF };	
const int       OR_MT_U		=	11;
const int       OR_MT_S		=	7;
const unsigned	OR_MT_B		=	0x9D2C5680;
const int       OR_MT_T		=	15;
const unsigned	OR_MT_C		=	 0xEFC60000;
const int       OR_MT_L		=	18;
const unsigned	OR_MT_LLMASK=	0x7FFFFFFF;
const unsigned	OR_MT_UMASK	=	0x80000000;

uint32 g_adwMT[OR_MT_N];
uint32 g_dwMTIndex = 0;

// ******************************************************************************** //
// The intrinsic generator: refills the table with new numbers
void OrMTReCreate()
{
	uint32 y;

	g_dwMTIndex = 0;

	for (int k=0 ; k<OR_MT_N-OR_MT_M ; ++k)
	{
		y = (g_adwMT[k] & OR_MT_UMASK) | (g_adwMT[k+1] & OR_MT_LLMASK);
		g_adwMT[k] = g_adwMT[k+OR_MT_M] ^ (y >> 1) ^ OR_MT_A[y & 1];
	}
	for (int k=OR_MT_N-OR_MT_M ; k<OR_MT_N-1 ; ++k) {
		y = (g_adwMT[k] & OR_MT_UMASK) | (g_adwMT[k+1] & OR_MT_LLMASK);
		g_adwMT[k] = g_adwMT[k+(OR_MT_M-OR_MT_N)] ^ (y >> 1) ^ OR_MT_A[y & 1];
	}
	y = (g_adwMT[OR_MT_N-1] & OR_MT_UMASK) | (g_adwMT[0] & OR_MT_LLMASK);
	g_adwMT[OR_MT_N-1] = g_adwMT[OR_MT_M-1] ^ (y >> 1) ^ OR_MT_A[y & 1];
}

// ******************************************************************************** //
// Initialize Mersenne Twister
void OrE::Algorithm::SRand(uint32 _dwSeed)
{
	// fill table
	for(int i=0; i<OR_MT_N; i++)
	{
		g_adwMT[i] = i%2?_dwSeed+i*527:(2135+_dwSeed*74111)*i;
	}

	OrMTReCreate();
	g_dwMTIndex = OR_MT_N;
}

// ******************************************************************************** //
// Creates an unsigned random Number
uint32 OrE::Algorithm::RandU()
{
	uint32 y;

	if(g_dwMTIndex == OR_MT_N)
		OrMTReCreate();


	y = g_adwMT[g_dwMTIndex++];
	y ^= y >> OR_MT_U;
	y ^= y << OR_MT_S & OR_MT_B;
	y ^= y << OR_MT_T & OR_MT_C;
	y ^= y >> OR_MT_L;

	return y;
}

// ******************************************************************************** //
// Creates a integral random number between _iMin and _iMax (inclusive)
int	OrE::Algorithm::Rand(int _iMin, int _iMax)
{
	uint32 y;

	if(g_dwMTIndex == OR_MT_N)
		OrMTReCreate();


	y = g_adwMT[g_dwMTIndex++];
	y ^=y >> OR_MT_U;
	y ^=y << OR_MT_S & OR_MT_B;
	y ^=y << OR_MT_T & OR_MT_C;
	y ^=y >> OR_MT_L;

	// stint scope
	return (y % (_iMax + 1 - _iMin)) + _iMin;
}

// ******************************************************************************** //
// Creates a random number between 0 and 1 (inclusive)
float OrE::Algorithm::Rand()
{
	uint32 y;

	if(g_dwMTIndex == OR_MT_N)
		OrMTReCreate();

	y = g_adwMT[g_dwMTIndex++];
	y ^=y >> OR_MT_U;
	y ^=y << OR_MT_S & OR_MT_B;
	y ^=y << OR_MT_T & OR_MT_C;
	y ^=y >> OR_MT_L;

	// stint scope
	return (float)(y*(2.328306437e-10));
}

// ******************************************************************************** //
// Creates a random number of an exponential distribution [0,+Inf]
float OrE::Algorithm::ExpRand(float _fLambda)
{
	return float(-log(Rand()+1.0e-323)/double(_fLambda));
}	

// ******************************************************************************** //
// Creates a random number of a normal distribution [-Inf,+Inf]
float OrE::Algorithm::StdNormRand(float _fMean, float _fVariance)
{
	// Slow but stable, there are faster ways
	return float(_fMean + _fVariance*sqrt(std::max(0.0,-2.0*log(Rand()+1.0e-323))) * cos(d2Pi*Rand()));
}

// ******************************************************************************** //
// Creates a random number of a normal distribution [-Inf,+Inf]
float OrE::Algorithm::NormRand()
{
	// Slow but stable, there are faster ways
	return float(sqrt(std::max(0.0,-2.0*log(Rand()+1.0e-323))) * cos(d2Pi*Rand()));
}


// ******************************************************************************** //
OrE::Algorithm::PerlinNoise::PerlinNoise(uint32 _dwSeed, float _fPeriodicity)
{
	m_dwSeed = _dwSeed*1987;
	m_fPeriodicity = std::max(1.0f, _fPeriodicity);
	// Use Mersenne twister to seed a table for later random samplings
//	SRand(_dwSeed);
//	for(int i=0; i<1024; ++i)
		//m_dwWhiteNoiseTable[i] = RandU();
}

// ******************************************************************************** //
double OrE::Algorithm::PerlinNoise::Sample1D(__int64 _i)
{
	_i += m_dwSeed;
	_i ^= (_i<<13);
	return (((_i * (_i * _i * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483647.0);
}

// ******************************************************************** //
// PERLIN NOISE
// ******************************************************************** //

// Function for interpolation between two points of the noise
inline float InterpolationPolynom(float _dR)
{
	// Polynomial function to get detailed information see "Burger-GradientNoiseGerman-2008".
	return _dR*_dR*_dR*(_dR*(_dR*6.0f-15.0f)+10.0f);
}

inline float Derivative(float _dR)
{
	return _dR*_dR*(_dR*(_dR-2.0f)+1.0f)*30.0f;
}

inline void IntFrac(float _f, int& _iInt, float& _fFrac)	{ _iInt = Floor(_f); _fFrac = _f-_iInt; }

// ******************************************************************************** //
float OrE::Algorithm::PerlinNoise::Rand1D(float _fX, float _fFrequence)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 2 samples total
	float fFracX;
	int iX0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	iX0 = (iX0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;

	float s0 = (float)Sample1D(iX0);
	float s1 = (float)Sample1D(iX1);

	float u = InterpolationPolynom(fFracX);

	return lerp(s0, s1, u);
}

float OrE::Algorithm::PerlinNoise::Rand1D(float _fX, float _fFrequence, float& _vOutGrad)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 2 samples total
	float fFracX;
	int iX0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	iX0 = (iX0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;

	float s0 = (float)Sample1D(iX0);
	float s1 = (float)Sample1D(iX1);

	float u = InterpolationPolynom(fFracX);
	float du = Derivative(fFracX);

	// Calculate the gradient per component
	_vOutGrad = (s1-s0)*du;

	return lerp(s0, s1, u);
}

float OrE::Algorithm::PerlinNoise::Rand1D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX)
{
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		fRes += fAmplitude*Rand1D(_fX, fFrequence);

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}

	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

float OrE::Algorithm::PerlinNoise::Rand1D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float& _vOutGrad)
{
	_vOutGrad = 0.0f;
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		float vNormal;
		fRes += fAmplitude*Rand1D(_fX, fFrequence, vNormal);
		_vOutGrad += fFrequence*fAmplitude*vNormal;

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}

	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

// ******************************************************************************** //
float OrE::Algorithm::PerlinNoise::Rand2D(float _fX, float _fY, float _fFrequence)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 4 samples total
	float fFracX, fFracY;
	int iX0, iY0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	IntFrac(_fY*_fFrequence, iY0, fFracY);
	iX0 = (iX0+iPeriod)%iPeriod;
	iY0 = (iY0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;
	int iY1 = ((iY0+1)%iPeriod)*101; iY0*=101;

	float s00 = (float)Sample1D(iX0 + iY0);
	float s10 = (float)Sample1D(iX1 + iY0);
	float s01 = (float)Sample1D(iX0 + iY1);
	float s11 = (float)Sample1D(iX1 + iY1);

	float u = InterpolationPolynom(fFracX);
	float v = InterpolationPolynom(fFracY);

	return lerp(lerp(s00, s10, u), lerp(s01, s11, u), v);
}

float OrE::Algorithm::PerlinNoise::Rand2D(float _fX, float _fY, float _fFrequence, Vec3& _vOutGrad)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 4 samples total
	float fFracX, fFracY;
	int iX0, iY0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	IntFrac(_fY*_fFrequence, iY0, fFracY);
	iX0 = (iX0+iPeriod)%iPeriod;
	iY0 = (iY0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;
	int iY1 = ((iY0+1)%iPeriod)*101; iY0*=101;

	float s00 = (float)Sample1D(iX0 + iY0);
	float s10 = (float)Sample1D(iX1 + iY0);
	float s01 = (float)Sample1D(iX0 + iY1);
	float s11 = (float)Sample1D(iX1 + iY1);

	float u = InterpolationPolynom(fFracX);
	float v = InterpolationPolynom(fFracY);
	float du = Derivative(fFracX);
	float dv = Derivative(fFracY);

	float k1 = s10 - s00;
	float k2 = s01 - s00;

	_vOutGrad.x = (k1+(s11-s01-k1)*v)*du;
	_vOutGrad.y = (k2+(s11-s10-k2)*u)*dv;

	//	   lerp(lerp(s00, s10, u), lerp(s01, s11, u), v);
	return lerp(	 s00 + k1 * u, lerp(s01, s11, u), v);
}

float OrE::Algorithm::PerlinNoise::Rand2D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY)
{
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//OrE::Math::Pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		fRes += fAmplitude*Rand2D(_fX, _fY, fFrequence);

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}

	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

float OrE::Algorithm::PerlinNoise::Rand2D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, Vec3& _vOutGrad)
{
	_vOutGrad.x = _vOutGrad.y = 0.0f;
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//OrE::Math::Pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		Vec3 vNormal;
		fRes += fAmplitude*Rand2D(_fX, _fY, fFrequence, vNormal);
		_vOutGrad += fFrequence*fAmplitude*vNormal;

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}

	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

// ******************************************************************************** //
float OrE::Algorithm::PerlinNoise::Rand3D(float _fX, float _fY, float _fZ, float _fFrequence)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 8 samples total
	float fFracX, fFracY, fFracZ;
	int iX0, iY0, iZ0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	IntFrac(_fY*_fFrequence, iY0, fFracY);
	IntFrac(_fZ*_fFrequence, iZ0, fFracZ);
	iX0 = (iX0+iPeriod)%iPeriod;
	iY0 = (iY0+iPeriod)%iPeriod;
	iZ0 = (iZ0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;
	int iY1 = ((iY0+1)%iPeriod)*101; iY0*=101;
	int iZ1 = ((iZ0+1)%iPeriod)*307; iZ0*=307;

	float s000 = (float)Sample1D(iX0 + iY0 + iZ0);
	float s100 = (float)Sample1D(iX1 + iY0 + iZ0);
	float s010 = (float)Sample1D(iX0 + iY1 + iZ0);
	float s110 = (float)Sample1D(iX1 + iY1 + iZ0);
	float s001 = (float)Sample1D(iX0 + iY0 + iZ1);
	float s101 = (float)Sample1D(iX1 + iY0 + iZ1);
	float s011 = (float)Sample1D(iX0 + iY1 + iZ1);
	float s111 = (float)Sample1D(iX1 + iY1 + iZ1);

	float u = InterpolationPolynom(fFracX);
	float v = InterpolationPolynom(fFracY);
	float w = InterpolationPolynom(fFracZ);

	return lerp(lerp(lerp(s000, s100, u), lerp(s010, s110, u), v),
				lerp(lerp(s001, s101, u), lerp(s011, s111, u), v), w);
}

float OrE::Algorithm::PerlinNoise::Rand3D(float _fX, float _fY, float _fZ, float _fFrequence, Vec3& _vOutGrad)
{
	int iPeriod = int(_fFrequence*m_fPeriodicity);
	// We need 2 samples per dimension -> 8 samples total
	float fFracX, fFracY, fFracZ;
	int iX0, iY0, iZ0;
	IntFrac(_fX*_fFrequence, iX0, fFracX);
	IntFrac(_fY*_fFrequence, iY0, fFracY);
	IntFrac(_fZ*_fFrequence, iZ0, fFracZ);
	iX0 = (iX0+iPeriod)%iPeriod;
	iY0 = (iY0+iPeriod)%iPeriod;
	iZ0 = (iZ0+iPeriod)%iPeriod;
	int iX1 = ((iX0+1)%iPeriod)*57; iX0*=57;
	int iY1 = ((iY0+1)%iPeriod)*101; iY0*=101;
	int iZ1 = ((iZ0+1)%iPeriod)*307; iZ0*=307;

	float s000 = (float)Sample1D(iX0 + iY0 + iZ0);
	float s100 = (float)Sample1D(iX1 + iY0 + iZ0);
	float s010 = (float)Sample1D(iX0 + iY1 + iZ0);
	float s110 = (float)Sample1D(iX1 + iY1 + iZ0);
	float s001 = (float)Sample1D(iX0 + iY0 + iZ1);
	float s101 = (float)Sample1D(iX1 + iY0 + iZ1);
	float s011 = (float)Sample1D(iX0 + iY1 + iZ1);
	float s111 = (float)Sample1D(iX1 + iY1 + iZ1);

	float u = InterpolationPolynom(fFracX);
	float v = InterpolationPolynom(fFracY);
	float w = InterpolationPolynom(fFracZ);
	float du = Derivative(fFracX);
	float dv = Derivative(fFracY);
	float dw = Derivative(fFracZ);
	float uv = u*v;
	float uw = u*w;
	float vw = v*w;

	float k0 = s000;
    float k1 = s100 - s000;
    float k2 = s010 - s000;
    float k3 = s001 - s000;
    float k4 = s110 - s010 - k1;
    float k5 = s000 - s010 - s001 + s011;
    float k6 = - k1 - s001 + s101;
    float k7 = - k4 + s001 - s101 - s011 + s111;

	_vOutGrad.x = du * (k1 + k4*v + k6*w + k7*vw);
	_vOutGrad.y = dv * (k2 + k4*u + k5*w + k7*uw);
	_vOutGrad.z = dw * (k3 + k6*u + k5*v + k7*uv);

    return k0 + k1*u + k2*v + k3*w + k4*uv + k5*vw + k6*uw + k7*uv*w;
}

float OrE::Algorithm::PerlinNoise::Rand3D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, float _fZ)
{
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//OrE::Math::Pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		fRes += fAmplitude*Rand3D(_fX, _fY, _fZ, fFrequence);

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}
	
	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

float OrE::Algorithm::PerlinNoise::Rand3D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, float _fZ, Vec3& _vOutGrad)
{
	_vOutGrad.x = _vOutGrad.y = _vOutGrad.z = 0.0f;
	float fRes = 0.0f;
	float fAmplitude = 1.0f;
	float fFrequence = float(1<<_iLowOctave);//OrE::Math::Pow( 2.0f, (float)_iLowOctave );
	for(int i=_iLowOctave; i<=_iHeightOctave; ++i)
	{
		Vec3 vNormal;
		fRes += fAmplitude*Rand3D(_fX, _fY, _fZ, fFrequence, vNormal);
		_vOutGrad += fFrequence*fAmplitude*vNormal;

		fAmplitude *= _fPersistence;
		fFrequence *= 2.0f;
	}
	
	// Transform to [-1,1]
	return fRes*2.0f*(1.0f-_fPersistence)/(1.0f-fAmplitude)-1.0f;
}

// *************************************EOF**************************************** //