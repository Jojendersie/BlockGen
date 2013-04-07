#include "Generator.h"
#include "Helpers.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

// **************************************************************************** //
// Predeclarations
void Fill(int x1, int x2, int y1, int y2, int z1, int z2);					// Fills the given area with perlin noise. Not doing any test for boundaries!
void Extend(int _bx, int _by, int _bz);										// Enlarges the map array

// **************************************************************************** //
SMap Map;
OrE::Algorithm::PerlinNoise PerlinObject2D(0, 10000000.0f);
OrE::Algorithm::PerlinNoise PerlinObject3D(1, 10000000.0f);

// **************************************************************************** //
int GenerateBlock(int x, int y, int z) // generates the structures, no dynamic objects
{
	float fx = x * SURFACE_FREQ;
	float fy = y * SURFACE_FREQ;
	float fzr = (z-SURFACE_VERTICAL_BLOCK_OFFSET)/float(SURFACE_VERTICAL_BLOCK_NUMBER);

	int iPriorBlockType = 1;

	// Interpolations between different biotops
	float f1 = PerlinObject3D.Rand2D(0,3,0.5f,fx*0.1f,fy*0.1f);						f1*=f1;
	float f2 = PerlinObject2D.Rand2D(0,3,0.5f,fx*0.1f+23493.034f,fy*0.1f+109.0f);	f2*=f2;
	float f3 = 0.4f*PerlinObject3D.Rand2D(0,3,0.5f,fx*0.1f-964.0f,fy*0.1f+8653411.11f);	f3*=f3;
	float fSum = 1.0f/(f1+f2+f3);
	f1 *= fSum;	f2 *= fSum;	f3 *= fSum;
	//if(f3 > f1 && f3 > f2) return 1;
	//else return 0;
	
	// --------------- Desert ---------------------
	// Warp effekt
	float fwx = fx+PerlinObject3D.Rand1D(0,2,0.5f, fx)*0.75f;
	float fwy = fy+PerlinObject3D.Rand1D(0,2,0.5f, fy)*0.75f;
	// Seichte Oberfläche
	float fSurfaceClip = PerlinObject2D.Rand2D(0,7,0.6f,fwx, fwy)*SURFACE_VERTICAL_BLOCK_NUMBER*0.5f+SURFACE_VERTICAL_BLOCK_OFFSET;
	//fSurfaceClip = int(fSurfaceClip)>z?1.0f:0.0f;
	fSurfaceClip -= z;
	fSurfaceClip *= 0.1f;
	// Säule
	//float fCol = PerlinObject3D.Rand2D(0,3,0.6f,fx,fy)>0.3f?3.6f:1.0f;
	float fCol = PerlinObject2D.Rand2D(2,5,0.4f,fx,fy);
	fCol = std::pow( clamp(fCol-0.4f, 0.0f, 0.15f)*5.0f, 2.0f );// + (fCol>0.3f ? 0.1f : 0.0f);

	float fDesert = fSurfaceClip+fCol;
	// If desert is the main landscape control the block type
	if((f1>f2) && (f1>f3))
	{
		if(fSurfaceClip-fzr <= 0.3f) iPriorBlockType = 2;
		if(fCol >= 0.2f) iPriorBlockType = 1;
	}

	// --------------- Standard -------------------
	// Cut through the volume with an 2D Perlin noise
	fSurfaceClip = PerlinObject2D.Rand2D(0,7,0.6f,SURFACE_FREQ * x, SURFACE_FREQ * y);

	// Volumetric sample (caves)
	float fVol = PerlinObject3D.Rand3D(0,4,0.4f,VOLUME_FREQ * x, VOLUME_FREQ * y, 3.0f*VOLUME_FREQ * z);
	float fVolThreshold = (z + SURFACE_VERTICAL_BLOCK_OFFSET)/(SURFACE_VERTICAL_BLOCK_NUMBER*3.0f);
	fVolThreshold *= fVolThreshold;
	fVolThreshold -= fVol;

	float fStandard = fVolThreshold*(fSurfaceClip>fzr?1.0f:0.0f);

	// If standart is the main landscape control the block type
	if((f2>f1) && (f2>f3))
	{
		if(fSurfaceClip-fzr <= 0.25f) iPriorBlockType = 3;
	}

	// --------------- 3D Bubbles ------------------
	float fHeigh = -(z-SURFACE_VERTICAL_BLOCK_OFFSET)*VOLUME_FREQ*0.5f;
	fVol = PerlinObject3D.Rand3D(0,4,0.4f,VOLUME_FREQ * x, VOLUME_FREQ * y, 3.0f*VOLUME_FREQ * z);

	float fBubble = fHeigh+fVol;

	// If standart is the main landscape control the block type
	if((f3>f1) && (f3>f2))
	{
		iPriorBlockType = 4;
	}

	return ((f1*fDesert + f2*fStandard + f3*fBubble) > 0.0f)?iPriorBlockType:0;
}

