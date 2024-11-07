#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#pragma comment(lib, "lua54.lib")

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
