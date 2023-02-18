#ifndef NTH_RENDERER_VK_PHYSICALDEVICE_HPP
#define NTH_RENDERER_VK_PHYSICALDEVICE_HPP

#include <Renderer/Vulkan/Instance.hpp>

#include <vulkan/vulkan.h>

#include <unordered_set>
#include <string_view>

namespace Nth {
	namespace Vk {
		class PhysicalDevice {
		public:
			PhysicalDevice(const Instance& instance, const VkPhysicalDevice& physicalDevice);
			PhysicalDevice() = delete;
			PhysicalDevice(const PhysicalDevice&) = delete;
			PhysicalDevice(PhysicalDevice&&) = default;
			~PhysicalDevice() = default;

			VkPhysicalDeviceProperties get_properties() const;
			VkPhysicalDeviceFeatures get_features() const;
			VkPhysicalDeviceMemoryProperties get_memory_properties() const;
			VkFormatProperties get_format_properties(VkFormat format) const;
			std::vector<VkQueueFamilyProperties> get_queue_family_properties() const;
			std::vector<VkExtensionProperties> get_extensions_properties(const char* layer_name = nullptr) const;

			bool is_supported_extension(std::string_view name) const;

			VkPhysicalDevice operator()() const;

			PhysicalDevice& operator=(const PhysicalDevice&) = delete;
			PhysicalDevice& operator=(PhysicalDevice&&) = delete;

		private:
			std::unordered_set<std::string> enumerate_extensions_properties_names(const char* layer_name = nullptr) const;

			const Instance& m_instance;
			VkPhysicalDevice m_physical_device;

			std::unordered_set<std::string> m_extensions_names;
		};
	}
}

#endif