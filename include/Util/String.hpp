#pragma once

#ifndef NTH_STRING_HPP
#define NTH_STRING_HPP

#include <string>

namespace Nth {
	std::wstring toWString(std::string_view const& str);
}

#endif
