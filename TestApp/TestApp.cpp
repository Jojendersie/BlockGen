// TestApp.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <conio.h>
#include "stdafx.h"
#include "..\Generator.h"


char* g_acASCIIArtTable = "  .-~=oO#@@@@@@@@@";



int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlockMap(L"dirt03");

	// Schnitt
	for(int z=19;z>=-20;--z)
	{
		
		for(int y=-20;y<59;++y)
			if(IsSurfaceBlock(0.0f, (float)y, (float)z))
				std::cout << GetBlock(0.0f, (float)y, (float)z);
			else std::cout << " ";
		std::cout << '\n';
	}

	// Oberfläche
	/*for(int x=-20;x<20;++x)
	{
		for(int y=-20;y<59;++y)
		{
			int z = 19;
			for(;z>=-20;--z) if(GetBlock(x, y, z)) break;
			std::cout << g_acASCIIArtTable[z-4];
		}
		std::cout << '\n';
	}*/

	//Save(L"test");
	_getch();
	return 0;
}

