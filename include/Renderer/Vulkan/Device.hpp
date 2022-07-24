#ifndef NTH_RENDERER_VK_DEVICE_HPP
#define NTH_RENDERER_VK_DEVICE_HPP

#include "Renderer/Vulkan/Instance.hpp"

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
			Device(Instance const& instance);
			Device(Device const&) = delete;
			Device(Device&&) = delete;
			~Device();

			bool create(PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex);

			bool isValid() const;
			bool isLoadedExtension(const std::string_view name) const;
			bool isLoadedLayer(const std::string_view  name) const;

			PhysicalDevice const& getPhysicalDevice() const;
			VmaAllocator getAllocator() const;
			uint32_t getPresentQueueFamilyIndex() const;
			uint32_t getGraphicQueueFamilyIndex() const;

			void waitIdle() const;

			#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) PFN_##fun fun;
			#include "Renderer/Vulkan/DeviceFunctions.inl"

			VkDevice const& operator()() const;

			Device& operator=(Device const&) = delete;
			Device& operator=(Device&&) = delete;

		private:
			PFN_vkVoidFunction loadDeviceFunction(const char* name);

			VkDevice m_device;
			std::unique_ptr<PhysicalDevice> m_physicalDevice;
			uint32_t m_presentQueueFamilyIndex;
			uint32_t m_graphicQueueFamilyIndex;
			Instance const& m_instance;
			VmaAllocator m_allocator;

			std::unordered_set<std::string> m_extensions;
			std::unordered_set<std::string> m_layers;
		};
	}
}

#endif
