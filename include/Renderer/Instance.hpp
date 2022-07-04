#pragma once

#ifndef NTH_INSTANCE_HPP
#define NTH_INSTANCE_HPP

#include <vector>
#include <string>
#include <unordered_set>

#include <vulkan/vulkan.h>

namespace Nth {

	class PhysicalDevice;

	class Instance {
	public:
		Instance();
		Instance(Instance const&) = delete;
		Instance(Instance&&) = delete;
		~Instance();

		bool create(std::string const& appName, uint32_t appVersion, std::string const& engineName, uint32_t engineVersion,
			uint32_t apiVersion, std::vector<const char*> const& layers, std::vector<const char*> const& extensions);

		bool isValid() const;
		bool isLoadedExtension(std::string const& name) const;
		bool isLoadedLayer(std::string const& name) const;

		VkInstance const& operator()() const;

		std::vector<PhysicalDevice> enumeratePhysicalDevices();

		//PFN_vkVoidFunction vkGetDeviceProcAddr(VkDevice const& device, std::string const& name);
		
		#define NTH_INSTANCE_FUNCTION(fun) PFN_##fun fun;
		#include "Renderer/InstanceFunctions.inl"

		Instance& operator=(Instance const&) = delete;
		Instance& operator=(Instance&&) = delete;
	private:
		bool create(VkInstanceCreateInfo const& infos);

		PFN_vkVoidFunction loadInstanceFunction(const char* name);

		VkInstance m_instance;
		std::unordered_set<std::string> m_layers;
		std::unordered_set<std::string> m_extensions;
	};
}

#endif
