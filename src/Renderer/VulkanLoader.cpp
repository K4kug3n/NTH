#include "Renderer/Loader.hpp"

#include "Util/String.hpp"
#include "Renderer/VkUtil.hpp"

#include <stdexcept>
#include <iostream>

namespace Nth {
	
	Lib VulkanLoader::m_vulkan;
	PFN_vkGetInstanceProcAddr VulkanLoader::vkGetInstanceProcAddr = nullptr;
	#define NTH_GLOBAL_FUNCTION(fun) PFN_##fun Loader::fun = nullptr;
	#define NTH_GLOBAL_FUNCTION_OPTIONAL(fun) NTH_GLOBAL_FUNCTION(fun)
	#include "Renderer/GlobalFunctions.inl"

	bool VulkanLoader::initialize() {
	#if defined(_WIN32)
		m_vulkan.load("vulkan-1.dll");
	#elif defined(__unix__)
		m_vulkan.load("libvulkan.so");
	#else
		#error "OS not supported"
	#endif

		if (!m_vulkan.isValid()) {
			std::cerr << "Error: Can't load Vulkan" << std::endl;
			return false;
		}

		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(m_vulkan.getSymbol("vkGetInstanceProcAddr"));
		if (!vkGetInstanceProcAddr) {
			std::cerr << "Error: Can't get 'vkGetInstanceProcAddr' as exposed function" << std::endl;
			return false;
		}

		auto loadGlobalFunction = [&](const char* name, bool optional) -> PFN_vkVoidFunction {
			PFN_vkVoidFunction fun{ vkGetInstanceProcAddr(nullptr, name) };
			if (!fun && !optional) {
				throw std::runtime_error(name);
			}

			return fun;
		};

		try {
			#define NTH_GLOBAL_FUNCTION(fun) fun = reinterpret_cast<PFN_##fun>(loadGlobalFunction(#fun, false));
			#define NTH_GLOBAL_FUNCTION_OPTIONAL(fun) fun = reinterpret_cast<PFN_##fun>(loadGlobalFunction(#fun, true));

			#include "Renderer/GlobalFunctions.inl"
		}
		catch (std::exception& e) {
			std::cerr << "Error: Can't load " << e.what() << " function" << std::endl;
			return false;
		}

		return true;
	}

	void VulkanLoader::unitialize(){
		if (m_vulkan.isValid()) {
			m_vulkan.free();
		}
	}

	std::vector<VkLayerProperties> VulkanLoader::enumerateLayerProperties(){
		uint32_t count{ 0 };
		VkResult result{ vkEnumerateInstanceLayerProperties(&count, nullptr) };
		if (result != VkResult::VK_SUCCESS || count == 0) {
			std::cerr << "Error: Could not get number of instance layers : " << toString(result) << std::endl;
		}

		std::vector<VkLayerProperties> layers(count);
		result = vkEnumerateInstanceLayerProperties(&count, layers.data());
		if (result != VkResult::VK_SUCCESS) {
			std::cerr << "Error: Could not enumerate instance layers : " << toString(result) << std::endl;
		}

		return layers;
	}

	std::vector<VkExtensionProperties> VulkanLoader::enumerateExtensionProperties(char const* layerName){
		uint32_t count{ 0 };
		VkResult result{ vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr) };
		if (result != VkResult::VK_SUCCESS || count == 0) {
			std::cerr << "Error: Could not get number of instance layers : " << toString(result) << std::endl;
		}

		std::vector<VkExtensionProperties> extensions(count);
		result = vkEnumerateInstanceExtensionProperties(layerName, &count, extensions.data());
		if (result != VkResult::VK_SUCCESS) {
			std::cerr << "Error: Could not enumerate instance layers : " << toString(result) << std::endl;
		}

		return extensions;
	}

	PFN_vkVoidFunction VulkanLoader::getInstanceProcAddr(VkInstance const& instance, std::string const& name){
		return vkGetInstanceProcAddr(instance, name.c_str());
	}
}
