#include <Utils/Image.hpp>

//#include <Utils/Reader.hpp>

#include <stb_image.h>

#include <iostream>

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

	const std::vector<unsigned char>& Image::pixels() const {
		return m_pixels;
	}

	Image Image::LoadFromFile(const std::filesystem::path& path, PixelChannel desired_channel) {
		/*std::vector<char> file_data = read_binary_file(path);
		if (file_data.size() == 0) {
			return Image();
		}*/

		int width = 0;
		int height = 0;
		int components = 0;

		//unsigned char* image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &width, &height, &components, static_cast<int>(desired_channel));
		unsigned char* image_data = stbi_load(path.string().c_str(), &width, &height, &components, static_cast<int>(desired_channel));
		if ((image_data == nullptr) ||
			(width <= 0) ||
			(height <= 0) ||
			(components <= 0)) {
			std::cout << "Could not read image data!" << std::endl;
			stbi_image_free(image_data);
			return Image();
		}

		int size = (width) * (height) * (static_cast<int>(desired_channel) <= 0 ? components : static_cast<int>(desired_channel));

		std::vector<unsigned char> output(size);
		std::memcpy(&output[0], image_data, size);

		stbi_image_free(image_data);
		return Image{ static_cast<unsigned>(width), static_cast<unsigned>(height), static_cast<unsigned>(desired_channel), output };
	}
}

