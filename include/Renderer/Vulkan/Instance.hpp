#ifndef NTH_RENDERER_VK_INSTANCE_HPP
#define NTH_RENDERER_VK_INSTANCE_HPP

#include <vector>
#include <string_view>
#include <unordered_set>

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class PhysicalDevice;

		class Instance {
		public:
			Instance();
			Instance(Instance const&) = delete;
			Instance(Instance&&) = delete;
			~Instance();

			bool create(const std::string_view appName, uint32_t appVersion, const std::string_view engineName, uint32_t engineVersion,
				uint32_t apiVersion, std::vector<const char*> const& layers, std::vector<const char*> const& extensions);

			bool isValid() const;
			bool isLoadedExtension(const std::string_view name) const;
			bool isLoadedLayer(const std::string_view name) const;

			VkInstance const& operator()() const;

			std::vector<PhysicalDevice> enumeratePhysicalDevices();

			#define NTH_RENDERER_VK_INSTANCE_FUNCTION(fun) PFN_##fun fun;
			#include "Renderer/Vulkan/InstanceFunctions.inl"

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
}

#endif
