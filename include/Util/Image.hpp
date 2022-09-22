#ifndef NTH_UTIL_IMAGE_HPP
#define NTH_UTIL_IMAGE_HPP

#include <string_view>
#include <vector>
#include <memory>

namespace Nth {
	enum class PixelChannel {
		Unknow = 0,
		Grey = 1,
		Greya = 2,
		Rgb = 3,
		Rgba = 4
	};

	class Image {
	public:
		Image() = default;
		Image(unsigned int width, unsigned int height, unsigned int channels, std::vector<unsigned char> const& pixels);
		Image(Image const&) = default;
		Image(Image&&) = default;
		~Image() = default;

		unsigned int width() const;
		unsigned int height() const;
		unsigned int channels() const;
		std::vector<unsigned char> const& pixels() const;

		static Image loadFromFile(const std::string_view filename, PixelChannel desiredChannel = PixelChannel::Unknow);

		Image& operator=(Image const&) = default;
		Image& operator=(Image&&) = default;

	private:
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_channels;
		std::vector<unsigned char> m_pixels;
	};
}

#endif
