// Map Generator for "Roots of Magic"
// The dll handles a singleton map. Only one map can be loaded at a time.

// A map consists of a regular grid with following block types:
// Block: 1 == Fels
// Block: 2 == Sand
// Block: 3 == Erde
// Block: 4 == Chrome

#include "OrTypeDef.h"
#include "OrRandom.h"
#include <sstream>

// Objects of all kinds

//params for SMap
const int MaxChunks = 1024; // 8*8*8*1024*2 = 1.048.576b 1mb
const  unsigned int ChunkLength = 8;

struct chunk {
	int x0, y0, z0;
	unsigned short block[ChunkLength*ChunkLength*ChunkLength];
};
struct SMap {
	chunk* pChunkArray[MaxChunks];				//[0] points on the last access one 
	wchar_t* Dir;
	int IndexHigh; //highest entry
	void Ini()
	{
		for(int i = 0; i < MaxChunks; i++)
			pChunkArray[i] = NULL;
	}
	void Set(int x, int y, int z, int _iVal)
	{
		bool bVoidEntry = false;
		//get chunk origin
		int ModX = x % ChunkLength; 
		int ModY = y % ChunkLength;
		int ModZ = z % ChunkLength;
		for(int i = 0; i < IndexHigh; i++)
			if(pChunkArray[i] != NULL && pChunkArray[i]->x0==x-ModX && pChunkArray[i]->y0==y-ModY && pChunkArray[i]->z0==z-ModZ)
			{
				pChunkArray[i]->block[abs(ModZ) + abs(ModX)*ChunkLength + abs(ModY)*ChunkLength*ChunkLength] = _iVal;
				if(pChunkArray[0] != pChunkArray[i]) pChunkArray[0] = pChunkArray[i]; //save last access
				return;
			}
	}
	int Get(int x, int y, int z)				
	{
		//get chunk origin
		int ModX = x % ChunkLength; 
		int ModY = y % ChunkLength;
		int ModZ = z % ChunkLength;
		for(int i = 0; i <= IndexHigh; i++)
			if(pChunkArray[i] != NULL && pChunkArray[i]->x0==x-ModX && pChunkArray[i]->y0==y-ModY && pChunkArray[i]->z0==z-ModZ)
			{
				return pChunkArray[i]->block[abs(ModZ) + abs(ModX) * ChunkLength+abs(ModY)*ChunkLength *ChunkLength];
			}
		return 65535; //not defined
	}
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

extern "C" __declspec(dllexport) void House(int x0,int y0,int z0, int x1,int y1,int etages = 1,int windows = 0);
// Load a map a generate a new one if file does not exists
// Returns false if new generated and true if loaded from file
//@param _pcFileName - Name without endings or path
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
