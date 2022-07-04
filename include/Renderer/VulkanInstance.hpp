#pragma once

#ifndef NTH_VULKANINSTANCE_HPP
#define NTH_VULKANINSTANCE_HPP

#include "Renderer/Instance.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/Queue.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Nth {

	class Surface;
	
	class VulkanInstance {
	public:
		VulkanInstance();
		VulkanInstance(VulkanInstance const&) = delete;
		VulkanInstance(VulkanInstance&&) = delete;
		~VulkanInstance();

		static bool isInitialized();

		std::shared_ptr<Device> createDevice(Surface& surface);

		Instance& getInstance();
		std::shared_ptr<Device>& getDevice();
		
		VulkanInstance& operator=(VulkanInstance const&) = delete;
		VulkanInstance& operator=(VulkanInstance&&) = delete;

		static VulkanInstance* instance();

	private:
		bool checkPhysicalDeviceProperties(PhysicalDevice& physicalDevice, Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);

		Instance m_instance;
		std::shared_ptr<Device> m_device;

		static VulkanInstance* m_classInstance;
	};
}

#endif