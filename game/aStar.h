#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <queue>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <SDL2/SDL.h>


class Map;



class aStar {
	public: 
		std::vector<SDL_Point> FindPath(int startX, int startY, int endX, int endY, const Map& map);
		struct Node {
	int x, y;
	int gCost;
	int hCost;
	int fCost() const { return gCost + hCost; }
	Node* parent;
};
	private:
		int Heuristic(int x1, int y1, int x2, int y2);
		std::vector<Node*> GetNeighbors(Node* node, const Map& map);
		std::vector<SDL_Point> ReconstructPath(Node* node);
		static inline int Key(int x, int y, int width) {
        return x * width + y; // row * width + col
    }
};

#endif