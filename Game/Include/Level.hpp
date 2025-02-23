#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <unordered_map>
#include <string>

#include "defGeometry2D.hpp"
#include "Dynamic.hpp"

enum class TileType : uint8_t
{
    Empty,
    Coin,
    Grass,
    Dirt
};

class Dynamic;

class Level
{
public:
    Level(const std::vector<TileType>& map, const def::Vector2i& size);

	void Create(const def::Vector2i& size);
    void Load(const std::vector<TileType>& map, const def::Vector2i& size);

	void SetTile(const def::Vector2i& pos, TileType tile);
	TileType GetTile(const def::Vector2i& pos) const;

    const def::Vector2i& GetSize() const;
    const std::vector<TileType>& GetData() const;

public:
    struct DynamicUnit
    {
        bool isRedundant = false;
        Dynamic* dynamic;
    };

    std::list<DynamicUnit> dynamics;

private:
    def::Vector2i m_Size;
	std::vector<TileType> m_Data;

};

#endif
