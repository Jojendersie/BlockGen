// ******************************************************************************** //
// OrRandom.h																		//
// ==========																		//
// This file is part of the OrBaseLib.												//
//																					//
// Author: Johannes Jendersie														//
//																					//
// Here is a quite easy licensing as open source:									//
// http://creativecommons.org/licenses/by/3.0/										//
// If you use parts of this project, please let me know what the purpose of your	//
// is project is. You can do this by writing a comment at github.com/Jojendersie/.	//
//																					//
// For details on this project see: Readme.txt										//
//																					//
// ******************************************************************************** //
// Implementation of different pseudo-random generators.								//
//																					//
// ******************************************************************************** //

#pragma once

namespace OrE {
namespace Algorithm {

// ******************************************************************************** //
// Initialize Mersenne Twister
void SRand(uint32 _dwSeed);

// Creates a random number between 0 and 1 (inclusive)
float Rand();

// Creates an integral random number between _iMin and _iMax (inclusive)
int	Rand(int _iMin, int _iMax);

// Creates an unsigned random Number
uint32 RandU();

// Creates a random number of an exponential distribution [0,+Inf]
float ExpRand(float _fLambda);

// Creates a random number of a normal distribution [-Inf,+Inf]
float StdNormRand(float _fMean, float _fVariance);

// Creates a random number of a normal distribution [-Inf,+Inf]
float NormRand();

// ******************************************************************************** //
// A perlin noise object for n-D-Noise Variables. (~4KB per Object)
class PerlinNoise
{
private:
	uint32 m_dwSeed;
	float m_fPeriodicity;
	double Sample1D(__int64 _i);
public:
	// Initialize the object deterministically.
	// Parameter:
	//	_dwSeed - Random base value - causes same results in same applications
	//	_fPeriodicity - after which distance the noise should be periodically. Minimum is 1.
	PerlinNoise(uint32 _dwSeed, float _fPeriodicity);

	// One sample at the specified position R^d -> [-1,1]
	// Parameter:
	//	_fFrequence - used to determine periodically edges and octave distances in sampling
	//	_iLowOctave - Octave with the lowest frequency (inclusive)
	//	_iHeightOctave - Octave with the highest frequency (inclusive). All frequencies between are sampled too.
	//	_fPersistence - Relative height of a frequency relative to the last one. Recommended: 0.5f.
	//	_fX - 1 float coordinate.
	//	_vOutGrad - Gradient vector at the sampled point (analytical determined)
	float Rand1D(float _fX, float _fFrequence);
	float Rand1D(float _fX, float _fFrequence, float& _vOutGrad);
	float Rand1D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX);
	float Rand1D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float& _vOutGrad);

	float Rand2D(float _fX, float _fY, float _fFrequence);
	float Rand2D(float _fX, float _fY, float _fFrequence, Vec3& _vOutNormal);
	float Rand2D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY);
	float Rand2D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, Vec3& _vOutGrad);

	float Rand3D(float _fX, float _fY, float _fZ, float _fFrequence);
	float Rand3D(float _fX, float _fY, float _fZ, float _fFrequence, Vec3& _vOutGrad);
	float Rand3D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, float _fZ);
	float Rand3D(int _iLowOctave, int _iHeightOctave, float _fPersistence, float _fX, float _fY, float _fZ, Vec3& _vOutGrad);

	void SetSeed(uint32 _dwSeed)		{m_dwSeed = _dwSeed;}
	uint32 GetSeed()					{return m_dwSeed;}
};

}; // namespace OrE
}; // namespace Algorithm
// *************************************EOF**************************************** //