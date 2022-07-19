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
			PipelineLayout(PipelineLayout const&) = delete;
			PipelineLayout(PipelineLayout&& object) noexcept;
			~PipelineLayout();

			bool create(Device const& device, VkPipelineLayoutCreateFlags flags, uint32_t descriptorCount, VkDescriptorSetLayout const* descriptotSetLayouts, uint32_t constantCount, VkPushConstantRange const* pushConstantRanges);
			void destroy();

			bool isValid();

			VkPipelineLayout const& operator()() const;

			PipelineLayout& operator=(PipelineLayout const&) = delete;
			PipelineLayout& operator=(PipelineLayout&& object) noexcept;

		private:
			VkPipelineLayout m_pipelineLayout;
			Device const* m_device;
		};
	}
}

#endif
