#pragma once

#ifndef NTH_PHYSICALDEVICE_HPP
#define NTH_PHYSICALDEVICE_HPP

#include "Renderer/Instance.hpp"

#include <vulkan/vulkan.h>

#include <unordered_set>
#include <string>

namespace Nth {

	class PhysicalDevice {
	public:
		PhysicalDevice(Instance const& instance, VkPhysicalDevice const& physicalDevice);
		PhysicalDevice() = delete;
		PhysicalDevice(PhysicalDevice const&) = delete;
		PhysicalDevice(PhysicalDevice&&) noexcept = default;
		~PhysicalDevice() = default;

		VkPhysicalDeviceProperties getProperties() const;
		VkPhysicalDeviceFeatures getFeatures() const;
		VkPhysicalDeviceMemoryProperties getMemoryProperties() const;
		std::vector<VkQueueFamilyProperties> getQueueFamilyProperties() const;
		std::vector<VkExtensionProperties> getExtensionsProperties(const char* layerName = nullptr) const;

		bool isSupportedExtension(std::string const& name) const;

		VkPhysicalDevice const& operator()() const;

		PhysicalDevice& operator=(PhysicalDevice const&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = delete;

	private:
		std::unordered_set<std::string> enumerateExtensionsPropertiesNames(const char* layerName = nullptr) const;

		Instance const& m_instance;
		VkPhysicalDevice m_physicalDevice;

		std::unordered_set<std::string> m_extensionsNames;
	};
}

#endif