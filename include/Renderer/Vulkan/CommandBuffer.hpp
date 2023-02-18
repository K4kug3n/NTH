#ifndef NTH_RENDERER_VK_COMMANDBUFFER_HPP
#define NTH_RENDERER_VK_COMMANDBUFFER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class CommandPool;

		class CommandBuffer {
		public:
			CommandBuffer();
			CommandBuffer(CommandPool const* pool, VkCommandBuffer& commandBuffer);
			CommandBuffer(const CommandBuffer&) = delete;
			CommandBuffer(CommandBuffer&& object) noexcept;
			~CommandBuffer();

			void begin(const VkCommandBufferBeginInfo& infos) const;
			void begin_render_pass(const VkRenderPassBeginInfo& render_pass_begin, VkSubpassContents contents) const;
			void bind_vertex_buffer(VkBuffer buffer, VkDeviceSize offset) const;
			void bind_index_buffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) const;
			void bind_descriptor_sets(VkPipelineLayout layout, uint32_t first_set, uint32_t descriptor_set_count, VkDescriptorSet const* descriptor_sets, uint32_t dynamic_offset_count, uint32_t const* dynamic_offsets) const;
			void bind_pipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) const;
			
			void clear_color_image(VkImage image, VkImageLayout layout, const VkClearColorValue& color, uint32_t range_count, VkImageSubresourceRange const* p_ranges) const;
			void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, const VkBufferCopy& buffer_copy_infos) const;
			void copy_buffer_to_image(VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout, uint32_t region_count, VkBufferImageCopy const* p_regions) const;

			void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const;
			void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) const;

			void end() const;
			void end_render_pass() const;

			void free();

			void pipeline_barrier(VkPipelineStageFlags src_mask, VkPipelineStageFlags dst_mask, VkDependencyFlags dependency_flags,
				uint32_t memory_barrier_count, VkMemoryBarrier const* memory_barriers, uint32_t buffer_memory_barrier_count,
				VkBufferMemoryBarrier const* buffer_memory_barriers, uint32_t image_memory_barrier_count, VkImageMemoryBarrier const* image_memory_barriers) const;
			void push_constants(VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, void const* p_values) const;

			void set_viewport(const VkViewport& viewport) const;
			void set_scissor(const VkRect2D& scissor) const;

			VkCommandBuffer operator()() const;

			CommandBuffer& operator=(const CommandBuffer&) = delete;
			CommandBuffer& operator=(CommandBuffer&& object) noexcept;

		private:
			VkCommandBuffer m_command_buffer;
			CommandPool const* m_pool;
		};
	}
}

#endif
