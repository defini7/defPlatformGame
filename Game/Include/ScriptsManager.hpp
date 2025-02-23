#ifndef SCRIPTS_MANAGER_HPP
#define SCRIPTS_MANAGER_HPP

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

#endif
