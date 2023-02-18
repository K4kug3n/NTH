#include <Renderer/Vulkan/PhysicalDevice.hpp>

#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		PhysicalDevice::PhysicalDevice(const Instance& instance, const VkPhysicalDevice& physicalDevice) :
			m_physical_device{ physicalDevice },
			m_instance{ instance },
			m_extensions_names{ enumerate_extensions_properties_names() }{ }

		VkPhysicalDeviceProperties PhysicalDevice::get_properties() const {
			VkPhysicalDeviceProperties properties;
			m_instance.vkGetPhysicalDeviceProperties(m_physical_device, &properties);

			return properties;
		}

		VkPhysicalDeviceFeatures PhysicalDevice::get_features() const {
			VkPhysicalDeviceFeatures features;
			m_instance.vkGetPhysicalDeviceFeatures(m_physical_device, &features);

			return features;
		}

		VkPhysicalDeviceMemoryProperties PhysicalDevice::get_memory_properties() const {
			VkPhysicalDeviceMemoryProperties memory_propeties;
			m_instance.vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memory_propeties);

			return memory_propeties;
		}

		VkFormatProperties PhysicalDevice::get_format_properties(VkFormat format) const {
			VkFormatProperties props;

			m_instance.vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

			return props;
		}

		std::vector<VkQueueFamilyProperties> PhysicalDevice::get_queue_family_properties() const {
			uint32_t count{ 0 };
			m_instance.vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, nullptr);
			if (count == 0) {
				throw std::runtime_error("Can't get number of queue family propeties");
			}

			std::vector<VkQueueFamilyProperties> queue_family_properties(count);
			m_instance.vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, queue_family_properties.data());

			return queue_family_properties;
		}

		std::vector<VkExtensionProperties> PhysicalDevice::get_extensions_properties(const char* layer_name) const {
			uint32_t count{ 0 };
			VkResult result{ m_instance.vkEnumerateDeviceExtensionProperties(m_physical_device, layer_name, &count, nullptr) };
			if (result != VkResult::VK_SUCCESS || count == 0) {
				throw std::runtime_error("Can't get number of extensions propeties : " + to_string(result));
			}

			std::vector<VkExtensionProperties> extensionsProperties(count);
			result = m_instance.vkEnumerateDeviceExtensionProperties(m_physical_device, layer_name, &count, extensionsProperties.data());
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Can't enumerate extensions propeties : " + to_string(result));
			}

			return extensionsProperties;
		}

		bool PhysicalDevice::is_supported_extension(std::string_view name) const {
			return m_extensions_names.count(name.data()) > 0;
		}

		std::unordered_set<std::string> PhysicalDevice::enumerate_extensions_properties_names(const char* layerName) const {
			std::unordered_set<std::string> extensions_names;

			for (const auto& ext : get_extensions_properties(layerName)) {
				extensions_names.insert(ext.extensionName);
			}

			return extensions_names;
		}

		VkPhysicalDevice PhysicalDevice::operator()() const {
			return m_physical_device;
		}
	}
}
