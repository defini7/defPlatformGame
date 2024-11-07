#pragma once

#include <unordered_map>
#include <string>

#include "../Include/ScriptsManager.hpp"
#include "../Include/Level.hpp"

class Assets
{
public:
    static Assets& Get();

    Assets(Assets const&) = delete;
	void operator=(Assets const&) = delete;

public:
    void LoadSprite(const std::string& name, def::Graphic* sprite);
    void LoadLevel(const std::string& name, Level* level);

    def::Graphic* GetSprite(const std::string& name);
    Level* GetLevel(const std::string& name);

    bool LoadConfig();

    std::unordered_map<TileType, def::vi2d> mapSpriteFileOffsets;
    def::vi2d vTileSize;

private:
    Assets() = default;
	~Assets() = default;

    std::unordered_map<std::string, def::Graphic*> m_mapSprites;
    std::unordered_map<std::string, Level*> m_mapLevels;

};
