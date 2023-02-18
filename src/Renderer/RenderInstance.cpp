#include <Renderer/RenderInstance.hpp>

#include <Renderer/Vulkan/VulkanLoader.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>

#include <vulkan/vulkan.h>

#include <iostream>
#include <unordered_set>
#include <string>
#include <array>
#include <stdexcept>

namespace Nth {
	RenderInstance::RenderInstance() :
		m_device(*this) {

		if (!Vk::VulkanLoader::initialize()) {
			throw std::runtime_error("Can't load Vulkan");
		}

		uint32_t supportedApi{ VK_MAKE_VERSION(1, 0, 0) };
		if (Vk::VulkanLoader::vkEnumerateInstanceVersion) {
			Vk::VulkanLoader::vkEnumerateInstanceVersion(&supportedApi);
		}

		std::unordered_set<std::string> available_layer{};
		for (const VkLayerProperties& layer_prop : Vk::VulkanLoader::enumerate_layer_properties()) {
			available_layer.insert(layer_prop.layerName);
		}

		std::vector<const char*> enabled_layer;
		#if !defined(NDEBUG)
		if (available_layer.count("VK_LAYER_KHRONOS_validation")) {
			enabled_layer.push_back("VK_LAYER_KHRONOS_validation");
		}
		#endif

		std::vector<const char*> enabled_extensions;
		#ifdef VK_USE_PLATFORM_XCB_KHR
		enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#endif
		#ifdef VK_USE_PLATFORM_XLIB_KHR
		enabledExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		#endif
		#ifdef VK_USE_PLATFORM_WIN32_KHR
		enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#endif

		enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		m_instance.create("NTH_RENDERER", VK_MAKE_VERSION(0, 0, 1), "NTH", VK_MAKE_VERSION(0, 0, 1), supportedApi, enabled_layer, enabled_extensions);

		for (const auto& device : m_instance.enumerate_physical_devices()) {
			std::cout << "Device: " << device.get_properties().deviceName << std::endl;
		}
	}

	RenderInstance::~RenderInstance() {
	}

	void RenderInstance::create_device(Vk::Surface& surface) {
		uint32_t present_queue_family_index = UINT32_MAX;
		uint32_t graphics_queue_family_index = UINT32_MAX;

		std::vector<Vk::PhysicalDevice> physical_devices = m_instance.enumerate_physical_devices();
		if (physical_devices.empty()) {
			throw std::runtime_error("No physical device detected");
		}

		size_t selectedIndex{ physical_devices.size() };
		for (size_t i{ 0 }; i < physical_devices.size(); ++i) {
			if (check_physical_device_properties(physical_devices[i], surface, present_queue_family_index, graphics_queue_family_index)) {
				selectedIndex = i;
			};
		}

		if (selectedIndex == physical_devices.size()) {
			throw std::runtime_error("Can't find physical with needed properties");
		}

		Vk::PhysicalDevice& physical_device{ physical_devices[selectedIndex] };

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::vector<float> queue_priorities = { 1.0f };

		queue_create_infos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			0,                                           // VkDeviceQueueCreateFlags     flags
			graphics_queue_family_index,                    // uint32_t                     queueFamilyIndex
			1,                                           // uint32_t                     queueCount
			queue_priorities.data()                       // const float                 *pQueuePriorities
		});

		if (graphics_queue_family_index != present_queue_family_index) {
			queue_create_infos.push_back({
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType              sType
				nullptr,                                     // const void                  *pNext
				0,                                           // VkDeviceQueueCreateFlags     flags
				present_queue_family_index,                     // uint32_t                     queueFamilyIndex
				1,                                           // uint32_t                     queueCount
				queue_priorities.data()                       // const float                 *pQueuePriorities
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
			static_cast<uint32_t>(queue_create_infos.size()),   // uint32_t                           queueCreateInfoCount
			queue_create_infos.data(),                          // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
			0,                                                // uint32_t                           enabledLayerCount
			nullptr,                                          // const char * const                *ppEnabledLayerNames
			static_cast<uint32_t>(extensions.size()),         // uint32_t                           enabledExtensionCount
			extensions.data(),                                // const char * const                *ppEnabledExtensionNames
			nullptr                                           // const VkPhysicalDeviceFeatures    *pEnabledFeatures
		};

		m_device.create(std::move(physical_device), deviceCreateInfo, present_queue_family_index, graphics_queue_family_index);
	}

	Vk::Instance& RenderInstance::get_handle() {
		return m_instance;
	}

	const Vk::Instance& RenderInstance::get_handle() const {
		return m_instance;
	}

	RenderDevice& RenderInstance::get_device() {
		return m_device;
	}

	const RenderDevice& RenderInstance::get_device() const {
		return m_device;
	}

	bool RenderInstance::check_physical_device_properties(Vk::PhysicalDevice& physical_device, Vk::Surface& surface, uint32_t& graphics_queue_family_index, uint32_t& present_queue_family_index) {
		std::vector<char const*> devices_extensions_names = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (const auto& name : devices_extensions_names) {
			if (!physical_device.is_supported_extension(name)) {
				std::cerr << "Warning: physical device " << physical_device() << " don't support " << name << std::endl;
				return false;
			}
		}

		VkPhysicalDeviceProperties properties{ physical_device.get_properties() };
		VkPhysicalDeviceFeatures features{ physical_device.get_features() };

		uint32_t major_version = VK_VERSION_MAJOR(properties.apiVersion);
		if ((major_version < 1) && (properties.limits.maxImageDimension2D < 4096)) {
			std::cerr << "Warning: physical device " << physical_device() << "don't support require parameters" << std::endl;
			return false;
		}

		std::vector<VkQueueFamilyProperties> queueFamiliesProperities{ physical_device.get_queue_family_properties() };

		for (size_t i{ 0 }; i < queueFamiliesProperities.size(); ++i) {
			bool supported = surface.get_physical_device_surface_support(physical_device, static_cast<uint32_t>(i));
			if (!supported) {
				std::cerr << "Warning: Can't get support of queue " << i << std::endl;
			}

			if (queueFamiliesProperities[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { // Support graphics operations
				if (supported) { // Prefer a queue that do everythings
					graphics_queue_family_index = static_cast<uint32_t>(i);
					present_queue_family_index = static_cast<uint32_t>(i);
					return true; // We have all we want
				}
				else if (graphics_queue_family_index == UINT32_MAX) {
					graphics_queue_family_index = static_cast<uint32_t>(i);
				}
			}
			else if (supported) {
				present_queue_family_index = static_cast<uint32_t>(i);
			}
		}

		if (graphics_queue_family_index == UINT32_MAX) {
			std::cerr << "Warning: Physical device " << physical_device() << " haven't graphics queue" << std::endl;
			return false;
		}

		if (present_queue_family_index == UINT32_MAX) {
			std::cerr << "Warning: Physical device " << physical_device() << " doesn't support this surface" << std::endl;
			return false;
		}

		return true;
	}
}
