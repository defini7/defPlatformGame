#include "../Include/Game.hpp"

void Initialise()
{
	// Here we are loading some enums and functions to the Lua

	auto& lua = ScriptsManager::Get().state;
	lua.new_enum("TileType", "Empty", 0, "Coin", 1, "Grass", 2, "Dirt", 3);

	lua["SpawnPlayer"] = [](size_t index, float x, float y)
		{
			Game& engine = Game::Get();

			engine.AddDynamicFront(index - 1, new Dynamic_Player({ x, y }));
			engine.GetPlayer() = engine.GetLevels()[index - 1]->dynamics.begin()->dynamic;
		};

	lua["SpawnMushroom"] = [](size_t index, float x, float y)
		{
			Game::Get().AddDynamicBack(index - 1, new Dynamic_Enemy_Mushroom({ x, y }));
		};

	lua["SpawnTurtle"] = [](size_t index, float x, float y)
		{
			Game::Get().AddDynamicBack(index - 1, new Dynamic_Enemy_Turtle({ x, y }));
		};
}

int main()
{
	Initialise();

	if (!ScriptsManager::Get().LoadScript("../../Assets/config.lua"))
	{
		logger::Error("Can't load ../../Assets/config.lua file");
		return 1;
	}

	auto& lua = ScriptsManager::Get().state;

	std::optional<sol::table> wrappedTableWindow = lua["Window"];

	if (!wrappedTableWindow)
	{
		logger::Error("Can't find 'Window' table\n");
		return 1;
	}

	sol::table& windowTable = wrappedTableWindow.value();

	Game& game = Game::Get();

	game.Construct(
		windowTable["ScreenWidth"].get_or(256),
		windowTable["ScreenHeight"].get_or(192),
		windowTable["PixelWidth"].get_or(4),
		windowTable["PixelHeight"].get_or(4)
	);

	game.Run();
	return 0;
}
