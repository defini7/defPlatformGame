/*-----------------------------------------------------------------
 *  Copyright 2026 defini7. All rights reserved.
 *  Licensed under the GNU General Public License v3.0.
 *  See LICENSE file in the project root for license information.
 *----------------------------------------------------------------*/

#include "../Include/ScriptsManager.hpp"

ScriptsManager::ScriptsManager()
{
	// loading some basic lua libraries, i'm not sure which we will use so load
	// basic ones except math
	state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table, sol::lib::string);
}

ScriptsManager& ScriptsManager::Get()
{
	static ScriptsManager manager;
	return manager;
}

bool ScriptsManager::LoadScript(const std::string& filename)
{
	try
	{
		state.safe_script_file(filename);
	}
	catch (const sol::error& e)
	{
		Logger::Error(e.what());

		return false;
	}

	return true;
}
