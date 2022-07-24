#include "Util/Image.hpp"

#include "Util/Reader.hpp"

#include <stb_image.h>

#include <iostream>

namespace Nth{
	Image::Image(unsigned int width, unsigned int height, unsigned int channels, std::vector<char> const& pixels) :
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

	std::vector<char> const& Image::pixels() const {
		return m_pixels;
	}

	Image Image::loadFromFile(const std::string_view filename, PixelChannel desiredChannel) {
		std::vector<char> file_data = readBinaryFile(filename);
		if (file_data.size() == 0) {
			return Image();
		}

		int width = 0;
		int height = 0;
		int components = 0;
		unsigned char* imageData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &width, &height, &components, static_cast<int>(desiredChannel));
		if ((imageData == nullptr) ||
			(width <= 0) ||
			(height <= 0) ||
			(components <= 0)) {
			std::cout << "Could not read image data!" << std::endl;
			stbi_image_free(imageData);
			return Image();
		}

		int size = (width) * (height) * (static_cast<int>(desiredChannel) <= 0 ? components : static_cast<int>(desiredChannel));

		std::vector<char> output(size);
		memcpy(&output[0], imageData, size);

		stbi_image_free(imageData);
		return Image{ static_cast<unsigned>(width), static_cast<unsigned>(height), static_cast<unsigned>(desiredChannel), output };
	}
}

