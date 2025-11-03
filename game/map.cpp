#include "map.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

Map::Map() : m_tiles(MAP_HEIGHT, std::vector<Tile>(MAP_WIDTH)) {
}

void Map::Initialize(const std::string& mapFilePath) {
	std::ifstream mapFile(mapFilePath);
	if (!mapFile.is_open()) {
		std::cerr << "Failed to open map file: " << mapFilePath << std::endl;
		return;
	}
	std::vector<std::string> tileNames = {
		"Green_Grass_Full",
		"Dirt_Full",
		"Water_Full",
		"Dirt_Half",
	};
	
	char tileChar;

	for(int y = 0; y < MAP_HEIGHT; ++y) {
		for(int x = 0; x < MAP_WIDTH; ++x) {
			Tile tile;
			if (!(mapFile >> tileChar)) {
				std::cerr << "Not enough tile data in map file at position (" << x << ", " << y << ")" << std::endl;
				tile.fileName = "";
				tile.isWalkable = false;
			} else {
				switch(tileChar) {
					case 'G':
						tile.fileName = "Green_Grass_Full";
						tile.isWalkable = true;
						break;
					case 'S':
						tile.fileName = "Sand_Full";
						tile.isWalkable = true;
						break;
					case 'D':
						tile.fileName = "Diagonal_Path_Green";
						tile.isWalkable = true;
						break;
					case 'W':
						tile.fileName = "Water_Full";
						tile.isWalkable = false;
						break;
					case 'I':
						tile.fileName = "Straight_Path_Full";
						tile.isWalkable = true;
						break;
					case 'T':
						tile.fileName = "T_Path_Full";
						tile.isWalkable = true;
						break;
					case 's':
						tile.fileName = "Sand_Half";
						tile.isWalkable = true;
						break;
					case 'g':
						tile.fileName = "Green_Grass_Half";
						tile.isWalkable = true;
						break;
					default:
						std::cerr << "Unknown tile character: " << tileChar << std::endl;
						tile.fileName = "";
						tile.isWalkable = false;
				}
			}
			m_tiles[y][x] = tile;
		}
	}
	
	std::cout << "Map initialized with random tiles." << std::endl;
}

const Tile& Map::GetTile(int x, int y) const {
	if( x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT){
		return m_tiles[y][x];
	} else {
		std::cerr << "Tile coordinates out of bounds at: (" << x << ", " << y << ")" << std::endl;
		
		static Tile invalidTile = {"", false};
		return invalidTile;
	}
}

Tile& Map::GetTile(int x, int y) {
	if( x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT){
		return m_tiles[y][x];
	} else {
		std::cerr << "Tile coordinates out of bounds at: (" << x << ", " << y << ")" << std::endl;
		
		static Tile invalidTile = {"", false};
		return invalidTile;
	}
}