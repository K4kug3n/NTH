#ifndef NTH_RENDERER_VK_LOADER_HPP
#define NTH_RENDERER_VK_LOADER_HPP

#include <string>
#include <vector>

#include "Util/Lib.hpp"

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

			static std::vector<VkLayerProperties> enumerateLayerProperties();
			static std::vector<VkExtensionProperties> enumerateExtensionProperties(char const* layerName = nullptr);

			static PFN_vkVoidFunction getInstanceProcAddr(VkInstance const& instance, const std::string_view name);

			#define NTH_RENDERER_VK_GLOBAL_FUNCTION(fun) static PFN_##fun fun;
			#define NTH_RENDERER_VK_GLOBAL_FUNCTION_OPTIONAL(fun) NTH_RENDERER_VK_GLOBAL_FUNCTION(fun)
			#include "Renderer/Vulkan/GlobalFunctions.inl"
		private:
			static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

			static Lib m_vulkan;
		};
	}
}

#endif
