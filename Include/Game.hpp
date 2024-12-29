#pragma once

#pragma warning(disable : 4996)
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "defGeometry2D.hpp"
#include "defGameEngine.hpp"

#include "Logger.hpp"
#include "Assets.hpp"
#include "Dynamic.hpp"
#include "Level.hpp"
#include "ScriptsManager.hpp"

#include <unordered_map>
#include <functional>
#include <vector>

enum class GameState
{
    Menu,
    Game
};

class Dynamic;

class Game : public def::GameEngine
{
public:
	virtual ~Game();

    Game(Game const&) = delete;
    void operator=(Game const&) = delete;

    static Game& Get();

private:
    Game();

protected:
    bool OnUserCreate() override;
    bool OnUserUpdate(float fDeltaTime) override;

	void DrawWorld();
	void DrawInterface();

	void State_Menu();
	void State_Game();

    static bool LoadConfig();

public:
    void AddDynamic_front(size_t nLevel, Dynamic* pDynamic);
    void AddDynamic_back(size_t nLevel, Dynamic* pDynamic);

public:
    GameState nState = GameState::Menu;
    bool bRunning = true;

    int nScore = 0;
	int nMenuCursor = 0;

    std::vector<Level*> vecLevels;
    std::vector<Level*>::iterator itCurrentLevel;

    Dynamic* pPlayer;

};
