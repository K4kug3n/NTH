#include <Renderer/Texture.hpp>

#include <Util/Image.hpp>
#include <Util/Color.hpp>

#include <stdexcept>

namespace Nth {
	Texture textureFromFile(std::filesystem::path const& path) {
		Image image = Image::loadFromFile(path, PixelChannel::Rgba);

		if (image.pixels().empty()) {
			throw std::runtime_error("Can't read file " + path.string());
		}

		Texture texture;
		texture.data = image.pixels();
		texture.height = image.height();
		texture.width = image.width();

		return texture;
	}

	Texture uniformTexture(Color const& color) {
		Texture texture;
		texture.width = 1;
		texture.height = 1;
		texture.data = { color.r, color.g, color.b, color.a };
		texture.type = "base_color";
		texture.path = "./";

		return texture;
	}
}