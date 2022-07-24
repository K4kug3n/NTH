#ifndef NTH_RENDERER_VK_SURFACE_HPP
#define NTH_RENDERER_VK_SURFACE_HPP

#include "Renderer/Vulkan/Instance.hpp"

#include <vulkan/vulkan.h>

namespace Nth {
	
	struct WindowHandle;

	namespace Vk {
		class PhysicalDevice;
	
		class Surface {
		public:
			Surface(Instance const& instance);
			Surface(Surface const&) = delete;
			~Surface();

			bool create(WindowHandle const& infos);

			bool getPhysicalDeviceSurfaceSupport(PhysicalDevice const& device, uint32_t queueFamilyIndex, bool& supported) const;
			bool getCapabilities(PhysicalDevice const& physicalDevice, VkSurfaceCapabilitiesKHR& capabilities) const;
			bool getFormats(PhysicalDevice const& physicalDevice, std::vector<VkSurfaceFormatKHR>& formats) const;
			bool getPresentModes(PhysicalDevice const& physicalDevice, std::vector<VkPresentModeKHR>& presentModes) const;

			bool isValid() const;

			VkSurfaceKHR const& operator()() const;

			Surface& operator=(Surface const&) = delete;

		private:
			Instance const& m_instance;
			VkSurfaceKHR m_surface;
		};
	}
}

#endif
