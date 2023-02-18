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
			Instance(const Instance&) = delete;
			Instance(Instance&&) = delete;
			~Instance();

			void create(const std::string_view app_name, uint32_t app_version, std::string_view engine_name, uint32_t engine_version,
				uint32_t api_version, const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
			void create(const VkInstanceCreateInfo& infos);

			bool is_valid() const;
			bool is_loaded_extension(std::string_view name) const;
			bool is_loaded_layer(std::string_view name) const;

			VkInstance operator()() const;

			std::vector<PhysicalDevice> enumerate_physical_devices();

			#define NTH_RENDERER_VK_INSTANCE_FUNCTION(fun) PFN_##fun fun;
			#include <Renderer/Vulkan/InstanceFunctions.inl>

			Instance& operator=(const Instance&) = delete;
			Instance& operator=(Instance&&) = delete;
		private:
			PFN_vkVoidFunction load_instance_function(const char* name);

			VkInstance m_instance;
			std::unordered_set<std::string> m_layers;
			std::unordered_set<std::string> m_extensions;
		};
	}
}

#endif
