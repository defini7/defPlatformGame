#include "../Include/Level.hpp"

Level::Level(const std::vector<TileType>& map, const def::vi2d& size)
{
    Load(map, size);
}

void Level::Create(const def::vi2d& size)
{
    m_vSize = size;
    m_vecData.clear();
    m_vecData.resize(size.x * size.y);
}

void Level::Load(const std::vector<TileType>& map, const def::vi2d& size)
{
    Create(size);

    def::vi2d vTile;

    for (vTile.y = 0; vTile.y < m_vSize.y; vTile.y++)
        for (vTile.x = 0; vTile.x < m_vSize.x; vTile.x++)
            SetTile(vTile, map[vTile.y * m_vSize.x + vTile.x]);
}

void Level::SetTile(const def::vi2d& p, TileType tile)
{
    if (p.x >= 0 && p.y >= 0 && p.x < m_vSize.x && p.y < m_vSize.y)
        m_vecData[p.y * m_vSize.x + p.x] = tile;
}

TileType Level::GetTile(const def::vi2d& p) const
{
    if (p.x >= 0 && p.y >= 0 && p.x < m_vSize.x && p.y < m_vSize.y)
        return m_vecData[p.y * m_vSize.x + p.x];

    return TileType::Empty;
}

const def::vi2d& Level::GetSize() const
{
    return m_vSize;
}

const std::vector<TileType>& Level::GetData() const
{
    return m_vecData;
}
