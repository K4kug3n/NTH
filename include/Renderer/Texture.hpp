#ifndef NTH_RENDERER_TEXTURE_HPP
#define NTH_RENDERER_TEXTURE_HPP

#include <string_view>
#include <vector>

namespace Nth {
	struct Texture {
		std::string_view type;
		std::string_view path;
		std::vector<char> data;
		unsigned int height;
		unsigned int width;
	};

}

#endif
