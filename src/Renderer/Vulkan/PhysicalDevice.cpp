#include <Renderer/Vulkan/PhysicalDevice.hpp>

#include <Renderer/Vulkan/VkUtil.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		PhysicalDevice::PhysicalDevice(Instance const& instance, VkPhysicalDevice const& physicalDevice) :
			m_physicalDevice{ physicalDevice },
			m_instance{ instance },
			m_extensionsNames{ enumerateExtensionsPropertiesNames() }{ }

		VkPhysicalDeviceProperties PhysicalDevice::getProperties() const {
			VkPhysicalDeviceProperties properties;
			m_instance.vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

			return properties;
		}

		VkPhysicalDeviceFeatures PhysicalDevice::getFeatures() const {
			VkPhysicalDeviceFeatures features;
			m_instance.vkGetPhysicalDeviceFeatures(m_physicalDevice, &features);

			return features;
		}

		VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperties() const {
			VkPhysicalDeviceMemoryProperties memoryPropeties;
			m_instance.vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryPropeties);

			return memoryPropeties;
		}

		VkFormatProperties PhysicalDevice::getFormatProperties(VkFormat format) const {
			VkFormatProperties props;

			m_instance.vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

			return props;
		}

		std::vector<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilyProperties() const {
			uint32_t count{ 0 };
			m_instance.vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, nullptr);
			if (count == 0) {
				throw std::runtime_error("Error: Can't get number of queue family propeties");
			}

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
			m_instance.vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, queueFamilyProperties.data());

			return queueFamilyProperties;
		}

		std::vector<VkExtensionProperties> PhysicalDevice::getExtensionsProperties(const char* layerName) const {
			uint32_t count{ 0 };
			VkResult result{ m_instance.vkEnumerateDeviceExtensionProperties(m_physicalDevice, layerName, &count, nullptr) };
			if (result != VkResult::VK_SUCCESS || count == 0) {
				throw std::runtime_error("Error: Can't get number of extensions propeties : " + toString(result));
			}

			std::vector<VkExtensionProperties> extensionsProperties(count);
			result = m_instance.vkEnumerateDeviceExtensionProperties(m_physicalDevice, layerName, &count, extensionsProperties.data());
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Error: Can't enumerate extensions propeties : " + toString(result));
			}

			return extensionsProperties;
		}

		bool PhysicalDevice::isSupportedExtension(const std::string_view name) const {
			return m_extensionsNames.count(name.data()) > 0;
		}

		std::unordered_set<std::string> PhysicalDevice::enumerateExtensionsPropertiesNames(const char* layerName) const {
			std::unordered_set<std::string> extensionsNames;

			for (auto const& ext : getExtensionsProperties(layerName)) {
				extensionsNames.insert(ext.extensionName);
			}

			return extensionsNames;
		}

		VkPhysicalDevice const& PhysicalDevice::operator()() const {
			return m_physicalDevice;
		}
	}
}
