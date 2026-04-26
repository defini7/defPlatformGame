/*-----------------------------------------------------------------
 *  Copyright 2026 defini7. All rights reserved.
 *  Licensed under the GNU General Public License v3.0.
 *  See LICENSE file in the project root for license information.
 *----------------------------------------------------------------*/

#pragma once

#include "defGeometry2D.hpp"
#include "defGameEngine.hpp"

#include "Logger.hpp"
#include "Assets.hpp"
#include "Dynamic.hpp"
#include "Level.hpp"
#include "ScriptsManager.hpp"
#include "Snow.hpp"
#include "Menu.hpp"

#include <unordered_map>
#include <functional>
#include <vector>

enum class GameState
{
    Menu,
    Game
};

class Dynamic;
class Menu;

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
    void AddDynamicFront(size_t level, Dynamic* dynamic);
    void AddDynamicBack(size_t level, Dynamic* dynamic);

    std::vector<Level*>& GetLevels();
    std::vector<Level*>::iterator& GetCurrentLevel();
    Dynamic*& GetPlayer();

    void IncreaseScore();

private:
    GameState m_State = GameState::Menu;
    bool m_IsRunning = true;

    int m_Score = 0;

    std::vector<Level*> m_Levels;
    std::vector<Level*>::iterator m_CurrentLevel;

    Dynamic* m_Player;

};
