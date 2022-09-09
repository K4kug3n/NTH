#include <Renderer/Vulkan/Device.hpp>

#include <Renderer/Vulkan/VkUtil.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/VulkanLoader.hpp>

#include <iostream>

#include <cassert>

namespace Nth {
	namespace Vk {
		Device::Device(Instance const& instance) :
			m_device{ VK_NULL_HANDLE },
			m_physicalDevice{ nullptr },
			m_allocator{ VK_NULL_HANDLE },
			m_instance{ instance } {

			#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) fun = nullptr;
			#include <Renderer/Vulkan/DeviceFunctions.inl>
		}

		Device::~Device() {
			destroy();
		}

		bool Device::create(PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos) {
			assert(m_instance.isValid());
			
			// TODO: Review
			m_physicalDevice = std::make_unique<PhysicalDevice>(std::move(physicalDevice));

			VkResult result{ m_instance.vkCreateDevice((*m_physicalDevice)(), &infos, nullptr, &m_device) };
			if (result != VkResult::VK_SUCCESS) {
				std::cerr << "Error: Can't create device, " + toString(result) << std::endl;
				return false;
			}

			for (uint32_t i{ 0 }; i < infos.enabledExtensionCount; ++i) {
				m_extensions.emplace(infos.ppEnabledExtensionNames[i]);
			}

			for (uint32_t i{ 0 }; i < infos.enabledLayerCount; ++i) {
				m_layers.emplace(infos.ppEnabledLayerNames[i]);
			}

			try {
				#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN(ext) if(isLoadedExtension(#ext)) {
				#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END() }
				#define NTH_RENDERER_VK_DEVICE_FUNCTION(fun) fun = reinterpret_cast<PFN_##fun>(loadDeviceFunction(#fun));
				
				#include <Renderer/Vulkan/DeviceFunctions.inl>
			}
			catch (std::exception& e) {
				std::cerr << "Error: Can't load " << e.what() << " function" << std::endl;
				return false;
			}

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
			allocatorInfo.physicalDevice = (*m_physicalDevice)();
			allocatorInfo.device = m_device;
			allocatorInfo.instance = m_instance();
			allocatorInfo.pVulkanFunctions = &vulkanFunctions;

			result = vmaCreateAllocator(&allocatorInfo, &m_allocator);
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create allocator, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		void Device::destroy() {
			if (isValid()) {
				vmaDestroyAllocator(m_allocator);
				vkDestroyDevice(m_device, nullptr);

				m_device = VK_NULL_HANDLE;
			}
		}

		bool Device::isValid() const {
			return m_device != VK_NULL_HANDLE;
		}

		bool Device::isLoadedExtension(const std::string_view name) const {
			return m_extensions.count(name.data()) > 0;
		}

		bool Device::isLoadedLayer(const std::string_view name) const {
			return m_layers.count(name.data()) > 0;
		}

		PhysicalDevice const& Device::getPhysicalDevice() const {
			return *m_physicalDevice;
		}

		VmaAllocator Device::getAllocator() const {
			return m_allocator;
		}

		void Device::waitIdle() const {
			assert(isValid());
			vkDeviceWaitIdle(m_device);
		}

		VkDevice const& Device::operator()() const {
			return m_device;
		}

		PFN_vkVoidFunction Device::loadDeviceFunction(const char* name) {
			PFN_vkVoidFunction fun{ m_instance.vkGetDeviceProcAddr(m_device, name) };
			if (!fun) {
				throw std::runtime_error(name);
			}

			return fun;
		}
	}
}
