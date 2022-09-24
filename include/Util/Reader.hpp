#ifndef NTH_UTIL_READER_HPP
#define NTH_UTIL_READER_HPP

#include <vector>
#include <filesystem>

namespace Nth {
	std::vector<char> readBinaryFile(std::filesystem::path const& path);
}

#endif
