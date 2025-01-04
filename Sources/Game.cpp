#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define DGE_APPLICATION
#include "../Include/Game.hpp"

Game& Game::Get()
{
    static Game game;
    return game;
}

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
    LoadConfig();

    return true;
}

bool Game::OnUserUpdate(float fDeltaTime)
{
    switch (m_State)
    {
    case GameState::Menu: State_Menu(); break;
    case GameState::Game: State_Game(); break;
    }

    return m_IsRunning;
}

void Game::DrawWorld()
{
    auto& assets = Assets::Get();

    DrawTexture({ 0, 0 }, assets.GetSprite("Background")->texture);

    def::vi2d visibleTiles = GetScreenSize() / Assets::Get().tileSize;

    def::vf2d offset = (m_Player->model.pos - (def::vf2d)visibleTiles * 0.5f)
        .max({ 0.0f, 0.0f })
        .min((*m_CurrentLevel)->GetSize() - visibleTiles);

    def::vf2d tileOffset = (offset - offset.floor()) * Assets::Get().tileSize;

    auto* tiles = assets.GetSprite("Tiles")->texture;

    def::vi2d tile;
    for (tile.y = -1; tile.y <= visibleTiles.y; tile.y++)
        for (tile.x = -1; tile.x <= visibleTiles.x; tile.x++)
        {
            TileType tileType = (*m_CurrentLevel)->GetTile(tile + (def::vi2d)offset);

            def::vi2d p = tile * Assets::Get().tileSize - tileOffset;

            if (tileType != TileType::Empty)
                DrawPartialTexture(p, tiles, assets.spriteFileOffsets[tileType], Assets::Get().tileSize);
        }

    for (const auto& dynamic : (*m_CurrentLevel)->dynamics)
    {
        DrawPartialTexture(
            (dynamic.dynamic->model.pos - offset) * Assets::Get().tileSize,
            tiles,
            Assets::Get().tileSize * dynamic.dynamic->graphicsID,
            dynamic.dynamic->model.size * Assets::Get().tileSize);
    }
}

void Game::DrawInterface()
{
    DrawTextureString({ 2, 2 }, "Score: " + std::to_string(m_Score));
}

void Game::State_Menu()
{
    if (GetKey(def::Key::ESCAPE).pressed)
    {
        m_MenuCursor = 0;
        m_State = GameState::Game;
    }

    std::unordered_map<std::string, size_t> options =
    {
        { "Continue", 0 },
        { "Credits", 1 },
        { "Exit", 2 }
    };

    std::vector<std::function<void()>> optionsActions =
    {
        [&]() { m_State = GameState::Game; },
        [&]() {},
        [&]() { m_IsRunning = false; }
    };

    if (GetKey(def::Key::UP).pressed || GetKey(def::Key::W).pressed)
    {
        m_MenuCursor--;

        if (m_MenuCursor < 0)
            m_MenuCursor = options.size() - 1;
    }

    if (GetKey(def::Key::DOWN).pressed || GetKey(def::Key::S).pressed)
    {
        m_MenuCursor++;

        if (m_MenuCursor >= options.size())
            m_MenuCursor = 0;
    }

    if (GetKey(def::Key::ENTER).pressed)
        optionsActions[m_MenuCursor]();

    ClearTexture(def::BLUE);

    for (const auto& [optionName, optionIndex] : options)
    {
        def::vi2d offset = { 10, 20 + 12 * (int)optionIndex };

        if (m_MenuCursor == optionIndex)
        {
            std::string pointer = "-> ";
            DrawTextureString(offset, pointer);
            offset.x += 10 * pointer.size();
        }

        DrawTextureString(offset, optionName, def::WHITE);
    }
}

void Game::State_Game()
{
    if (GetKey(def::Key::ESCAPE).pressed)
        m_State = GameState::Menu;

    auto& dynamics = (*m_CurrentLevel)->dynamics;

    for (auto& dynamic : dynamics)
    {
        if (dynamic.dynamic)
            dynamic.dynamic->Update();
    }

    dynamics.remove_if([](const Level::DynamicUnit& dynamic) { return dynamic.isRedundant; });

    DrawWorld();
    DrawInterface();
}

void Game::IncreaseScore()
{
    m_Score++;
}

bool Game::LoadConfig()
{
    if (!Assets::Get().LoadConfig())
        return false;

    sol::state& lua = ScriptsManager::Get().state;

    auto LoadTable = [](const sol::optional<sol::table>& wrappedTable, const std::string& name, sol::table& table)
        {
            if (!wrappedTable)
            {
                logger::Error("Can't load table " + name);
                return false;
            }

            table = wrappedTable.value();

            return true;
        };

#define INIT_TABLE(var, source, name) sol::table var; if (!LoadTable(source, name, var)) return false;

    INIT_TABLE(gameTable, lua["Game"], "Game");
    INIT_TABLE(velocityTable, gameTable["Velocity"], "Game.Velocity");
    INIT_TABLE(velocityPlayerTable, velocityTable["Player"], "Game.Velocity.Player");
    INIT_TABLE(velocityEnemyTable, velocityTable["Enemy"], "Game.Velocity.Enemy");

    auto LoadVelocity = []<class T>(sol::table & table)
    {
        T::s_GroundSpeed = table["GroundSpeed"].get_or(15.0f);
        T::s_AirSpeed = table["AirSpeed"].get_or(17.5f);
        T::s_JumpSpeed = table["JumpSpeed"].get_or(12.0f);
    };

    Dynamic::s_VelocityEpsilon = velocityTable["Epsilon"].get_or(1.0f);
    Dynamic::s_Friction = velocityTable["Friction"].get_or(4.0f);

    LoadVelocity.template operator() < Dynamic_Player > (velocityPlayerTable);
    LoadVelocity.template operator() < Dynamic_Enemy > (velocityEnemyTable);

    Dynamic::s_FallSpeed = velocityTable["FallSpeed"].get_or(20.0f);

    auto GetVelocityVector = [&](sol::optional<sol::table> table, def::vf2d& vector)
        {
            if (table)
            {
                sol::table& vectorTable = table.value();

                vector.x = vectorTable[1];
                vector.y = vectorTable[2];
            }
        };

    GetVelocityVector(velocityEnemyTable["Min"].get<sol::optional<sol::table>>(), Dynamic_Enemy::s_MinVelocity);
    GetVelocityVector(velocityEnemyTable["Max"].get<sol::optional<sol::table>>(), Dynamic_Enemy::s_MaxVelocity);

    GetVelocityVector(velocityPlayerTable["Min"].get<sol::optional<sol::table>>(), Dynamic_Player::s_MinVelocity);
    GetVelocityVector(velocityPlayerTable["Max"].get<sol::optional<sol::table>>(), Dynamic_Player::s_MaxVelocity);

    return true;
}

std::vector<Level*>& Game::GetLevels()
{
    return m_Levels;
}

std::vector<Level*>::iterator& Game::GetCurrentLevel()
{
    return m_CurrentLevel;
}

Dynamic*& Game::GetPlayer()
{
    return m_Player;
}

void Game::AddDynamicBack(size_t level, Dynamic* dynamic)
{
    m_Levels[level]->dynamics.push_back({ false, dynamic });
}

void Game::AddDynamicFront(size_t level, Dynamic* dynamic)
{
    m_Levels[level]->dynamics.push_front({ false, dynamic });
}