/* GenerateDynamics() *************************************
 * generates water and vegetation
 * call this after the surface exists
 * grass 501
 * tree [511; 520]
 */
int GenerateDynamics(int x, int y, int z)
{
	static float TreeRate = 0.9f; 
	if(Map.Get(x,y,z - 1) == 3) //dirt as ground
	{
		if(OrE::Algorithm::Rand() < TreeRate)
		{
			TreeRate = 0.f;
			return -(int(OrE::Algorithm::Rand() * 9 + 1)) - 10;
		}
	TreeRate += 0.1f;
	return -1;//-1
	}
	return 0;
}

// **************************************************************************** //
// Enlargen the map array
void Extend(int _bx, int _by, int _bz)
{
	// Give +20 blocks on one ore more (max 3) sides
	int iNewMinX = (_bx<0?-20:0)+Map.iMinX;
	int iNewMaxX = (_bx>0?20:0)+Map.iMaxX;		int iSX = iNewMaxX-iNewMinX;
	int iNewMinY = (_by<0?-20:0)+Map.iMinY;
	int iNewMaxY = (_by>0?20:0)+Map.iMaxY;		int iSY = iNewMaxY-iNewMinY;
	int iNewMinZ = (_bz<0?-20:0)+Map.iMinZ;
	int iNewMaxZ = (_bz>0?20:0)+Map.iMaxZ;		int iSZ = iNewMaxZ-iNewMinZ;
	unsigned char* pNewBlockArray = (unsigned char*)malloc(iSX*iSY*iSZ);
	for(int x=iNewMinX; x<iNewMaxX; ++x)
		for(int y=iNewMinY; y<iNewMaxY; ++y)
			for(int z=iNewMinZ; z<iNewMaxZ; ++z)
			{
				// Outside old map?
				if(x>=Map.iMinX && x<Map.iMaxX
					&& y>=Map.iMinY && y<Map.iMaxY
					&& z>=Map.iMinZ && z<Map.iMaxZ)
					// inside old map
					pNewBlockArray[((x-iNewMinX)*iSY+y-iNewMinY)*iSZ + z-iNewMinZ] = Map.Get(x,y,z);
				else
					pNewBlockArray[((x-iNewMinX)*iSY+y-iNewMinY)*iSZ + z-iNewMinZ] = GenerateBlock(x,y,z);
			}
	// Copy new data into map
	free(Map.pBlockArray);
	Map.pBlockArray = pNewBlockArray;
	Map.iMinX = iNewMinX;	Map.iMaxX = iNewMaxX;
	Map.iMinY = iNewMinY;	Map.iMaxY = iNewMaxY;
	Map.iMinZ = iNewMinZ;	Map.iMaxZ = iNewMaxZ;
}

// **************************************************************************** //
__declspec(dllexport) int GetBlock(float _fX, float _fY, float _fZ)
{
	int x = (int)_fX;
	int y = (int)_fY;
	int z = (int)_fZ;
	// extend map if necessary
	int iExX = x<Map.iMinX ? -1 : (x>=Map.iMaxX ? 1 : 0);
	int iExY = y<Map.iMinY ? -1 : (y>=Map.iMaxY ? 1 : 0);
	int iExZ = z<Map.iMinZ ? -1 : (z>=Map.iMaxZ ? 1 : 0);
	if(iExX || iExY || iExZ) Extend(iExX,iExY,iExZ);
	return Map.Get(x,y,z);
}

// **************************************************************************** //
__declspec(dllexport) bool IsSurfaceBlock(float _fX, float _fY, float _fZ)
{
	// Check if block defined
	int x = (int)_fX;
	int y = (int)_fY;
	int z = (int)_fZ;
	if(!Map.Get(x,y,z)) return false;
	// Check neigborhood for given block
	// If neighbor outside of the map -> Surface Block
	// If neighbor is empty block -> Surface Block
	/*if(x == Map.iMinX || x == Map.iMaxX-1
		|| y == Map.iMinY || y == Map.iMaxY-1
		|| z == Map.iMinZ || z == Map.iMaxZ-1)
		return true;*/
	if(IsSolid(x,y+1,z)) return true;
	if(IsSolid(x,y-1,z)) return true;
	if(IsSolid(x,y,z+1)) return true;
	if(IsSolid(x,y,z-1)) return true;
	if(IsSolid(x+1,y,z)) return true;
	if(IsSolid(x-1,y,z)) return true;
	return false;
}

