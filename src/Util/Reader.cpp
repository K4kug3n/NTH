#include <Util/Reader.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>

namespace Nth{
	std::vector<char> readBinaryFile(const std::string_view filename) {
		std::ifstream input(filename.data(), std::ios::binary);

		return std::vector<char>(std::istreambuf_iterator<char>(input), {});
	}
}

