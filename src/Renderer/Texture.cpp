#include <Renderer/Texture.hpp>

#include <Util/Image.hpp>

#include <stdexcept>

namespace Nth {
	Texture textureFromFile(std::string_view filepath) {
		Image image = Image::loadFromFile(filepath, PixelChannel::Rgba);

		if (image.pixels().empty()) {
			throw std::runtime_error("Can't read file " + std::string{ filepath });
		}

		Texture texture;
		texture.data = image.pixels();
		texture.height = image.height();
		texture.width = image.width();

		return texture;
	}
}