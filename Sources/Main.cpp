#include "../Include/Game.hpp"

void InitialiZe()
{
	auto& lua = ScriptsManager::Get().state;
	lua.new_enum("TileType", "Empty", 0, "Coin", 1, "Grass", 2, "Dirt", 3);

	lua["SpawnPlayer"] = [](size_t index, float x, float y)
		{
			Game& engine = Game::Get();

			engine.AddDynamic_front(index - 1, new Dynamic_Player({ x, y }));
			engine.pPlayer = engine.vecLevels[index - 1]->listDynamics.begin()->pDynamic;
		};

	lua["SpawnMushroom"] = [](size_t index, float x, float y)
		{
			Game::Get().AddDynamic_back(index - 1, new Dynamic_Enemy_Mushroom({ x, y }));
		};

	lua["SpawnTurtle"] = [](size_t index, float x, float y)
		{
			Game::Get().AddDynamic_back(index - 1, new Dynamic_Enemy_Turtle({ x, y }));
		};
}

int main()
{
	InitialiZe();

	if (!ScriptsManager::Get().LoadScript("Assets/config.lua"))
	{
		logger::Error("Can't load Assets/config.lua file");
		return 1;
	}

	auto& lua = ScriptsManager::Get().state;

	std::optional<sol::table> optTableWindow = lua["Window"];

	if (!optTableWindow)
	{
		logger::Error("Can't find 'Window' table\n");
		return 1;
	}

	sol::table& tblWindow = optTableWindow.value();

	Game& game = Game::Get();

	game.Construct(
		tblWindow["ScreenWidth"].get_or(256),
		tblWindow["ScreenHeight"].get_or(192),
		tblWindow["PixelWidth"].get_or(4),
		tblWindow["PixelHeight"].get_or(4)
	);

	game.Run();
	return 0;
}
