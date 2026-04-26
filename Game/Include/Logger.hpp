/*-----------------------------------------------------------------
 *  Copyright 2026 defini7. All rights reserved.
 *  Licensed under the GNU General Public License v3.0.
 *  See LICENSE file in the project root for license information.
 *----------------------------------------------------------------*/

#pragma once

#include <string>
#include <iostream>

namespace Logger
{
	void Print(const std::string& text, const std::string& type, std::ostream& os);

	void Log(const std::string& text, std::ostream& os = std::cout);
	void Warn(const std::string& text, std::ostream& os = std::cout);
	void Error(const std::string& text, std::ostream& os = std::cout);
}
