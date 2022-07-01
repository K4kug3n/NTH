#pragma once

#ifndef NTH_READER_HPP
#define NTH_READER_HPP

#include <vector>
#include <string>

namespace Nth {
	std::vector<char> readBinaryFile(std::string const& filename);
}

#endif
