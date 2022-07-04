#include "Renderer/CommandBuffer.hpp"

#include "Renderer/CommandPool.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"

#include <iostream>

namespace Nth {
	CommandBuffer::CommandBuffer() :
		m_commandBuffer(VK_NULL_HANDLE),
		m_pool(nullptr) {
	}

	CommandBuffer::CommandBuffer(CommandPool const* pool, VkCommandBuffer & commandBuffer):
		m_commandBuffer(commandBuffer),
		m_pool(pool) {
	}

	CommandBuffer::CommandBuffer(CommandBuffer&& object) noexcept :
		m_commandBuffer(object.m_commandBuffer),
		m_pool(object.m_pool) {
		object.m_commandBuffer = VK_NULL_HANDLE;
	}

	CommandBuffer::~CommandBuffer() {
		free();
	}

	void CommandBuffer::free() {
		if (m_commandBuffer != VK_NULL_HANDLE) {
			m_pool->getDevice()->vkFreeCommandBuffers((*m_pool->getDevice())(), (*m_pool)(), 1, &m_commandBuffer);
		}
	}

	bool CommandBuffer::begin(VkCommandBufferBeginInfo const& infos) const {
		VkResult result{ m_pool->getDevice()->vkBeginCommandBuffer(m_commandBuffer, &infos) };
		if (result != VK_SUCCESS) {
			std::cerr << "Error: Can't begin record command buffer, " << toString(result) << std::endl;
			return false;
		}

		return true;
	}

	void CommandBuffer::clearColorImage(VkImage image, VkImageLayout layout, VkClearColorValue const& color, uint32_t rangeCount, VkImageSubresourceRange const* pRanges) const {
		m_pool->getDevice()->vkCmdClearColorImage(m_commandBuffer, image, layout, &color, rangeCount, pRanges);
	}

	void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, VkMemoryBarrier const* memoryBarriers, uint32_t bufferMemoryBarrierCount, VkBufferMemoryBarrier const* bufferMemoryBarriers, uint32_t imageMemoryBarrierCount, VkImageMemoryBarrier const* imageMemoryBarriers) const {
		m_pool->getDevice()->vkCmdPipelineBarrier(m_commandBuffer, srcMask, dstMask, dependencyFlags, memoryBarrierCount, memoryBarriers, bufferMemoryBarrierCount, bufferMemoryBarriers, imageMemoryBarrierCount, imageMemoryBarriers);
	}

	void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo const& renderPassBegin, VkSubpassContents contents) const {
		m_pool->getDevice()->vkCmdBeginRenderPass(m_commandBuffer, &renderPassBegin, contents);
	}

	void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const {
		m_pool->getDevice()->vkCmdBindPipeline(m_commandBuffer, pipelineBindPoint, pipeline);
	}

	void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
		m_pool->getDevice()->vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandBuffer::setViewport(VkViewport const& viewport) const {
		m_pool->getDevice()->vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
	}

	void CommandBuffer::setScissor(VkRect2D const& scissor) const {
		m_pool->getDevice()->vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
	}

	void CommandBuffer::bindVertexBuffer(VkBuffer buffer, VkDeviceSize offset) const {
		m_pool->getDevice()->vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &buffer, &offset);
	}

	void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const {
		m_pool->getDevice()->vkCmdBindIndexBuffer(m_commandBuffer, buffer, offset, indexType);
	}

	void CommandBuffer::bindDescriptorSets(VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, VkDescriptorSet const* descriptorSets, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) const {
		m_pool->getDevice()->vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, firstSet, descriptorSetCount, descriptorSets, dynamicOffsetCount, dynamicOffsets);
	}

	void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const {
		m_pool->getDevice()->vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void CommandBuffer::pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, void const* pValues) const {
		m_pool->getDevice()->vkCmdPushConstants(m_commandBuffer, layout, stageFlags, offset, size, pValues);
	}

	void CommandBuffer::copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, VkBufferImageCopy const* pRegions) const {
		m_pool->getDevice()->vkCmdCopyBufferToImage(m_commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
	}

	void CommandBuffer::endRenderPass() const {
		m_pool->getDevice()->vkCmdEndRenderPass(m_commandBuffer);
	}

	void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkBufferCopy const& bufferCopyInfos) const {
		m_pool->getDevice()->vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyInfos);
	}

	bool CommandBuffer::end() const {
		VkResult result{ m_pool->getDevice()->vkEndCommandBuffer(m_commandBuffer) };
		if (result != VK_SUCCESS) {
			std::cerr << "Error: Can't end record command buffer, " << toString(result) << std::endl;
			return false;
		}
		return true;
	}

	VkCommandBuffer const& CommandBuffer::operator()() const {
		return m_commandBuffer;
	}

	CommandBuffer& CommandBuffer::operator=(CommandBuffer&& object) noexcept {
		m_commandBuffer = object.m_commandBuffer;
		m_pool = object.m_pool;

		return *this;
	}

}