// **************************************************************************** //
__declspec(dllexport) bool IsSolid(float _fX, float _fY, float _fZ)
{
	int x = (int)_fX;
	int y = (int)_fY;
	int z = (int)_fZ;

	uint16 Type = Map.Get(x,y,z);
	if(Type > 0 && Type <= 500) return true;
	return false;
}
// **************************************************************************** //
// Removes a created block by setting its value to 0
__declspec(dllexport) void DestroyBlock(float _fX, float _fY, float _fZ)
{
	int x = (int)_fX;
	int y = (int)_fY;
	int z = (int)_fZ;
	Map.Set(x,y,z, 0);
}

// **************************************************************************** //
// Set value to a hopefully specified block (each integer possible)
__declspec(dllexport) void SetBlock(float _fX, float _fY, float _fZ, int _iType)
{
	int x = (int)_fX;
	int y = (int)_fY;
	int z = (int)_fZ;
	Map.Set(x,y,z, _iType);
}


// **************************************************************************** //
// Load a map a generate a new one if file does not exists
__declspec(dllexport) bool LoadBlockMap(wchar_t* _pcFileName)
{
	// Errors are not supported!
	// Try open
	FILE* pFile = _wfopen(_pcFileName, L"rb");
	if(!pFile)
	{
		// Case file does not exists (create a minimal block).
		Map.iMinX = -20;	Map.iMaxX = 20;
		Map.iMinY = -20;	Map.iMaxY = 20;
		Map.iMinZ = -20;	Map.iMaxZ = 20;
		Map.pBlockArray = (unsigned char*)malloc(Map.GetSizeX()*Map.GetSizeY()*Map.GetSizeZ());

		// Seed random to file name
		uint32 dwSeed = 0;
		while(*_pcFileName)
		{
			dwSeed += *_pcFileName;
			// Only remark the real file not the path
			if(*_pcFileName == '/' || *_pcFileName == '\\') dwSeed = 0;
			++_pcFileName;
		}
		PerlinObject2D.SetSeed(dwSeed);
		PerlinObject3D.SetSeed(dwSeed*1009 + 71);

		Fill(-20,20,-20,20,-20,20);
		return false;
	} else {
		// Read and check for version...
		FileHeader FH;
		fread(&FH, sizeof(FileHeader), 1, pFile);
		if(FH.uiFileSignature != *(unsigned int*)FILE_SIGNATURE) return false;

		PerlinObject2D.SetSeed(FH.uiSeed2D);
		PerlinObject3D.SetSeed(FH.uiSeed3D);

		// Reading bounding box
		fread(&Map, sizeof(int)*6, 1, pFile);

		// Allocating and reading data
		Map.pBlockArray = (unsigned char*)malloc(Map.GetSizeX()*Map.GetSizeY()*Map.GetSizeZ());
		fread(Map.pBlockArray, Map.GetSizeX()*Map.GetSizeY()*Map.GetSizeZ(), 1, pFile);

		fclose(pFile);
		return true;
	}
}

// **************************************************************************** //
// Fills the given area with perlin noise. Not doing any test for boundaries!
void Fill(int x1, int x2, int y1, int y2, int z1, int z2)
{
	static bool bPreviousZ = false;
	for(int x=x1;x<x2;++x)
		for(int y=y1;y<y2;++y)
			for(int z=z1;z<z2;++z)
			{
				Map.Set(x,y,z, GenerateBlock(x,y,z));
				if(bPreviousZ && !Map.Get(x,y,z))
					Map.Set(x,y,z, GenerateDynamics(x,y,z));
				bPreviousZ = Map.Get(x,y,z) > 1;
			}
}

// **************************************************************************** //
// Save the current map to the file. The file is generated if not existing
__declspec(dllexport) bool SaveBlockMap(wchar_t* _pcFileName)
{
/*	FILE* pFile2 = fopen("c:\dest.txt", "wb");
	fwrite(_pcFileName, sizeof(wchar_t)*wcslen(_pcFileName), 1, pFile2);
	fclose(pFile2);*/
	// Open and create, errors are not supported!
	FILE* pFile = _wfopen(_pcFileName, L"wb");
	if(!pFile) return false;

	FileHeader FH;
	FH.uiFileSignature = *(unsigned int*)FILE_SIGNATURE;
	FH.uiSeed2D = PerlinObject2D.GetSeed();
	FH.uiSeed3D = PerlinObject3D.GetSeed();
	fwrite(&FH, sizeof(FileHeader), 1, pFile);

	// Writing bounding box
	fwrite(&Map, sizeof(int)*6, 1, pFile);

	// Writing data
	fwrite(Map.pBlockArray, Map.GetSizeX()*Map.GetSizeY()*Map.GetSizeZ(), 1, pFile);

	fclose(pFile);
	return true;
}