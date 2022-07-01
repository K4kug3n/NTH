#include "Util/Image.hpp"

#include <stb_image.h>

#include <iostream>
#include <memory>

namespace Nth{
	Image::Image(unsigned int width, unsigned int height, unsigned int channels, std::vector<unsigned char> const& pixels) :
		m_width(width),
		m_height(height),
		m_channels(channels),
		m_pixels(pixels) { }

	unsigned int Image::width() const {
		return m_width;
	}

	unsigned int Image::height() const {
		return m_height;
	}

	unsigned int Image::channels() const {
		return m_channels;
	}

	std::vector<unsigned char> const& Image::pixels() const {
		return m_pixels;
	}

	Image Image::loadFromFile(std::string const& filename, PixelChannel desiredChannel) {
		int width, height, channels;

		stbi_uc* pixels = stbi_load(filename.c_str(), &width, &height, &channels, static_cast<int>(desiredChannel));

		if (!pixels) {
			std::cout << "Failed to load texture file " << filename << std::endl;
			return Image();
		}

		std::vector<unsigned char> data = {};
		data.assign(pixels, pixels + (width * height * channels));

		stbi_image_free(pixels);

		return Image(width, height, channels, data);
	}
}

