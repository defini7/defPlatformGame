#pragma once

#pragma warning(disable : 4996)
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "../Include/defGameEngine.hpp"

#include "Assets.hpp"
#include "Dynamic.hpp"
#include "Level.hpp"
#include "ScriptsManager.hpp"

#include <unordered_map>
#include <functional>

enum class GameState
{
    Menu,
    Game
};

class Dynamic;

class Game : public def::GameEngine
{
public:
	Game();
	virtual ~Game();

protected:
    bool OnUserCreate() override;
    bool OnUserUpdate(float fDeltaTime) override;

	void DrawWorld();
	void DrawInterface();

	void State_Menu();
	void State_Game();

    static bool LoadConfig();

public:
	static constexpr size_t TILES_COUNT = 4;

    GameState nState = GameState::Menu;
    bool bRunning = true;

    std::list<Dynamic*> vecDynamics;
    std::list<Dynamic*>::iterator itPlayer;

    int nScore = 0;
	int nMenuCursor = 0;

    std::vector<Level*> vecLevels;
    std::vector<Level*>::iterator itCurrentLevel;

};
