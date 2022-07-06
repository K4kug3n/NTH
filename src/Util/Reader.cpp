#include "Util/Reader.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace Nth{
	std::vector<char> readBinaryFile(std::string const& filename) {
		std::ifstream input(filename, std::ios::binary);

		return std::vector<char>(std::istreambuf_iterator<char>(input), {});
	}
}

