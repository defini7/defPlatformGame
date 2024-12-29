#include "../Include/Assets.hpp"

Assets& Assets::Get()
{
    static Assets me;
    return me;
}

void Assets::LoadSprite(const std::string& name, def::Graphic* sprite)
{
    m_mapSprites[name] = sprite;
}

def::Graphic* Assets::GetSprite(const std::string& name)
{
    return m_mapSprites[name];
}

std::unordered_map<std::string, def::Graphic*>& Assets::GetSprites()
{
    return m_mapSprites;
}

bool Assets::LoadConfig()
{
    sol::state& lua = ScriptsManager::Get().state;

    auto load_table = [](const sol::optional<sol::table>& optTable, const std::string& name, sol::table& tblTable)
        {
            if (!optTable)
            {
                logger::Error("Can't load table " + name);
                return false;
            }

            tblTable = optTable.value();
            return true;
        };

#define INIT_TABLE(var, source, name) sol::table var; if (!load_table(source, name, var)) return false;

    INIT_TABLE(tblAssets, lua["Assets"], "Assets");
    INIT_TABLE(tblTiles, tblAssets["Tiles"], "Assets.Tiles");
    INIT_TABLE(tblSprites, tblAssets["Sprites"], "Assets.Sprites");
    INIT_TABLE(tblLevels, tblAssets["Levels"], "Assets.Levels");
    INIT_TABLE(tblTileSize, tblTiles["Size"], "Assets.Tiles.Size");
    INIT_TABLE(tblFileOffsets, tblTiles["FileOffsets"], "Assets.Tiles.FileOffsets");
    
    vTileSize.x = tblTileSize[1];
    vTileSize.y = tblTileSize[2];

    for (const auto& [objTileType, objCoords] : tblFileOffsets)
    {
        TileType tile_type = objTileType.as<TileType>();
        const sol::table& tblCoords = objCoords.as<sol::table>();

        mapSpriteFileOffsets[tile_type].x = tblCoords[1];
        mapSpriteFileOffsets[tile_type].y = tblCoords[2];
    }

    Game& engine = Game::Get();

    engine.vecLevels.resize(tblLevels.size());

    for (const auto& [idx, objInfo] : tblLevels)
    {
        const sol::table& tblInfo = objInfo.as<sol::table>();

        INIT_TABLE(tblData, tblInfo["Data"], "Assets.Levels.<name>.Data");
        INIT_TABLE(tblSize, tblInfo["Size"], "Assets.Levels.<name>.Size");

        def::vi2d vSize = { tblSize[1], tblSize[2] };
        std::vector<TileType> vecTiles;

        for (int i = 0; i < vSize.x * vSize.y; i++)
            vecTiles.push_back(TileType(tblData[i + 1].get<int>()));

        engine.vecLevels[idx.as<size_t>() - 1] = new Level(vecTiles, vSize);
    }

    engine.itCurrentLevel = engine.vecLevels.begin();

    for (const auto& [name, path] : tblSprites)
        LoadSprite(name.as<std::string>(), new def::Graphic(path.as<std::string>()));

    sol::protected_function initialise = lua["Initialise"];
    if (initialise.valid()) initialise(); else return false;

    return true;
}

#undef INIT_TABLE
