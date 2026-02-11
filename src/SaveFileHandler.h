#pragma once

#include <string>

class World;

class SaveFileHandler {
public:
    bool Save(const World& world, const std::string& filepath) const;
    bool Load(World& world, const std::string& filepath) const;
};
