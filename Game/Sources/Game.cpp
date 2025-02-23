#define DGE_APPLICATION
#include "../Include/Game.hpp"

Game& Game::Get()
{
    static Game game;
    return game;
}

Game::Game()
{
    GetWindow()->SetTitle("Platform Game");
    UseOnlyTextures(true);
}

Game::~Game()
{
}

bool Game::OnUserCreate()
{
    // The menu consists of the 3 options,
    // each option is choosable by the ENTER key

    Menu::Get().SetOptions({
        { "Continue", [&]() { m_State = GameState::Game; } },
        { "Credits", []() {} },
        { "Exit", [&]() { m_IsRunning = false; } }
    });

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

    // In that mess first of all we draw the background
    DrawTexture({ 0, 0 }, assets.GetSprite("Background")->texture);

    // now we draw snow
    Snow::Get().Draw();

    // and here we prepare for drawing the tiles and dynamic objects
    def::Vector2i visibleTiles = GetWindow()->GetScreenSize() / Assets::Get().tileSize;

    def::Vector2f offset = (def::Vector2f(m_Player->model.pos.x, m_Player->model.pos.y) - visibleTiles * 0.5f)
        .Max({ 0.0f, 0.0f })
        .Min((*m_CurrentLevel)->GetSize() - visibleTiles);

    def::Vector2f tileOffset = (offset - offset.Floor()) * Assets::Get().tileSize;

    auto* tiles = assets.GetSprite("Tiles")->texture;

    // Now we draw the tiles ...
    def::Vector2i tile;
    for (tile.y = -1; tile.y <= visibleTiles.y; tile.y++)
        for (tile.x = -1; tile.x <= visibleTiles.x; tile.x++)
        {
            TileType tileType = (*m_CurrentLevel)->GetTile(tile + (def::Vector2i)offset);

            def::Vector2i p = tile * Assets::Get().tileSize - tileOffset;

            if (tileType != TileType::Empty)
                DrawPartialTexture(p, tiles, assets.spriteFileOffsets[tileType], Assets::Get().tileSize);
        }

    // ... and here we draw the dynamic objects
    for (const auto& dynamic : (*m_CurrentLevel)->dynamics)
    {
        DrawPartialTexture(
            (def::Vector2f(dynamic.dynamic->model.pos.x, dynamic.dynamic->model.pos.y) - offset) * Assets::Get().tileSize,
            tiles,
            Assets::Get().tileSize * dynamic.dynamic->graphicsID,
            def::Vector2f(dynamic.dynamic->model.size.x, dynamic.dynamic->model.size.y)  * Assets::Get().tileSize);
    }
}

void Game::DrawInterface()
{
    // There is nothing except the score hublot
    DrawTextureString({ 2, 2 }, "Score: " + std::to_string(m_Score));
}

void Game::State_Menu()
{
    auto& menu = Menu::Get();

    // If we press ESCAPE in the menu we get back to the game
    if (GetInput()->GetKeyState(def::Key::ESCAPE).pressed)
    {
        menu.ResetCursor();
        m_State = GameState::Game;
    }

    menu.Update();
    menu.Draw();
}

void Game::State_Game()
{
    // If we press ESCAPE key we go to the menu
    if (GetInput()->GetKeyState(def::Key::ESCAPE).pressed)
        m_State = GameState::Menu;

    auto& dynamics = (*m_CurrentLevel)->dynamics;

    // Update every dynamic object that still exists
    for (auto& dynamic : dynamics)
    {
        if (dynamic.dynamic)
            dynamic.dynamic->Update();
    }

    dynamics.remove_if([](const Level::DynamicUnit& dynamic) { return dynamic.isRedundant; });

    Snow::Get().Update(GetDeltaTime());

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

    // Let's hope aligning won't get wrong

#define INIT_TABLE(var, source, name) sol::table var; if (!LoadTable(source, name, var)) return false;

    INIT_TABLE(gameTable,           lua["Game"],             "Game"                );
    INIT_TABLE(velocityTable,       gameTable["Velocity"],   "Game.Velocity"       );
    INIT_TABLE(velocityPlayerTable, velocityTable["Player"], "Game.Velocity.Player");
    INIT_TABLE(velocityEnemyTable,  velocityTable["Enemy"],  "Game.Velocity.Enemy" );
    INIT_TABLE(snowTable,           gameTable["Snow"],       "Game.Snow"           );

#undef INIT_TABLE

    // Next we use some magic constants, errrr... sorry

    auto LoadVelocity = []<class T>(sol::table& table)
        {
            T::s_GroundSpeed = table["GroundSpeed"].get_or(15.0f);
            T::s_AirSpeed = table["AirSpeed"].get_or(17.5f);
            T::s_JumpSpeed = table["JumpSpeed"].get_or(12.0f);
        };

    Dynamic::s_VelocityEpsilon = velocityTable["Epsilon"].get_or(1.0f);
    Dynamic::s_Friction = velocityTable["Friction"].get_or(4.0f);
    Dynamic::s_FallSpeed = velocityTable["FallSpeed"].get_or(20.0f);

    // Pretty ugly but works!
    LoadVelocity.template operator()<Dynamic_Player>(velocityPlayerTable);
    LoadVelocity.template operator()<Dynamic_Enemy>(velocityEnemyTable);

    Snow::s_FlakeRadius = snowTable["Radius"].get_or(4);
    Snow::s_Speed = snowTable["Speed"].get_or(50.0f);

    auto GetVector = [&](sol::optional<sol::table> table, def::Vector2f& vector)
        {
            if (table)
            {
                sol::table& vectorTable = table.value();

                vector.x = vectorTable[1];
                vector.y = vectorTable[2];

                return true;
            }

            return false;
        };

    auto GetVelocityVector = [&](sol::table& table, def::Vector2f& vectorMin, def::Vector2f& vectorMax)
        {
            sol::optional<sol::table> unwrappedMin = table["Min"];
            sol::optional<sol::table> unwrappedMax = table["Max"];

            if (!GetVector(unwrappedMin, vectorMin)) return false;
            if (!GetVector(unwrappedMax, vectorMax)) return false;
            
            return true;
        };

    if (!GetVelocityVector(velocityEnemyTable, Dynamic_Enemy::s_MinVelocity, Dynamic_Enemy::s_MaxVelocity))
        return false;

    if (!GetVelocityVector(velocityPlayerTable, Dynamic_Player::s_MinVelocity, Dynamic_Player::s_MaxVelocity))
        return false;

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
