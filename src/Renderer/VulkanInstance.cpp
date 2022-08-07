#include "Renderer/VulkanInstance.hpp"

#include "Renderer/Vulkan/VulkanLoader.hpp"
#include "Renderer/Vulkan/Surface.hpp"
#include "Renderer/Vulkan/PhysicalDevice.hpp"

#include <vulkan/vulkan.h>

#include <iostream>
#include <unordered_set>
#include <string>
#include <array>
#include <stdexcept>

namespace Nth {
	VulkanInstance::VulkanInstance() :
		m_device(*this) {

		if (!Vk::VulkanLoader::initialize()) {
			throw std::runtime_error("Can't load Vulkan");
		}

		uint32_t supportedApi{ VK_MAKE_VERSION(1, 0, 0) };
		if (Vk::VulkanLoader::vkEnumerateInstanceVersion) {
			Vk::VulkanLoader::vkEnumerateInstanceVersion(&supportedApi);
		}

		std::unordered_set<std::string> availableLayer{};
		for (VkLayerProperties const& layerProp : Vk::VulkanLoader::enumerateLayerProperties()) {
			availableLayer.insert(layerProp.layerName);
		}

		std::vector<const char*> enabledLayer;
		#if !defined(NDEBUG)
		if (availableLayer.count("VK_LAYER_KHRONOS_validation")) {
			enabledLayer.push_back("VK_LAYER_KHRONOS_validation");
		}
		#endif

		std::vector<const char*> enabledExtensions;
		#ifdef VK_USE_PLATFORM_XCB_KHR
		enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#endif
		#ifdef VK_USE_PLATFORM_XLIB_KHR
		enabledExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		#endif
		#ifdef VK_USE_PLATFORM_WIN32_KHR
		enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#endif

		enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		if (!m_instance.create("NTH_RENDERER_VK_", VK_MAKE_VERSION(0, 0, 1), "NTH", VK_MAKE_VERSION(0, 0, 1), supportedApi, enabledLayer, enabledExtensions)) {
			throw std::runtime_error("Can't create Instance");
		}

		for (auto const& device : m_instance.enumeratePhysicalDevices()) {
			std::cout << "Device: " << device.getProperties().deviceName << std::endl;
		}
	}

	VulkanInstance::~VulkanInstance() {
	}

	bool VulkanInstance::createDevice(Vk::Surface& surface) {
		uint32_t presentQueueFamilyIndex = UINT32_MAX;
		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;

		std::vector<Vk::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
		if (physicalDevices.empty()) {
			std::cerr << "Error: No physical device detected" << std::endl;
			return false;
		}

		size_t selectedIndex{ physicalDevices.size() };
		for (size_t i{ 0 }; i < physicalDevices.size(); ++i) {
			if (checkPhysicalDeviceProperties(physicalDevices[i], surface, presentQueueFamilyIndex, graphicsQueueFamilyIndex)) {
				selectedIndex = i;
			};
		}

		if (selectedIndex == physicalDevices.size()) {
			std::cerr << "Error : Can't find physical with needed properties" << std::endl;
			return false;
		}

		Vk::PhysicalDevice& physicalDevice{ physicalDevices[selectedIndex] };

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::vector<float> queuePriorities = { 1.0f };

		queueCreateInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			0,                                           // VkDeviceQueueCreateFlags     flags
			graphicsQueueFamilyIndex,                    // uint32_t                     queueFamilyIndex
			1,                                           // uint32_t                     queueCount
			queuePriorities.data()                       // const float                 *pQueuePriorities
		});

		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
			queueCreateInfos.push_back({
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType              sType
				nullptr,                                     // const void                  *pNext
				0,                                           // VkDeviceQueueCreateFlags     flags
				presentQueueFamilyIndex,                     // uint32_t                     queueFamilyIndex
				1,                                           // uint32_t                     queueCount
				queuePriorities.data()                       // const float                 *pQueuePriorities
			});
		}

		std::vector<const char*> extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
		};

		VkDeviceCreateInfo deviceCreateInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,             // VkStructureType                    sType
			nullptr,                                          // const void                        *pNext
			0,                                                // VkDeviceCreateFlags                flags
			static_cast<uint32_t>(queueCreateInfos.size()),   // uint32_t                           queueCreateInfoCount
			queueCreateInfos.data(),                          // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
			0,                                                // uint32_t                           enabledLayerCount
			nullptr,                                          // const char * const                *ppEnabledLayerNames
			static_cast<uint32_t>(extensions.size()),         // uint32_t                           enabledExtensionCount
			extensions.data(),                                // const char * const                *ppEnabledExtensionNames
			nullptr                                           // const VkPhysicalDeviceFeatures    *pEnabledFeatures
		};

		m_device.create(std::move(physicalDevice), deviceCreateInfo, presentQueueFamilyIndex, graphicsQueueFamilyIndex);

		return true;
	}

	Vk::Instance& VulkanInstance::getHandle() {
		return m_instance;
	}

	Vk::Instance const& VulkanInstance::getHandle() const {
		return m_instance;
	}

	VulkanDevice& VulkanInstance::getDevice() {
		return m_device;
	}

	VulkanDevice const& VulkanInstance::getDevice() const {
		return m_device;
	}

	bool VulkanInstance::checkPhysicalDeviceProperties(Vk::PhysicalDevice& physicalDevice, Vk::Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex) {
		std::vector<char const*> devicesExtensionsNames = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (auto const& name : devicesExtensionsNames) {
			if (!physicalDevice.isSupportedExtension(name)) {
				std::cerr << "Warning: physical device " << physicalDevice() << " don't support " << name << std::endl;
				return false;
			}
		}

		VkPhysicalDeviceProperties properties{ physicalDevice.getProperties() };
		VkPhysicalDeviceFeatures features{ physicalDevice.getFeatures() };

		uint32_t majorVersion = VK_VERSION_MAJOR(properties.apiVersion);
		if ((majorVersion < 1) && (properties.limits.maxImageDimension2D < 4096)) {
			std::cerr << "Warning: physical device " << physicalDevice() << "don't support require parameters" << std::endl;
			return false;
		}

		std::vector<VkQueueFamilyProperties> queueFamiliesProperities{ physicalDevice.getQueueFamilyProperties() };

		for (size_t i{ 0 }; i < queueFamiliesProperities.size(); ++i) {
			bool supported{ false };
			if (!surface.getPhysicalDeviceSurfaceSupport(physicalDevice, static_cast<uint32_t>(i), supported)) {
				std::cerr << "Warning: Can't get support of queue " << i << std::endl;
			}

			if (queueFamiliesProperities[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { // Support graphics operations
				if (supported) { // Prefer a queue that do everythings
					graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
					presentQueueFamilyIndex = static_cast<uint32_t>(i);
					return true; // We have all we want
				}
				else if (graphicsQueueFamilyIndex == UINT32_MAX) {
					graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
				}
			}
			else if (supported) {
				presentQueueFamilyIndex = static_cast<uint32_t>(i);
			}
		}

		if (graphicsQueueFamilyIndex == UINT32_MAX) {
			std::cerr << "Warning: Physical device " << physicalDevice() << " haven't graphics queue" << std::endl;
			return false;
		}

		if (presentQueueFamilyIndex == UINT32_MAX) {
			std::cerr << "Warning: Physical device " << physicalDevice() << " doesn't support this surface" << std::endl;
			return false;
		}

		return true;
	}
}
