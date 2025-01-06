#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "Game.hpp"

class Game;

class Menu
{
public:
    Menu(Menu&& const) = delete;
    void operator=(Menu const&) = delete;

    static Menu& Get();

    void SetOptions(const std::unordered_map<std::string, std::function<void()>>& options);
    
    void ResetCursor(const size_t& value = 0);
    void MoveCursorForward();
    void MoveCursorBack();

    void Update();
    void Draw();

private:
    Menu() = default;

private:
    // Here we store the order of the options
    std::unordered_map<std::string, size_t> m_Options;

    // and here we store the actions and access them
    // by indecies from m_Options map
    std::vector<std::function<void()>> m_OptionsActions;

    size_t m_Cursor = 0;

};
