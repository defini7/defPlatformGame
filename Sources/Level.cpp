#include "../Include/Level.hpp"

Level::Level(const std::vector<TileType>& map, const def::vi2d& size)
{
    Load(map, size);
}

void Level::Create(const def::vi2d& size)
{
    m_Size = size;
    m_Data.clear();
    m_Data.resize(size.x * size.y);
}

void Level::Load(const std::vector<TileType>& map, const def::vi2d& size)
{
    Create(size);

    def::vi2d tile;

    for (tile.y = 0; tile.y < m_Size.y; tile.y++)
        for (tile.x = 0; tile.x < m_Size.x; tile.x++)
            SetTile(tile, map[tile.y * m_Size.x + tile.x]);
}

void Level::SetTile(const def::vi2d& pos, TileType tile)
{
    if (pos.x >= 0 && pos.y >= 0 && pos.x < m_Size.x && pos.y < m_Size.y)
        m_Data[pos.y * m_Size.x + pos.x] = tile;
}

TileType Level::GetTile(const def::vi2d& pos) const
{
    if (pos.x >= 0 && pos.y >= 0 && pos.x < m_Size.x && pos.y < m_Size.y)
        return m_Data[pos.y * m_Size.x + pos.x];

    return TileType::Empty;
}

const def::vi2d& Level::GetSize() const
{
    return m_Size;
}

const std::vector<TileType>& Level::GetData() const
{
    return m_Data;
}
