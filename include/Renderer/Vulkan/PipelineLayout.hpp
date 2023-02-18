#ifndef NTH_RENDERER_VK_PIPELINELAYOUT_HPP
#define NTH_RENDERER_VK_PIPELINELAYOUT_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace Nth {
	namespace Vk {
		class Device;

		class PipelineLayout {
		public:
			PipelineLayout();
			PipelineLayout(const PipelineLayout&) = delete;
			PipelineLayout(PipelineLayout&& object) noexcept;
			~PipelineLayout();

			void create(const Device& device, VkPipelineLayoutCreateFlags flags, uint32_t descriptor_count, 
				VkDescriptorSetLayout const* descriptot_set_layouts, uint32_t constant_count, VkPushConstantRange const* push_constant_ranges);
			void destroy();

			bool is_valid();

			VkPipelineLayout operator()() const;

			PipelineLayout& operator=(const PipelineLayout&) = delete;
			PipelineLayout& operator=(PipelineLayout&& object) noexcept;

		private:
			VkPipelineLayout m_pipeline_layout;
			Device const* m_device;
		};
	}
}

#endif
