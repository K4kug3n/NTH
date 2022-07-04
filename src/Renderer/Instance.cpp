#include "Renderer/Instance.hpp"

#include "Renderer/VulkanLoader.hpp"
#include "Renderer/PhysicalDevice.hpp"
#include "Renderer/VkUtil.hpp"

#include <iostream>

namespace Nth {
	Instance::Instance() :
		m_instance{ nullptr } {
		#define NTH_INSTANCE_FUNCTION(fun) fun = nullptr;
		#include "Renderer/InstanceFunctions.inl"
	}

	Instance::~Instance(){
		if (m_instance) {
			vkDestroyInstance(m_instance, nullptr);
		}
	}

	bool Instance::create(std::string const& appName, uint32_t appVersion, std::string const& engineName, uint32_t engineVersion,
		uint32_t apiVersion, std::vector<const char*> const& layers, std::vector<const char*> const& extensions){

		VkApplicationInfo appInfos{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,  // VkStructureType     sType
			nullptr,                             // const void         *pNext
			appName.c_str(),                     // const char         *pApplicationName
			appVersion,                          // uint32_t            applicationVersion
			engineName.c_str(),                  // const char         *pEngineName
			engineVersion,                       // uint32_t            engineVersion
			apiVersion                           // uint32_t            apiVersion
		};

		VkInstanceCreateInfo instanceInfos{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,           // VkStructureType            sType
			nullptr,                                          // const void                *pNext
			0,                                                // VkInstanceCreateFlags      flags
			&appInfos,                                        // const VkApplicationInfo   *pApplicationInfo
			static_cast<uint32_t>(layers.size()),             // uint32_t                   enabledLayerCount
			layers.empty() ? nullptr : layers.data(),         // const char * const        *ppEnabledLayerNames
			static_cast<uint32_t>(extensions.size()),         // uint32_t                   enabledExtensionCount
			extensions.empty() ? nullptr : extensions.data()  // const char * const        *ppEnabledExtensionNames
		};

		return create(instanceInfos);
	}

	bool Instance::isValid() const {
		return m_instance != nullptr;
	}

	bool Instance::isLoadedExtension(std::string const& name) const {
		return m_extensions.count(name) > 0;
	}

	bool Instance::isLoadedLayer(std::string const& name) const {
		return m_layers.count(name) > 0;
	}

	VkInstance const& Instance::operator()() const {
		return m_instance;
	}

	std::vector<PhysicalDevice> Instance::enumeratePhysicalDevices(){
		uint32_t count{ 0 };
		VkResult result{ vkEnumeratePhysicalDevices(m_instance, &count, nullptr) };
		if (result != VkResult::VK_SUCCESS || count == 0) {
			std::cerr << "Error: Could not get number of physical device : " << toString(result) << std::endl;
			return std::vector<PhysicalDevice>();
		}

		std::vector<VkPhysicalDevice> vkPhysicalDevices(count);
		result = vkEnumeratePhysicalDevices(m_instance, &count, vkPhysicalDevices.data());
		if (result != VkResult::VK_SUCCESS) {
			std::cerr << "Error: Could not enumerate physical devices : " << toString(result) << std::endl;
			return std::vector<PhysicalDevice>();
		}

		std::vector<PhysicalDevice> physicalDevices;
		physicalDevices.reserve(vkPhysicalDevices.size());
		for (auto const& device : vkPhysicalDevices) {
			physicalDevices.push_back(PhysicalDevice( *this, device ));
		}

		return physicalDevices;
	}

	bool Instance::create(VkInstanceCreateInfo const& infos){
		VkResult result{ VulkanLoader::vkCreateInstance(&infos, nullptr, &m_instance) };
		if (result != VkResult::VK_SUCCESS) {
			std::cerr << "Error: Could not create Vulkan instance : " << toString(result) << std::endl;
			return false;
		}

		for (uint32_t i{ 0 }; i < infos.enabledExtensionCount; ++i) {
			m_extensions.insert(infos.ppEnabledExtensionNames[i]);
		}
		for (uint32_t i{ 0 }; i < infos.enabledLayerCount; ++i) {
			m_layers.insert(infos.ppEnabledLayerNames[i]);
		}

		try {
			#define NTH_INSTANCE_EXT_FUNCTION_BEGIN(ext) if(isLoadedExtension(#ext)) {
			#define NTH_INSTANCE_EXT_FUNCTION_END() }
			#define NTH_INSTANCE_FUNCTION(fun) fun = reinterpret_cast<PFN_##fun>(loadInstanceFunction(#fun));

			#include "Renderer/InstanceFunctions.inl"
		} 
		catch (std::exception& e) {
			std::cerr << "Error: Can't load " << e.what() << " function" << std::endl;
			return false;
		}

		return true;
	}

	PFN_vkVoidFunction Instance::loadInstanceFunction(const char* name){
		PFN_vkVoidFunction fun{ VulkanLoader::getInstanceProcAddr(m_instance, name) };
		if (!fun) {
			throw std::runtime_error(name);
		}

		return fun;
	}
}
