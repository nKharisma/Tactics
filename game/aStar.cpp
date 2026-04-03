#include "aStar.h"
#include "map.h"
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <queue>

std::vector<SDL_Point> aStar::FindPath(int startX, int startY, int endX, int endY, const Map& map) {
    std::vector<SDL_Point> path;

    if (!map.GetTile(endX, endY).isWalkable) {
        SDL_Log("Target tile (%d,%d) is not walkable", endX, endY);
        return path;
    }

    Node* startNode = new Node{startX, startY, 0, Heuristic(startX, startY, endX, endY), nullptr};
    
    // sorting based on fCost, then hCost as tiebreaker
    auto compare = [](Node* left, Node* right) { 
        if (left->fCost() == right->fCost()) {
            return left->hCost > right->hCost;
        }
        return left->fCost() > right->fCost();
    };

    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> openNodes(compare);
    std::unordered_map<int, Node*> closedNodes;
    std::unordered_map<int, Node*> openNodesMap;
    std::vector<Node*> allNodes;

    openNodes.push(startNode);
    openNodesMap[Key(startNode->x, startNode->y, map.GetWidth())] = startNode;
    allNodes.push_back(startNode);

    while (!openNodes.empty()) {
        Node* currentNode = openNodes.top();
        openNodes.pop();
        openNodesMap.erase(Key(currentNode->x, currentNode->y, map.GetWidth()));

        if (currentNode->x == endX && currentNode->y == endY) {
            path = ReconstructPath(currentNode);
            break;
        }

        closedNodes[Key(currentNode->x, currentNode->y, map.GetWidth())] = currentNode;

        for (Node* neighbor : GetNeighbors(currentNode, map)) {
            int neighborKey = Key(neighbor->x, neighbor->y, map.GetWidth());

            if (closedNodes.find(neighborKey) != closedNodes.end()) {
                delete neighbor; // not added to allNodes
                continue;
            }

            int tentativeGCost = currentNode->gCost + 1;

            if (openNodesMap.find(neighborKey) == openNodesMap.end() || tentativeGCost < neighbor->gCost) {
                neighbor->gCost = tentativeGCost;
                neighbor->hCost = Heuristic(neighbor->x, neighbor->y, endX, endY);
                neighbor->parent = currentNode;

                if (openNodesMap.find(neighborKey) == openNodesMap.end()) {
                    openNodes.push(neighbor);
                    openNodesMap[neighborKey] = neighbor;
                    allNodes.push_back(neighbor);
                }
            } else {
                delete neighbor;
            }
        }
    }
    
    //cleanup of all allocated nodes
    for (Node* n : allNodes) {
        delete n;
    }

    if (path.empty()) {
        std::cerr << "No path found from (" << startX << ", " << startY << ") to (" << endX << ", " << endY << ")." << std::endl;
    }

    return path;
}

std::vector<SDL_Point> aStar::ReconstructPath(Node* node) {
    std::vector<SDL_Point> path;
    while (node != nullptr) {
        path.push_back({node->x, node->y});
        node = node->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// manhatten distance heuristic
int aStar::Heuristic(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<aStar::Node*> aStar::GetNeighbors(Node* node, const Map& map) {
    std::vector<Node*> neighbors;
    const int directions[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}}; // N, W, E, S

    for (const auto& dir : directions) {
        int newX = node->x + dir[0]; // row
        int newY = node->y + dir[1]; // col

        if (newX >= 0 && newX < map.GetHeight() && newY >= 0 && newY < map.GetWidth()) {
            const Tile& tile = map.GetTile(newX, newY);
            if (tile.isWalkable) {
                SDL_Log("Neighbor added: (%d, %d)", newX, newY);
                neighbors.push_back(new Node{newX, newY, 0, 0, nullptr});
            }
        } else {
            SDL_Log("Position (%d, %d) is out of bounds.", newX, newY);
        }
    }

    return neighbors;
}
