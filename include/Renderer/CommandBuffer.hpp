#ifndef NTH_RENDERER_VK_COMMANDBUFFER_HPP
#define NTH_RENDERER_VK_COMMANDBUFFER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class CommandPool;
		class Pipeline;
		class Buffer;

		class CommandBuffer {
		public:
			CommandBuffer();
			CommandBuffer(CommandPool const* pool, VkCommandBuffer& commandBuffer);
			CommandBuffer(CommandBuffer const&) = delete;
			CommandBuffer(CommandBuffer&& object) noexcept;
			~CommandBuffer();

			void free();

			bool begin(VkCommandBufferBeginInfo const& infos) const;
			void clearColorImage(VkImage image, VkImageLayout layout, VkClearColorValue const& color, uint32_t rangeCount, VkImageSubresourceRange const* pRanges) const;
			void pipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
				uint32_t memoryBarrierCount, VkMemoryBarrier const* memoryBarriers, uint32_t bufferMemoryBarrierCount,
				VkBufferMemoryBarrier const* bufferMemoryBarriers, uint32_t imageMemoryBarrierCount, VkImageMemoryBarrier const* imageMemoryBarriers) const;
			void beginRenderPass(VkRenderPassBeginInfo const& renderPassBegin, VkSubpassContents contents) const;
			void bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const;
			void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
			void setViewport(VkViewport const& viewport) const;
			void setScissor(VkRect2D const& scissor) const;
			void bindVertexBuffer(VkBuffer buffer, VkDeviceSize offset) const;
			void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
			void bindDescriptorSets(VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, VkDescriptorSet const* descriptorSets, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) const;
			void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const;
			void pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, void const* pValues) const; // Maybe useless with UBO
			void copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, VkBufferImageCopy const* pRegions) const;
			void endRenderPass() const;
			void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkBufferCopy const& bufferCopyInfos) const;
			bool end() const;

			VkCommandBuffer const& operator()() const;

			CommandBuffer& operator=(CommandBuffer const&) = delete;
			CommandBuffer& operator=(CommandBuffer&& object) noexcept;

		private:
			VkCommandBuffer m_commandBuffer;
			CommandPool const* m_pool;
		};
	}
}

#endif
