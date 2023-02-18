#include <Renderer/Vulkan/Device.hpp>

#include <Renderer/Vulkan/VkUtils.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/VulkanLoader.hpp>

#include <iostream>

#include <cassert>

namespace Nth {
	namespace Vk {
		Device::Device(const Instance& instance) :
			m_device{ VK_NULL_HANDLE },
			m_physical_device{ nullptr },
			m_allocator{ VK_NULL_HANDLE },
			m_instance{ instance } {

			#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) fun = nullptr;
			#include <Renderer/Vulkan/DeviceFunctions.inl>
		}

		Device::~Device() {
			destroy();
		}

		void Device::create(PhysicalDevice physicalDevice, const VkDeviceCreateInfo& infos) {
			assert(m_instance.is_valid());
			
			// TODO: Review
			m_physical_device = std::make_unique<PhysicalDevice>(std::move(physicalDevice));

			VkResult result{ m_instance.vkCreateDevice((*m_physical_device)(), &infos, nullptr, &m_device) };
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Can't create device, " + to_string(result));
			}

			for (uint32_t i{ 0 }; i < infos.enabledExtensionCount; ++i) {
				m_extensions.emplace(infos.ppEnabledExtensionNames[i]);
			}

			for (uint32_t i{ 0 }; i < infos.enabledLayerCount; ++i) {
				m_layers.emplace(infos.ppEnabledLayerNames[i]);
			}

			#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN(ext) if(is_loaded_extension(#ext)) {
			#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END() }
			#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) fun = reinterpret_cast<PFN_##fun>(load_device_function(#fun));
			
			#include <Renderer/Vulkan/DeviceFunctions.inl>

			VmaVulkanFunctions vulkanFunctions{};
			vulkanFunctions.vkGetPhysicalDeviceProperties = m_instance.vkGetPhysicalDeviceProperties;
			vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = m_instance.vkGetPhysicalDeviceMemoryProperties;
			vulkanFunctions.vkGetInstanceProcAddr = VulkanLoader::vkGetInstanceProcAddr;
			vulkanFunctions.vkGetDeviceProcAddr = m_instance.vkGetDeviceProcAddr;
			vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
			vulkanFunctions.vkFreeMemory = vkFreeMemory;
			vulkanFunctions.vkMapMemory = vkMapMemory;
			vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
			vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
			vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
			vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
			vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
			vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
			vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
			vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
			vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
			vulkanFunctions.vkCreateImage = vkCreateImage;
			vulkanFunctions.vkDestroyImage = vkDestroyImage;
			vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;

			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
			allocatorInfo.physicalDevice = (*m_physical_device)();
			allocatorInfo.device = m_device;
			allocatorInfo.instance = m_instance();
			allocatorInfo.pVulkanFunctions = &vulkanFunctions;

			result = vmaCreateAllocator(&allocatorInfo, &m_allocator);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create allocator, " + to_string(result));
			}
		}

		void Device::destroy() {
			if (is_valid()) {
				vmaDestroyAllocator(m_allocator);
				vkDestroyDevice(m_device, nullptr);

				m_device = VK_NULL_HANDLE;
			}
		}

		bool Device::is_valid() const {
			return m_device != VK_NULL_HANDLE;
		}

		bool Device::is_loaded_extension(std::string_view name) const {
			return m_extensions.count(name.data()) > 0;
		}

		bool Device::is_loaded_layer(std::string_view name) const {
			return m_layers.count(name.data()) > 0;
		}

		const PhysicalDevice& Device::get_physical_device() const {
			return *m_physical_device;
		}

		VmaAllocator Device::get_allocator() const {
			return m_allocator;
		}

		void Device::wait_idle() const {
			assert(is_valid());
			vkDeviceWaitIdle(m_device);
		}

		VkDevice Device::operator()() const {
			return m_device;
		}

		PFN_vkVoidFunction Device::load_device_function(const char* name) {
			PFN_vkVoidFunction fun{ m_instance.vkGetDeviceProcAddr(m_device, name) };
			if (!fun) {
				throw std::runtime_error(name);
			}

			return fun;
		}
	}
}
