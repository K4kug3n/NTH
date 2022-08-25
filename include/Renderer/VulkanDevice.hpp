#ifndef NTH_RENDERER_VULKANDEVICE_HPP
#define NTH_RENDERER_VULKANDEVICE_HPP

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/Queue.hpp>

namespace Nth {
	namespace Vk {
		class Surface;
		class PhysicalDevice;
	}

	class VulkanInstance;

	class VulkanDevice {
	public:
		VulkanDevice(VulkanInstance const& instance);
		VulkanDevice() = delete;
		VulkanDevice(VulkanDevice const&) = delete;
		VulkanDevice(VulkanDevice&&) = delete;
		~VulkanDevice() = default;

		//TODO: Review this
		void create(Vk::PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex);

		Vk::Queue& presentQueue();
		Vk::Queue const& presentQueue() const;
		Vk::Queue& graphicsQueue();
		Vk::Queue const& graphicsQueue() const;
		Vk::Device& getHandle();
		Vk::Device const& getHandle() const;

		VulkanDevice& operator=(VulkanDevice const&) = delete;
		VulkanDevice& operator=(VulkanDevice&&) = delete;
	private:
		VulkanInstance const& m_instance;
		Vk::Queue m_presentQueue;
		Vk::Queue m_graphicsQueue;

		Vk::Device m_device;
	};
}

#endif