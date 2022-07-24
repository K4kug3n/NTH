#ifndef NTH_UTIL_READER_HPP
#define NTH_UTIL_READER_HPP

#include <vector>
#include <string>

namespace Nth {
	std::vector<char> readBinaryFile(const std::string_view filename);
}

#endif
