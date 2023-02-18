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

	Texture texture_from_file(const std::filesystem::path& path);
	Texture uniform_texture(const Color& color);
}

#endif
