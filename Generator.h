// Map Generator for "Roots of Magic"
// The dll handles a singleton map. Only one map can be loaded at a time.

// A map consists of a regular grid with following block types:
// Block: 1 == Fels
// Block: 2 == Sand
// Block: 3 == Erde
// Block: 4 == Chrome

#include "OrTypeDef.h"
#include "OrRandom.h"

// Objects of all kinds
struct SMap {
	int iMinX, iMinY, iMaxX, iMaxY, iMinZ, iMaxZ;	// Bounding box
	unsigned  short* pBlockArray;						// 3D Array with z fast direction
	int GetSizeX()	{return iMaxX-iMinX;}
	int GetSizeY()	{return iMaxY-iMinY;}
	int GetSizeZ()	{return iMaxZ-iMinZ;}
	void Set(int x, int y, int z, int _iVal)	{pBlockArray[((x-iMinX)*GetSizeY()+y-iMinY)*GetSizeZ() + z-iMinZ] = _iVal;}
	int Get(int x, int y, int z)				{return pBlockArray[((x-iMinX)*GetSizeY()+y-iMinY)*GetSizeZ() + z-iMinZ];}
};
extern SMap Map;
extern OrE::Algorithm::PerlinNoise PerlinObject2D;
extern OrE::Algorithm::PerlinNoise PerlinObject3D;

struct FileHeader
{
	uint32 uiFileSignature;		// Check word + Something like file version
	uint32 uiSeed2D;			// Initial values for perlin noises
	uint32 uiSeed3D;			
	uint32 ui1;					// Free to use (upward compatibility)
	uint32 ui2;
	uint32 ui3;
	uint32 ui4;
	uint32 ui5;
};
static const char* FILE_SIGNATURE	= "blo1";



// All methods
extern "C" __declspec(dllexport) int GetBlock(float _fX, float _fY, float _fZ);
extern "C" __declspec(dllexport) bool IsSurfaceBlock(float _fX, float _fY, float _fZ);
extern "C" __declspec(dllexport) bool IsSolid(int x, int y, int z);
extern "C" __declspec(dllexport) void DestroyBlock(float _fX, float _fY, float _fZ);				// Removes an created block due to set 0
extern "C" __declspec(dllexport) void SetBlock(float _fX, float _fY, float _fZ, int _iType);		// set to a hopefully specified block (each integer possible)
// Load a map a generate a new one if file does not exists
// Returns false if new generated and true if loaded from file
extern "C" __declspec(dllexport) bool LoadBlockMap(wchar_t* _pcFileName);
// Save the current map to the file. The file is generated if not existing
// Returns false if file could not be opened
extern "C" __declspec(dllexport) bool SaveBlockMap(wchar_t* _pcFileName);




// Modifing constants
const float SURFACE_FREQ = 0.05f;
const float VOLUME_FREQ = 0.15f;
const float SURFACE_VERTICAL_BLOCK_NUMBER = 10.0f;
const float SURFACE_VERTICAL_BLOCK_OFFSET = 10.0f;

const bool TestMode = 0;