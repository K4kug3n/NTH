#include <Renderer/Vulkan/VkUtil.hpp>

namespace Nth {
	namespace Vk {
		std::string toString(VkResult result) {
			switch (result) {
			case VkResult::VK_SUCCESS:
				return "No error";

			case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:
				return "Out of host memory";

			case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return "Out of device memory";

			case VkResult::VK_ERROR_INITIALIZATION_FAILED:
				return "Initialization failed";

			case VkResult::VK_ERROR_LAYER_NOT_PRESENT:
				return "Layer not present";

			case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:
				return "Extension not present";

			case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:
				return "Driver incompatible";

			default:
				return "Unmanaged error";
			}
		}
	}	
}
