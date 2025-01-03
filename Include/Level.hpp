#pragma once

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
    Level(const std::vector<TileType>& map, const def::vi2d& size);

	void Create(const def::vi2d& size);
    void Load(const std::vector<TileType>& map, const def::vi2d& size);

	void SetTile(const def::vi2d& p, TileType tile);
	TileType GetTile(const def::vi2d& p) const;

    const def::vi2d& GetSize() const;
    const std::vector<TileType>& GetData() const;

public:
    struct DynamicUnit
    {
        bool bRedundant = false;
        Dynamic* pDynamic;
    };

    std::list<DynamicUnit> listDynamics;

private:
    def::vi2d m_vSize;
	std::vector<TileType> m_vecData;

};
