#ifndef NTH_UTILS_READER_HPP
#define NTH_UTILS_READER_HPP

#include <vector>
#include <filesystem>

namespace Nth {
	std::vector<char> read_binary_file(std::filesystem::path const& path);
}

#endif
