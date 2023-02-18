#ifndef NTH_RENDERER_VK_PIPELINE_HPP
#define NTH_RENDERER_VK_PIPELINE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Pipeline {
		public:
			Pipeline();
			Pipeline(const Pipeline&) = delete;
			Pipeline(Pipeline&& object) noexcept;
			~Pipeline();

			void create_graphics(const Device& device, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& infos);
			void destroy();

			VkPipeline operator()() const;

			Pipeline& operator=(const Pipeline&) = delete;
			Pipeline& operator=(Pipeline&& object) noexcept;

		private:
			VkPipeline m_pipeline;
			Device const* m_device;
		};
	}
}

#endif
