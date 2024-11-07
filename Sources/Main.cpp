#include "../Include/Game.hpp"

void Initialise()
{
	auto& lua = ScriptsManager::Get().state;
	lua.new_enum("TileType", "Empty", 0, "Coin", 1, "Grass", 2, "Dirt", 3);
}

int main()
{
	Initialise();

	if (!ScriptsManager::Get().LoadScript("../../../../../Assets/config.lua"))
	{
		std::cerr << "[GAME] Can't load Assets/config.lua file\n";
		return 1;
	}

	auto& lua = ScriptsManager::Get().state;

	std::optional<sol::table> optTableWindow = lua["Window"];

	if (!optTableWindow.has_value())
	{
		std::cerr << "[GAME] Can't find 'Window' table\n";
		return 1;
	}

	sol::table& tblWindow = optTableWindow.value();

	Game game;

	game.Construct(
		tblWindow["ScreenWidth"].get_or(256),
		tblWindow["ScreenHeight"].get_or(192),
		tblWindow["PixelWidth"].get_or(4),
		tblWindow["PixelHeight"].get_or(4)
	);

	game.Run();
	return 0;
}
