#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		CommandBuffer::CommandBuffer() :
			m_command_buffer(VK_NULL_HANDLE),
			m_pool(nullptr) {
		}

		CommandBuffer::CommandBuffer(CommandPool const* pool, VkCommandBuffer& commandBuffer) :
			m_command_buffer(commandBuffer),
			m_pool(pool) {
		}

		CommandBuffer::CommandBuffer(CommandBuffer&& object) noexcept :
			m_command_buffer(object.m_command_buffer),
			m_pool(object.m_pool) {
			object.m_command_buffer = VK_NULL_HANDLE;
		}

		CommandBuffer::~CommandBuffer() {
			free();
		}

		void CommandBuffer::begin(const VkCommandBufferBeginInfo& infos) const {
			VkResult result{ m_pool->get_device()->vkBeginCommandBuffer(m_command_buffer, &infos) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't begin record command buffer, " + to_string(result));
			}

		}

		void CommandBuffer::begin_render_pass(const VkRenderPassBeginInfo& render_pass_begin, VkSubpassContents contents) const {
			m_pool->get_device()->vkCmdBeginRenderPass(m_command_buffer, &render_pass_begin, contents);
		}

		void CommandBuffer::bind_pipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) const {
			m_pool->get_device()->vkCmdBindPipeline(m_command_buffer, pipeline_bind_point, pipeline);
		}

		void CommandBuffer::bind_vertex_buffer(VkBuffer buffer, VkDeviceSize offset) const {
			m_pool->get_device()->vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &buffer, &offset);
		}

		void CommandBuffer::bind_index_buffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) const {
			m_pool->get_device()->vkCmdBindIndexBuffer(m_command_buffer, buffer, offset, index_type);
		}

		void CommandBuffer::bind_descriptor_sets(VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptor_set_count, VkDescriptorSet const* descriptor_sets, uint32_t dynamic_offset_count, uint32_t const* dynamic_offsets) const {
			m_pool->get_device()->vkCmdBindDescriptorSets(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, firstSet, descriptor_set_count, descriptor_sets, dynamic_offset_count, dynamic_offsets);
		}

		void CommandBuffer::clear_color_image(VkImage image, VkImageLayout layout, const VkClearColorValue& color, uint32_t range_count, VkImageSubresourceRange const* p_ranges) const {
			m_pool->get_device()->vkCmdClearColorImage(m_command_buffer, image, layout, &color, range_count, p_ranges);
		}

		void CommandBuffer::copy_buffer_to_image(VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout, uint32_t region_count, VkBufferImageCopy const* p_regions) const {
			m_pool->get_device()->vkCmdCopyBufferToImage(m_command_buffer, src_buffer, dst_image, dst_image_layout, region_count, p_regions);
		}

		void CommandBuffer::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, const VkBufferCopy& buffer_copy_infos) const {
			m_pool->get_device()->vkCmdCopyBuffer(m_command_buffer, src_buffer, dst_buffer, 1, &buffer_copy_infos);
		}

		void CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const {
			m_pool->get_device()->vkCmdDraw(m_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
		}

		void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) const {
			m_pool->get_device()->vkCmdDrawIndexed(m_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
		}

		void CommandBuffer::end() const {
			VkResult result{ m_pool->get_device()->vkEndCommandBuffer(m_command_buffer) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Error: Can't end record command buffer, " + to_string(result));
			}
		}

		void CommandBuffer::end_render_pass() const {
			m_pool->get_device()->vkCmdEndRenderPass(m_command_buffer);
		}

		void CommandBuffer::free() {
			if (m_command_buffer != VK_NULL_HANDLE) {
				m_pool->get_device()->vkFreeCommandBuffers((*m_pool->get_device())(), (*m_pool)(), 1, &m_command_buffer);
				m_command_buffer = VK_NULL_HANDLE;
			}
		}

		void CommandBuffer::pipeline_barrier(VkPipelineStageFlags src_mask, VkPipelineStageFlags dst_mask, VkDependencyFlags dependency_flags, uint32_t memory_barrier_count, VkMemoryBarrier const* memory_barriers, uint32_t buffer_memory_barrier_count, VkBufferMemoryBarrier const* buffer_memory_barriers, uint32_t image_memory_barrier_count, VkImageMemoryBarrier const* image_memory_barriers) const {
			m_pool->get_device()->vkCmdPipelineBarrier(m_command_buffer, src_mask, dst_mask, dependency_flags, memory_barrier_count, memory_barriers, buffer_memory_barrier_count, buffer_memory_barriers, image_memory_barrier_count, image_memory_barriers);
		}

		void CommandBuffer::set_viewport(const VkViewport& viewport) const {
			m_pool->get_device()->vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
		}

		void CommandBuffer::set_scissor(const VkRect2D& scissor) const {
			m_pool->get_device()->vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
		}		

		void CommandBuffer::push_constants(VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, void const* pValues) const {
			m_pool->get_device()->vkCmdPushConstants(m_command_buffer, layout, stage_flags, offset, size, pValues);
		}

		VkCommandBuffer CommandBuffer::operator()() const {
			return m_command_buffer;
		}

		CommandBuffer& CommandBuffer::operator=(CommandBuffer&& object) noexcept {
			std::swap(m_command_buffer, object.m_command_buffer);
			m_pool = object.m_pool;

			return *this;
		}
	}
}
