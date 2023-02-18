#ifndef NTH_UTILS_IMAGE_HPP
#define NTH_UTILS_IMAGE_HPP

#include <filesystem>
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
		Image(const Image&) = default;
		Image(Image&&) = default;
		~Image() = default;

		unsigned int width() const;
		unsigned int height() const;
		unsigned int channels() const;
		const std::vector<unsigned char>& pixels() const;

		static Image LoadFromFile(const std::filesystem::path& path, PixelChannel desiredChannel = PixelChannel::Unknow);

		Image& operator=(const Image&) = default;
		Image& operator=(Image&&) = default;

	private:
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_channels;
		std::vector<unsigned char> m_pixels;
	};
}

#endif
