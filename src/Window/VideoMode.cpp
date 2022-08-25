#include <Window/VideoMode.hpp>

#include <SDL2/SDL_video.h>

#include <iostream>
#include <algorithm>

namespace Nth {
	VideoMode::VideoMode() :
		width(0), height(0) {}

	VideoMode::VideoMode(unsigned int width, unsigned int height) :
		width(width), height(height) {}

	VideoMode VideoMode::getDesktopMode() {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(0, &mode) < 0) {
			std::cerr << SDL_GetError() << std::endl;
			return VideoMode();
		}

		return VideoMode(mode.w, mode.h);
	}

	std::vector<VideoMode> VideoMode::getDisplayModes() {
		int numMode{ SDL_GetNumDisplayModes(0) };
		if (numMode < 0) {
			std::cerr << SDL_GetError() << std::endl;
			return std::vector<VideoMode>();
		}

		std::vector<VideoMode> vModes;
		SDL_DisplayMode mode;
		for (int i{ 0 }; i < numMode; i++) {
			if (SDL_GetDisplayMode(0, i, &mode) < 0) {
				std::cerr << SDL_GetError() << std::endl;
				return std::vector<VideoMode>();
			}

			VideoMode vMode{ static_cast<unsigned int>(mode.w), static_cast<unsigned int>(mode.h) };
			if (std::find(std::begin(vModes), std::end(vModes), vMode) == std::end(vModes)) {
				vModes.push_back(std::move(vMode));
			}
		}

		std::sort(begin(vModes), std::end(vModes), std::greater<VideoMode>());

		return vModes;
	}

	bool operator==(VideoMode const& l, VideoMode const& r) {
		return l.width == r.width && l.height == r.height;
	}

	bool operator>(VideoMode const& l, VideoMode const& r) {
		if (l.width == r.width) {
			return l.height > r.height;
		}
		else {
			return l.width > r.width;
		}
	}

}

