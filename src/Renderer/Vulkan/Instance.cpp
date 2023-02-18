#include <Renderer/Vulkan/Instance.hpp>

#include <Renderer/Vulkan/VulkanLoader.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		Instance::Instance() :
			m_instance{ nullptr } {
			#define NTH_RENDERER_VK_INSTANCE_FUNCTION(fun) fun = nullptr;
			#include <Renderer/Vulkan/InstanceFunctions.inl>
		}

		Instance::~Instance() {
			if (m_instance) {
				vkDestroyInstance(m_instance, nullptr);
			}
		}

		void Instance::create(std::string_view app_name, uint32_t app_version, std::string_view engine_name, uint32_t engine_version,
			uint32_t api_version, const std::vector<const char*>& layers, const std::vector<const char*>& extensions) {

			VkApplicationInfo app_infos{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,   // VkStructureType     sType
				nullptr,                              // const void         *pNext
				app_name.data(),                      // const char         *pApplicationName
				app_version,                          // uint32_t            applicationVersion
				engine_name.data(),                   // const char         *pEngineName
				engine_version,                       // uint32_t            engineVersion
				api_version                           // uint32_t            apiVersion
			};

			VkInstanceCreateInfo instance_infos{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,           // VkStructureType            sType
				nullptr,                                          // const void                *pNext
				0,                                                // VkInstanceCreateFlags      flags
				&app_infos,                                        // const VkApplicationInfo   *pApplicationInfo
				static_cast<uint32_t>(layers.size()),             // uint32_t                   enabledLayerCount
				layers.empty() ? nullptr : layers.data(),         // const char * const        *ppEnabledLayerNames
				static_cast<uint32_t>(extensions.size()),         // uint32_t                   enabledExtensionCount
				extensions.empty() ? nullptr : extensions.data()  // const char * const        *ppEnabledExtensionNames
			};

			create(instance_infos);
		}

		bool Instance::is_valid() const {
			return m_instance != nullptr;
		}

		bool Instance::is_loaded_extension(std::string_view name) const {
			return m_extensions.count(name.data()) > 0;
		}

		bool Instance::is_loaded_layer(std::string_view name) const {
			return m_layers.count(name.data()) > 0;
		}

		VkInstance Instance::operator()() const {
			return m_instance;
		}

		std::vector<PhysicalDevice> Instance::enumerate_physical_devices() {
			uint32_t count{ 0 };
			VkResult result{ vkEnumeratePhysicalDevices(m_instance, &count, nullptr) };
			if (result != VkResult::VK_SUCCESS || count == 0) {
				throw std::runtime_error("Could not get number of physical device, " + to_string(result));
			}

			std::vector<VkPhysicalDevice> vk_physical_devices(count);
			result = vkEnumeratePhysicalDevices(m_instance, &count, vk_physical_devices.data());
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Could not enumerate physical devices, " + to_string(result));
			}

			std::vector<PhysicalDevice> physical_devices;
			physical_devices.reserve(vk_physical_devices.size());
			for (const auto& device : vk_physical_devices) {
				physical_devices.push_back(PhysicalDevice(*this, device));
			}

			return physical_devices;
		}

		void Instance::create(const VkInstanceCreateInfo& infos) {
			VkResult result{ VulkanLoader::vkCreateInstance(&infos, nullptr, &m_instance) };
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Could not create Vulkan instance, " + to_string(result));
			}

			for (uint32_t i{ 0 }; i < infos.enabledExtensionCount; ++i) {
				m_extensions.insert(infos.ppEnabledExtensionNames[i]);
			}
			for (uint32_t i{ 0 }; i < infos.enabledLayerCount; ++i) {
				m_layers.insert(infos.ppEnabledLayerNames[i]);
			}

			#define NTH_RENDERER_VK_INSTANCE_EXT_FUNCTION_BEGIN(ext) if(is_loaded_extension(#ext)) {
			#define NTH_RENDERER_VK_INSTANCE_EXT_FUNCTION_END() }
			#define NTH_RENDERER_VK_INSTANCE_FUNCTION(fun) fun = reinterpret_cast<PFN_##fun>(load_instance_function(#fun));
			
			#include <Renderer/Vulkan/InstanceFunctions.inl>
		}

		PFN_vkVoidFunction Instance::load_instance_function(const char* name) {
			PFN_vkVoidFunction fun{ VulkanLoader::get_instance_proc_addr(m_instance, name) };
			if (!fun) {
				throw std::runtime_error(name);
			}

			return fun;
		}
	}
}
