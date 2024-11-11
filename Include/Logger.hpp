#pragma once

#include <string>
#include <iostream>

namespace logger
{
	void Print(const std::string& text, const std::string& type, std::ostream& os);

	void Log(const std::string& text, std::ostream& os = std::cout);
	void Warn(const std::string& text, std::ostream& os = std::cout);
	void Error(const std::string& text, std::ostream& os = std::cout);
}
