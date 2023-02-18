#include <Renderer/Texture.hpp>

#include <Utils/Image.hpp>
#include <Utils/Color.hpp>

#include <stdexcept>

namespace Nth {
	Texture texture_from_file(const std::filesystem::path& path) {
		Image image = Image::LoadFromFile(path, PixelChannel::Rgba);

		if (image.pixels().empty()) {
			throw std::runtime_error("Can't read file " + path.string());
		}

		Texture texture;
		texture.data = image.pixels();
		texture.height = image.height();
		texture.width = image.width();

		return texture;
	}

	Texture uniform_texture(const Color& color) {
		Texture texture;
		texture.width = 1;
		texture.height = 1;
		texture.data = { color.r, color.g, color.b, color.a };
		texture.type = "base_color";
		texture.path = "./";

		return texture;
	}
}