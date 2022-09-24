#include <Util/Reader.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>

namespace Nth{
	std::vector<char> readBinaryFile(std::filesystem::path const& path) {
		std::ifstream input(path, std::ios::binary);

		return std::vector<char>(std::istreambuf_iterator<char>(input), {});
	}
}

