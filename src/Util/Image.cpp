#include "Util/Image.hpp"

#include "Util/Reader.hpp"

#include <stb_image.h>

#include <iostream>
#include <memory>

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

	std::shared_ptr<Image> Image::loadFromFile(std::string const& filename, PixelChannel desiredChannel) {
		std::vector<char> file_data = readBinaryFile(filename);
		if (file_data.size() == 0) {
			return std::make_shared<Image>();
		}

		int tmp_width = 0;
		int tmp_height = 0;
		int tmp_components = 0;
		unsigned char* image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &tmp_width, &tmp_height, &tmp_components, static_cast<int>(desiredChannel));
		if ((image_data == nullptr) ||
			(tmp_width <= 0) ||
			(tmp_height <= 0) ||
			(tmp_components <= 0)) {
			std::cout << "Could not read image data!" << std::endl;
			return std::make_shared<Image>();
		}

		int size = (tmp_width) * (tmp_height) * (static_cast<int>(desiredChannel) <= 0 ? tmp_components : static_cast<int>(desiredChannel));

		std::vector<char> output(size);
		memcpy(&output[0], image_data, size);

		stbi_image_free(image_data);
		return std::make_shared<Image>(tmp_width, tmp_height, static_cast<int>(desiredChannel), output);
	}
}

