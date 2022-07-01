#include "Util/String.hpp"

#include <locale>
#include <codecvt>
#include <string>

namespace Nth{
	std::wstring toWString(std::string_view const& str){
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str.data());
	}
}

