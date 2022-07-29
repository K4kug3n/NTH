#ifndef NTH_RENDERER_VK_VULKANINSTANCE_HPP
#define NTH_RENDERER_VK_VULKANINSTANCE_HPP

#include "Renderer/Vulkan/Instance.hpp"
#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/Queue.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Nth {
	namespace Vk {
		class Surface;

		class VulkanInstance {
		public:
			VulkanInstance();
			VulkanInstance(VulkanInstance const&) = delete;
			VulkanInstance(VulkanInstance&&) = delete;
			~VulkanInstance();

			bool createDevice(Surface& surface);

			Instance& getInstance();
			Device& getDevice();
			Device const& getDevice() const;

			VulkanInstance& operator=(VulkanInstance const&) = delete;
			VulkanInstance& operator=(VulkanInstance&&) = delete;

			static VulkanInstance* instance();

		private:
			bool checkPhysicalDeviceProperties(PhysicalDevice& physicalDevice, Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);

			Instance m_instance;
			Device m_device;

			static VulkanInstance* m_classInstance;
		};
	}
}

#endif