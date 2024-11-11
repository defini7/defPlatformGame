#include "../Include/ScriptsManager.hpp"

ScriptsManager::ScriptsManager()
{
	state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table, sol::lib::string);
}

ScriptsManager& ScriptsManager::Get()
{
	static ScriptsManager me;
	return me;
}

bool ScriptsManager::LoadScript(const std::string& filename)
{
	try
	{
		state.safe_script_file(filename);
	}
	catch (const sol::error& e)
	{
		logger::Error(e.what());
		return false;
	}

	return true;
}
