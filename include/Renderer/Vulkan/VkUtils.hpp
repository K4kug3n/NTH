#ifndef NTH_RENDERER_VK_VKUTIL_HPP
#define NTH_RENDERER_VK_VKUTIL_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace Nth {
	namespace Vk {
		std::string to_string(VkResult result);
	}	
}

#endif