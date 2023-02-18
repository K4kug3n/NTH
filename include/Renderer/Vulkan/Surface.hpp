#ifndef NTH_RENDERER_VK_SURFACE_HPP
#define NTH_RENDERER_VK_SURFACE_HPP

#include <Renderer/Vulkan/Instance.hpp>

#include <vulkan/vulkan.h>

namespace Nth {
	
	struct WindowHandle;

	namespace Vk {
		class PhysicalDevice;
	
		class Surface {
		public:
			Surface() = delete;
			Surface(const Instance& instance);
			Surface(Surface&&) = delete;
			Surface(const Surface&) = delete;
			~Surface();

			void create(const WindowHandle& infos);

			bool get_physical_device_surface_support(const PhysicalDevice& device, uint32_t queue_family_index) const;
			VkSurfaceCapabilitiesKHR get_capabilities(const PhysicalDevice& physical_device) const;
			std::vector<VkSurfaceFormatKHR> get_formats(const PhysicalDevice& physical_device) const;
			std::vector<VkPresentModeKHR> get_present_modes(const PhysicalDevice& physical_device) const;

			bool is_valid() const;

			VkSurfaceKHR operator()() const;

			Surface& operator=(Surface&&) = delete;
			Surface& operator=(const Surface&) = delete;

		private:
			const Instance& m_instance;
			VkSurfaceKHR m_surface;
		};
	}
}

#endif
