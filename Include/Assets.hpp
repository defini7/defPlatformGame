#pragma once

#include <unordered_map>
#include <string>

#include "Logger.hpp"
#include "ScriptsManager.hpp"
#include "Game.hpp"
#include "Level.hpp"

enum class TileType : uint8_t;
class Level;

class Assets
{
public:
    static Assets& Get();

    Assets(Assets const&) = delete;
	void operator=(Assets const&) = delete;

public:
    void LoadSprite(const std::string& name, def::Graphic* sprite);
    def::Graphic* GetSprite(const std::string& name);
    std::unordered_map<std::string, def::Graphic*>& GetSprites();

    bool LoadConfig();
    
    std::unordered_map<TileType, def::vi2d> spriteFileOffsets;
    def::vi2d tileSize;

private:
    Assets() = default;
	~Assets() = default;

    std::unordered_map<std::string, def::Graphic*> m_Sprites;
    std::unordered_map<std::string, Level*> m_Levels;

};
