#include "../Include/Menu.hpp"

Menu& Menu::Get()
{
    static Menu menu;
    return menu;
}

void Menu::SetOptions(const std::list<std::pair<std::string, std::function<void()>>>& options)
{
    m_OptionsActions.resize(options.size());

    for (const auto& option : options)
    {
        size_t index = m_Options.size();

        m_Options[option.first] = index;
        m_OptionsActions[index] = option.second;
    }
}

void Menu::ResetCursor(const size_t& value)
{
    m_Cursor = value;
}

void Menu::MoveCursorForward()
{
    m_Cursor++;
}

void Menu::MoveCursorBack()
{
    m_Cursor--;
}

void Menu::Update()
{
    Game& game = Game::Get();

    // Here we move across the menu

    if (game.GetInput()->GetKeyState(def::Key::UP).pressed || game.GetInput()->GetKeyState(def::Key::W).pressed)
    {
        if (m_Cursor == 0)
            m_Cursor = m_Options.size() - 1;
        else
            m_Cursor--;
    }

    if (game.GetInput()->GetKeyState(def::Key::DOWN).pressed || game.GetInput()->GetKeyState(def::Key::S).pressed)
    {
        if (m_Cursor >= m_Options.size())
            m_Cursor = 0;
        else
            m_Cursor++;  
    }

    if (game.GetInput()->GetKeyState(def::Key::ENTER).pressed)
        m_OptionsActions[m_Cursor]();
}

void Menu::Draw()
{
    Game& game = Game::Get();

    // Firstly, draw the background, ...
    game.ClearTexture(def::BLUE);

    // secondly, draw the options
    // and highlight the picked one
    for (const auto& [name, index] : m_Options)
    {
        def::Vector2i offset = { 10, 20 + 12 * (int)index };

        if (m_Cursor == index)
        {
            std::string pointer = "-> ";
            game.DrawTextureString(offset, pointer);
            offset.x += 10 * pointer.size();
        }

        game.DrawTextureString(offset, name, def::WHITE);
    }
}
