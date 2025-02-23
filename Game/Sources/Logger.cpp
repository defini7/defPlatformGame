#include "../Include/Logger.hpp"

namespace logger
{
	void Print(const std::string& text, const std::string& type, std::ostream& os)
	{
		os << '[' << type << "] " << text << std::endl;
	}

	void Log(const std::string& text, std::ostream& os)
	{
		Print(text, "LOG", os);
	}

	void Warn(const std::string& text, std::ostream& os)
	{
		Print(text, "WARN", os);
	}

	void Error(const std::string& text, std::ostream& os)
	{
		Print(text, "ERROR", os);
	}
}
