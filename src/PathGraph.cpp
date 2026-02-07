#include "PathGraph.h"

// 4-directional offsets: right, down, left, up
static const int DX[] = { 1, 0, -1, 0 };
static const int DY[] = { 0, 1, 0, -1 };

bool PathGraph::IsPath(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const {
    if (x < 0 || x >= cols || y < 0 || y >= rows) return false;
    return tiles[y][x].type == TileType::Path;
}

int PathGraph::CountNeighbors(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const {
    int count = 0;
    for (int d = 0; d < 4; d++) {
        if (IsPath(tiles, x + DX[d], y + DY[d], rows, cols)) count++;
    }
    return count;
}

bool PathGraph::IsNode(const std::vector<std::vector<Tile>>& tiles, int x, int y, int rows, int cols) const {
    if (!IsPath(tiles, x, y, rows, cols)) return false;

    int neighbors = CountNeighbors(tiles, x, y, rows, cols);

    // Endpoint (dead end) or intersection (3+ neighbors)
    if (neighbors != 2) return true;

    // 2 neighbors: check if they form a corner (not a straight line)
    bool hasLeft  = IsPath(tiles, x - 1, y, rows, cols);
    bool hasRight = IsPath(tiles, x + 1, y, rows, cols);
    bool hasUp    = IsPath(tiles, x, y - 1, rows, cols);
    bool hasDown  = IsPath(tiles, x, y + 1, rows, cols);

    // Straight line = opposite sides connected
    if ((hasLeft && hasRight) || (hasUp && hasDown)) return false;

    // Otherwise it's a corner
    return true;
}

void PathGraph::Build(const std::vector<std::vector<Tile>>& tiles, int rows, int cols) {
    nodes.clear();
    posToNode.clear();
    maxCols = cols;

    // Pass 1: Find all nodes
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (IsNode(tiles, x, y, rows, cols)) {
                int idx = (int)nodes.size();
                posToNode[PosKey(x, y)] = idx;
                nodes.push_back(PathNode{x, y, {}});
            }
        }
    }

    // Pass 2: Walk from each node in each direction to find edges
    for (int i = 0; i < (int)nodes.size(); i++) {
        int nx = nodes[i].x;
        int ny = nodes[i].y;

        for (int d = 0; d < 4; d++) {
            int cx = nx + DX[d];
            int cy = ny + DY[d];

            if (!IsPath(tiles, cx, cy, rows, cols)) continue;

            // Walk in this direction until we hit another node
            int cost = 1;
            while (true) {
                auto it = posToNode.find(PosKey(cx, cy));
                if (it != posToNode.end()) {
                    // Found a node - create edge
                    nodes[i].edges.push_back({it->second, cost + 1});
                    break;
                }

                // Continue walking - find the next path tile that continues the direction
                // From a straight tile, there are exactly 2 neighbors; go to the one we didn't come from
                int prevX = (cost == 1) ? nx : cx - DX[d];
                int prevY = (cost == 1) ? ny : cy - DY[d];

                bool found = false;
                for (int nd = 0; nd < 4; nd++) {
                    int nextX = cx + DX[nd];
                    int nextY = cy + DY[nd];
                    if (nextX == prevX && nextY == prevY) continue;
                    if (IsPath(tiles, nextX, nextY, rows, cols)) {
                        // Update direction for next iteration
                        prevX = cx;
                        prevY = cy;
                        cx = nextX;
                        cy = nextY;
                        cost++;
                        found = true;
                        break;
                    }
                }

                if (!found) break;
            }
        }
    }
}

int PathGraph::FindNode(int x, int y) const {
    auto it = posToNode.find(PosKey(x, y));
    if (it != posToNode.end()) return it->second;
    return -1;
}

void PathGraph::RenderDebug(GameCamera& camera) {
    float halfTile = TILE_SIZE * camera.zoom * 0.5f;

    // Draw edges as yellow lines
    for (int i = 0; i < (int)nodes.size(); i++) {
        Vector2 from = WorldToScreen(nodes[i].x, nodes[i].y, camera.offset, camera.zoom);
        from.x += halfTile;
        from.y += halfTile;

        for (auto& [target, cost] : nodes[i].edges) {
            // Only draw each edge once (from lower index to higher)
            if (target <= i) continue;

            Vector2 to = WorldToScreen(nodes[target].x, nodes[target].y, camera.offset, camera.zoom);
            to.x += halfTile;
            to.y += halfTile;

            DrawLineEx(from, to, 2.0f, YELLOW);
        }
    }

    // Draw nodes as green circles on top
    for (const PathNode& node : nodes) {
        Vector2 pos = WorldToScreen(node.x, node.y, camera.offset, camera.zoom);
        float radius = 4.0f * camera.zoom;
        DrawCircle((int)(pos.x + halfTile), (int)(pos.y + halfTile), radius, GREEN);
    }
}
