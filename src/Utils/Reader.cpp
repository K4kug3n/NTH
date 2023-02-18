#include <Utils/Reader.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>

namespace Nth{
	std::vector<char> read_binary_file(std::filesystem::path const& path) {
		std::ifstream input(path, std::ios::binary);

		return std::vector<char>(std::istreambuf_iterator<char>(input), {});
	}
}

