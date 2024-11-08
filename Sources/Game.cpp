#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define DGE_APPLICATION
#include "../Include/Game.hpp"

Game::Game()
{
    SetTitle("Platform Game");
    UseOnlyTextures(true);
}

Game::~Game()
{
}

bool Game::OnUserCreate()
{
    Dynamic::s_pEngine = this;
    Assets& assets = Assets::Get();

    LoadConfig();

    vecLevels =
    {
        assets.GetLevel("Home"), assets.GetLevel("Next")
    };

    itCurrentLevel = vecLevels.begin();

    vecDynamics.push_back(new Dynamic_Player({ 1.0f, 1.0f }, { 0.0f, 0.0f }));
    itPlayer = vecDynamics.begin();

    return true;
}

bool Game::OnUserUpdate(float fDeltaTime)
{
    switch (nState)
    {
    case GameState::Menu: State_Menu(); break;
    case GameState::Game: State_Game(); break;
    }

    return bRunning;
}

void Game::DrawWorld()
{
    auto& assets = Assets::Get();

    DrawTexture({ 0, 0 }, assets.GetSprite("Background")->texture);

    def::vi2d vVisibleTiles = GetScreenSize() / Assets::Get().vTileSize;

    def::vf2d vOffset = ((*itPlayer)->vPos - (def::vf2d)vVisibleTiles * 0.5f)
        .max({ 0.0f, 0.0f })
        .min((*itCurrentLevel)->GetSize() - vVisibleTiles);

    def::vf2d vTileOffset = (vOffset - vOffset.floor()) * Assets::Get().vTileSize;

    auto* pTiles = assets.GetSprite("Tiles")->texture;

    def::vi2d vTile;
    for (vTile.y = -1; vTile.y <= vVisibleTiles.y; vTile.y++)
    {
        for (vTile.x = -1; vTile.x <= vVisibleTiles.x; vTile.x++)
        {
            TileType tile = (*itCurrentLevel)->GetTile(vTile + (def::vi2d)vOffset);

            def::vi2d p = vTile * Assets::Get().vTileSize - vTileOffset;

            if (tile != TileType::Empty)
                DrawPartialTexture(p, pTiles, assets.mapSpriteFileOffsets[tile], Assets::Get().vTileSize);
        }
    }

    for (const auto& pDynamic : vecDynamics)
        DrawPartialTexture((pDynamic->vPos - vOffset) * Assets::Get().vTileSize, pTiles, Assets::Get().vTileSize * pDynamic->vGraphicsID, Assets::Get().vTileSize);
}

void Game::DrawInterface()
{
    DrawTextureString({ 2, 2 }, "Score: " + std::to_string(nScore));
}

void Game::State_Menu()
{
    if (GetKey(def::Key::ESCAPE).pressed)
    {
        nMenuCursor = 0;
        nState = GameState::Game;
    }

    std::unordered_map<std::string, size_t> mapOptions =
    {
        { "Continue", 0 },
        { "Credits", 1 },
        { "Exit", 2 }
    };

    std::vector<std::function<void()>> vecOptions =
    {
        [&]() { nState = GameState::Game; },
        [&]() {},
        [&]() { bRunning = false; }
    };

    if (GetKey(def::Key::UP).pressed)
    {
        nMenuCursor--;
        if (nMenuCursor < 0) nMenuCursor = mapOptions.size() - 1;
    }

    if (GetKey(def::Key::DOWN).pressed)
    {
        nMenuCursor++;
        if (nMenuCursor >= mapOptions.size()) nMenuCursor = 0;
    }

    if (GetKey(def::Key::ENTER).pressed)
        vecOptions[nMenuCursor]();

    ClearTexture(def::BLUE);

    for (const auto& [sOptionName, nOptionIndex] : mapOptions)
    {
        def::vi2d vOffset = { 10, 20 + 12 * (int)nOptionIndex };

        if (nMenuCursor == nOptionIndex)
        {
            std::string sPointer = "-> ";
            DrawTextureString(vOffset, sPointer);
            vOffset.x += 10 * sPointer.size();
        }

        DrawTextureString(vOffset, sOptionName, def::WHITE);
    }
}

void Game::State_Game()
{
    if (GetKey(def::Key::ESCAPE).pressed)
        nState = GameState::Menu;

    for (auto& pDynamic : vecDynamics)
        pDynamic->Update();

    DrawWorld();
    DrawInterface();
}

bool Game::LoadConfig()
{
    if (!Assets::Get().LoadConfig())
        return false;

    sol::state& lua = ScriptsManager::Get().state;
    sol::optional<sol::table> optTable;

    optTable = lua["Game"].get<decltype(optTable)>();
    if (!optTable)
    {
        std::cerr << "[GAME] Can't find Game table";
        return false;
    }

    sol::table& tblGame = optTable.value();

    optTable = tblGame["Velocity"].get<decltype(optTable)>();
    if (!optTable)
    {
        std::cerr << "[GAME] Can't find Game.Velocity table";
        return false;
    }

    sol::table& tblVelocity = optTable.value();

    Dynamic::s_fVelocityEpsilon = tblVelocity["Epsilon"].get_or(1.0f);
    Dynamic::s_fFriction = tblVelocity["Friction"].get_or(4.0f);
    Dynamic::s_fGroundSpeed = tblVelocity["GroundSpeed"].get_or(15.0f);
    Dynamic::s_fAirSpeed = tblVelocity["AirSpeed"].get_or(17.5f);
    Dynamic::s_fJumpSpeed = tblVelocity["JumpSpeed"].get_or(12.0f);
    Dynamic::s_fFallSpeed = tblVelocity["FallSpeed"].get_or(20.0f);

    auto Velocity_GetVector = [&](const char* name, def::vf2d& vector)
        {
            sol::optional<sol::table> optTable = tblVelocity[name].get<decltype(optTable)>();

            if (optTable)
            {
                sol::table& tblVector = optTable.value();
                vector.x = tblVector[1];
                vector.y = tblVector[2];
            }
        };

    Velocity_GetVector("Min", Dynamic::s_vMinVelocity);
    Velocity_GetVector("Max", Dynamic::s_vMaxVelocity);
}
