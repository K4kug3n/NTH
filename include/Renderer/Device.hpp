#pragma once

#ifndef NTH_DEVICE_HPP
#define NTH_DEVICE_HPP

#include "Renderer/Instance.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <unordered_set>
#include <string>

namespace Nth {

	class PhysicalDevice;

	class Device {
	public:
		Device(Instance const& instance);
		Device(Device const&) = delete;
		Device(Device&&) = delete;
		~Device();

		bool create(PhysicalDevice const& physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex);

		bool isValid() const;
		bool isLoadedExtension(std::string const& name) const;
		bool isLoadedLayer(std::string const& name) const;

		PhysicalDevice const& getPhysicalDevice() const;
		VmaAllocator getAllocator() const;
		uint32_t getPresentQueueFamilyIndex() const;
		uint32_t getGraphicQueueFamilyIndex() const;

		void waitIdle() const;

		#define NTH_DEVICE_FUNCTION(fun) PFN_##fun fun;
		#include "Renderer/DeviceFunctions.inl"

		VkDevice const& operator()() const;

		Device& operator=(Device const&) = delete;
		Device& operator=(Device&&) = delete;

	private:
		PFN_vkVoidFunction loadDeviceFunction(const char* name);

		VkDevice m_device;
		PhysicalDevice const* m_physicalDevice;
		uint32_t m_presentQueueFamilyIndex;
		uint32_t m_graphicQueueFamilyIndex;
		Instance const& m_instance;
		VmaAllocator m_allocator;

		std::unordered_set<std::string> m_extensions;
		std::unordered_set<std::string> m_layers;
	};
}

#endif
