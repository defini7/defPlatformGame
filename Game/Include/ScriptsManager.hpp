/*-----------------------------------------------------------------
 *  Copyright 2026 defini7. All rights reserved.
 *  Licensed under the GNU General Public License v3.0.
 *  See LICENSE file in the project root for license information.
 *----------------------------------------------------------------*/

#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "Logger.hpp"

class ScriptsManager
{
public:
	static ScriptsManager& Get();

	ScriptsManager(ScriptsManager const&) = delete;
	void operator=(ScriptsManager const&) = delete;

	bool LoadScript(const std::string& filename);

private:
	ScriptsManager();
	~ScriptsManager() = default;

public:
	sol::state state;

};
