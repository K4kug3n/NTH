#ifndef NTH_RENDERER_VULKANINSTANCE_HPP
#define NTH_RENDERER_VULKANINSTANCE_HPP

#include <Renderer/Vulkan/Instance.hpp>
#include <Renderer/Vulkan/Queue.hpp>

#include <Renderer/VulkanDevice.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Nth {
	namespace Vk {
		class Surface;
	}

	// TODO: Clean this imple
	class VulkanInstance {
	public:
		VulkanInstance();
		VulkanInstance(VulkanInstance const&) = delete;
		VulkanInstance(VulkanInstance&&) = delete;
		~VulkanInstance();

		bool createDevice(Vk::Surface& surface);

		Vk::Instance& getHandle();
		Vk::Instance const& getHandle() const;
		VulkanDevice& getDevice();
		VulkanDevice const& getDevice() const;

		VulkanInstance& operator=(VulkanInstance const&) = delete;
		VulkanInstance& operator=(VulkanInstance&&) = delete;
	private:
		bool checkPhysicalDeviceProperties(Vk::PhysicalDevice& physicalDevice, Vk::Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);

		Vk::Instance m_instance;
		VulkanDevice m_device;
	};
}

#endif