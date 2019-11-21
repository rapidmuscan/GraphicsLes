#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "mesh.h"

 #define RAW_WIDTH 513
 #define RAW_HEIGHT 513

 #define HEIGHTMAP_X (16.0f*20)
 #define HEIGHTMAP_Z (16.0f*20)
 #define HEIGHTMAP_Y (1.25f*20)
 #define HEIGHTMAP_TEX_X 1.0f / 16.0f
 #define HEIGHTMAP_TEX_Z 1.0f / 16.0f

 class HeightMap : public Mesh {
 public:
	 HeightMap(std::string name);
	 ~HeightMap(void) {};
	
};
