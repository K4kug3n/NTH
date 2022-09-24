#ifndef NTH_RENDERER_TEXTURE_HPP
#define NTH_RENDERER_TEXTURE_HPP

#include <string_view>
#include <filesystem>
#include <vector>

namespace Nth {
	struct Color;

	struct Texture {
		std::string_view type;
		std::filesystem::path path;
		std::vector<unsigned char> data;
		unsigned int height;
		unsigned int width;
	};

	Texture textureFromFile(std::filesystem::path const& path);
	Texture uniformTexture(Color const& color);
}

#endif
