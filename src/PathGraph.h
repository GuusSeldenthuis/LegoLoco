#pragma once

#include "Tile.h"
#include "Camera.h"
#include <vector>
#include <unordered_map>

struct PathNode {
    int x, y;
    // Each edge: {target node index, cost in tiles}
    std::vector<std::pair<int, int>> edges;
};

class PathGraph {
private:
    std::vector<PathNode> nodes;
    std::unordered_map<int, int> posToNode;
    int maxCols = 0;

    int PosKey(int x, int y) const { return y * maxCols + x; }
    bool IsPath(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const;
    int CountNeighbors(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const;
    bool IsNode(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const;

public:
    void Build(const std::vector<std::vector<Tile>>& tiles, int rows, int cols);
    void RenderDebug(GameCamera& camera);
    const std::vector<PathNode>& GetNodes() const { return nodes; }
    int FindNode(int x, int y) const;
};
