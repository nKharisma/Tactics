#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 15;

class Unit;

struct Tile {
	std::string fileName;
	bool isWalkable;
	int movementCost;
	Unit* occupyingUnit = nullptr;
};

class Map {
public:
	Map();
	
	void Initialize(const std::string& mapFilePath); 
	int GetWidth() const { return MAP_WIDTH; }
	int GetHeight() const { return MAP_HEIGHT; }
	const Tile& GetTile(int x, int y) const;
	Tile& GetTile(int x, int y);
	
private:
	std::vector<std::vector<Tile>> m_tiles;
};

#endif 