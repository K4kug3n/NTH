#ifndef NTH_RENDERER_VK_VULKANLOADER_HPP
#define NTH_RENDERER_VK_VULKANLOADER_HPP

#include <string>
#include <vector>

#include <Utils/Library.hpp>

#include <vulkan/vulkan.h>

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	#include <Windows.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	#include <xcb/xcb.h>
	#include <dlfcn.h>
	#include <cstdlib>
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <dlfcn.h>
	#include <cstdlib>
#endif

namespace Nth {
	namespace Vk {
		class VulkanLoader {
		public:
			VulkanLoader() = delete;
			~VulkanLoader() = delete;

			static bool initialize();
			static void unitialize();

			static std::vector<VkLayerProperties> enumerate_layer_properties();
			static std::vector<VkExtensionProperties> enumerate_extension_properties(char const* layer_name = nullptr);

			static PFN_vkVoidFunction get_instance_proc_addr(const VkInstance& instance, std::string_view name);
			static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

			#define NTH_RENDERER_VK_GLOBAL_FUNCTION(fun) static PFN_##fun fun;
			#define NTH_RENDERER_VK_GLOBAL_FUNCTION_OPTIONAL(fun) NTH_RENDERER_VK_GLOBAL_FUNCTION(fun)
			#include <Renderer/Vulkan/GlobalFunctions.inl>
		private:
			static Library m_vulkan;
		};
	}
}

#endif
