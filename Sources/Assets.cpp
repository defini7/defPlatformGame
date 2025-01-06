#include "../Include/Assets.hpp"

Assets& Assets::Get()
{
    static Assets assets;
    return assets;
}

void Assets::LoadSprite(const std::string& name, def::Graphic* sprite)
{
    m_Sprites[name] = sprite;
}

def::Graphic* Assets::GetSprite(const std::string& name)
{
    return m_Sprites[name];
}

std::unordered_map<std::string, def::Graphic*>& Assets::GetSprites()
{
    return m_Sprites;
}

bool Assets::LoadConfig()
{
    sol::state& lua = ScriptsManager::Get().state;

    auto load_table = [](const sol::optional<sol::table>& wrappedTable, const std::string& name, sol::table& table)
        {
            if (!wrappedTable)
            {
                logger::Error("Can't load table " + name);
                return false;
            }

            table = wrappedTable.value();
            return true;
        };

#define INIT_TABLE(var, source, name) sol::table var; if (!load_table(source, name, var)) return false;

    // Here we load all tables that we need to setup our game
    INIT_TABLE(assetsTable, lua["Assets"], "Assets");
    INIT_TABLE(tilesTable, assetsTable["Tiles"], "Assets.Tiles");
    INIT_TABLE(spritesTable, assetsTable["Sprites"], "Assets.Sprites");
    INIT_TABLE(levelsTable, assetsTable["Levels"], "Assets.Levels");
    INIT_TABLE(tileSizeTable, tilesTable["Size"], "Assets.Tiles.Size");
    INIT_TABLE(fileOffsetsTable, tilesTable["FileOffsets"], "Assets.Tiles.FileOffsets");
    
    // Getting the size of each tile in the game
    tileSize.x = tileSizeTable[1];
    tileSize.y = tileSizeTable[2];

    // Getting offset of each tile in the file
    for (const auto& [tileTypeObj, coords] : fileOffsetsTable)
    {
        TileType tileType = tileTypeObj.as<TileType>();
        const sol::table& tblCoords = coords.as<sol::table>();

        spriteFileOffsets[tileType].x = tblCoords[1];
        spriteFileOffsets[tileType].y = tblCoords[2];
    }

    auto& engine = Game::Get();
    auto& levels = engine.GetLevels();

    levels.resize(levelsTable.size());

    for (const auto& [indexObj, infoObj] : levelsTable)
    {
        const sol::table& info = infoObj.as<sol::table>();

        // Getting size and content of each level
        INIT_TABLE(dataTable, info["Data"], "Assets.Levels.<name>.Data");
        INIT_TABLE(sizeTable, info["Size"], "Assets.Levels.<name>.Size");

        def::vi2d size = { sizeTable[1], sizeTable[2] };
        std::vector<TileType> tiles;

        for (int i = 0; i < size.x * size.y; i++)
            tiles.push_back(TileType(dataTable[i + 1].get<int>()));

        levels[indexObj.as<size_t>() - 1] = new Level(tiles, size);
    }

#undef INIT_TABLE
    
    // So the first level in the game is the first loaded level in here
    engine.GetCurrentLevel() = levels.begin();

    for (const auto& [nameObj, pathObj] : spritesTable)
    {
        LoadSprite(
            nameObj.as<std::string>(),
            new def::Graphic(pathObj.as<std::string>()));
    }

    // In Initialise function we spawn enemies
    sol::protected_function Initialise = lua["Initialise"];
    if (!Initialise.valid()) return false;
        
    Initialise();

    return true;
}
