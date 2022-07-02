#pragma once

#ifndef NTH_VKUTIL_HPP
#define NTH_VKUTIL_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace Nth {
	std::string toString(VkResult const& result);
}

#endif