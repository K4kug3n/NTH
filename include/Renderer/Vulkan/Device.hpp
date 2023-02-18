#ifndef NTH_RENDERER_VK_DEVICE_HPP
#define NTH_RENDERER_VK_DEVICE_HPP

#include <Renderer/Vulkan/Instance.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <unordered_set>
#include <memory>
#include <string_view>

namespace Nth {
	namespace Vk {
		class PhysicalDevice;

		class Device {
		public:
			Device(const Instance& instance);
			Device(const Device&) = delete;
			Device(Device&&) = delete;
			~Device();

			void create(PhysicalDevice physicalDevice, const VkDeviceCreateInfo& infos);
			void destroy();

			bool is_valid() const;
			bool is_loaded_extension(std::string_view name) const;
			bool is_loaded_layer(std::string_view  name) const;

			const PhysicalDevice& get_physical_device() const;
			VmaAllocator get_allocator() const;

			void wait_idle() const;

			#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) PFN_##fun fun;
			#include <Renderer/Vulkan/DeviceFunctions.inl>

			VkDevice operator()() const;

			Device& operator=(const Device&) = delete;
			Device& operator=(Device&&) = delete;

		private:
			PFN_vkVoidFunction load_device_function(const char* name);

			VkDevice m_device;
			std::unique_ptr<PhysicalDevice> m_physical_device;
			const Instance& m_instance;
			VmaAllocator m_allocator;

			std::unordered_set<std::string> m_extensions;
			std::unordered_set<std::string> m_layers;
		};
	}
}

#endif
