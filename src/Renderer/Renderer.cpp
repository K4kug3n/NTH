#include "Renderer/Renderer.hpp"

#include "Util/Image.hpp"

#include <cstring>
#include <iostream>

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan),
		m_resourceIndex(0),
		m_renderingResources(RenderWindow::resourceCount) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		if (!m_renderWindow.create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}

		if (!createRenderingResources()) {
			throw std::runtime_error("Can't create rendering ressources");
		}

		if (!createSSBO()) {
			throw std::runtime_error("Can't create ssbo");
		}

		m_stagingBuffer = VulkanBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			5000000
		};

		if (!createTexture()) {
			throw std::runtime_error("Can't create texture!");
		}

		if (!createUniformBuffer()) {
			throw std::runtime_error("Can't create uniform buffer");
		}

		m_mainDescriptorLayout = getMainDescriptorLayout();
		m_ssboDescriptorLayout = getSSBODescriptorLayout();

		m_descriptorAllocator.init(m_vulkan.getDevice().getHandle());

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].mainDescriptor = m_descriptorAllocator.allocate(m_mainDescriptorLayout);
			m_renderingResources[i].ssboDescriptor = m_descriptorAllocator.allocate(m_ssboDescriptorLayout);
		}

		updateDescriptorSet();

		EventHandler& eventHandler = m_renderWindow.getEventHandler();
		eventHandler.onResize.connect([this]() {
			m_vulkan.getDevice().getHandle().waitIdle();

			if (!copyUniformBufferData()) {
				throw std::runtime_error("Can't re-copy uniform data");
			}
		});

		return m_renderWindow;
	}

	Material& Renderer::createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName) {		
		std::vector<VkDescriptorSetLayout> vkDescritptorLayouts {
			m_mainDescriptorLayout(),
			m_ssboDescriptorLayout()
		};

		Material material;
		material.createPipeline(m_vulkan.getDevice().getHandle(), m_renderWindow.getRenderPass(), vertexShaderName, fragmentShaderName, vkDescritptorLayouts);

		return m_materials.emplace_back(std::move(material));
	}

	void Renderer::createMesh(Mesh& mesh) {
		mesh.vertexBuffer = VulkanBuffer{ 
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0]))
		};

		mesh.vertexBuffer.copyByStaging(mesh.vertices.data(), mesh.vertexBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);

		mesh.indexBuffer = VulkanBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			sizeof(mesh.indices[0])* mesh.indices.size()
		};

		mesh.indexBuffer.copyByStaging(mesh.indices.data(), mesh.indexBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);
	}

	void Renderer::draw(std::vector<RenderObject> const& objects) {
		m_renderWindow.draw(m_renderingResources[m_resourceIndex], objects);
		
		m_resourceIndex = (m_resourceIndex + 1) % RenderWindow::resourceCount;
	}

	Vk::DescriptorSetLayout Renderer::getMainDescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler     *pImmutableSamplers
			},
			{
				1,                                         // uint32_t           binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType   descriptorType
				1,                                         // uint32_t           descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                // VkShaderStageFlags stageFlags
				nullptr                                    // const VkSampler *pImmutableSamplers
			}
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
			nullptr,                                              // const void                          *pNext
			0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(layoutBindings.size()),         // uint32_t                             bindingCount
			layoutBindings.data()                                 // const VkDescriptorSetLayoutBinding  *pBindings
		};

		Vk::DescriptorSetLayout layout;
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}

	Vk::DescriptorSetLayout Renderer::getSSBODescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ,         // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler     *pImmutableSamplers
			}
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
			nullptr,                                              // const void                          *pNext
			0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(layoutBindings.size()),        // uint32_t                             bindingCount
			layoutBindings.data()                                // const VkDescriptorSetLayoutBinding  *pBindings
		};

		Vk::DescriptorSetLayout layout;
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}
	void Renderer::waitIdle() const {
		m_vulkan.getDevice().getHandle().waitIdle();
	}

	bool Renderer::createTexture() {
		Image image = Image::loadFromFile("viking_room.png", PixelChannel::Rgba);

		std::vector<char> const& pixels = image.pixels();
		if (pixels.empty()) {
			return false;
		}

		m_image.create(
			m_vulkan.getDevice(),
			image.width(),
			image.height(),
			static_cast<uint32_t>(pixels.size()),
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_image.createView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		if (!copyTextureData(pixels.data(), static_cast<uint32_t>(pixels.size()), image.width(), image.height())) {
			return false;
		}

		return true;
	}

	bool Renderer::copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height) {
		if (!m_stagingBuffer.memory.map(0, dataSize, 0)) {
			std::cerr << "Could not map memory and upload texture data to a staging buffer!" << std::endl;
			return false;
		}
		void* stagingBufferMemoryPointer = m_stagingBuffer.memory.getMappedPointer();

		std::memcpy(stagingBufferMemoryPointer, textureData, dataSize);

		m_stagingBuffer.memory.flushMappedMemory(0, dataSize);

		m_stagingBuffer.memory.unmap();

		// Prepare command buffer to copy data from staging buffer to a vertex buffer
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
			nullptr,                                      // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // VkCommandBufferUsageFlags              flags
			nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer& commandBuffer = m_renderingResources[0].commandBuffer;

		commandBuffer.begin(commandBufferBeginInfo);

		VkImageSubresourceRange image_subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,              // VkImageAspectFlags        aspectMask
			0,                                      // uint32_t                  baseMipLevel
			1,                                      // uint32_t                  levelCount
			0,                                      // uint32_t                  baseArrayLayer
			1                                       // uint32_t                  layerCount
		};

		VkImageMemoryBarrier imageMemoryBarrierFromUndefinedToTransferDst = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType           sType
			nullptr,                                // const void               *pNext
			0,                                      // VkAccessFlags             srcAccessMask
			VK_ACCESS_TRANSFER_WRITE_BIT,           // VkAccessFlags             dstAccessMask
			VK_IMAGE_LAYOUT_UNDEFINED,              // VkImageLayout             oldLayout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // VkImageLayout             newLayout
			VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  dstQueueFamilyIndex
			m_image.image.handle(),                 // VkImage                   image
			image_subresource_range                 // VkImageSubresourceRange   subresourceRange
		};
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierFromUndefinedToTransferDst);

		VkBufferImageCopy bufferImageCopyInfo = {
			0,                                  // VkDeviceSize               bufferOffset
			0,                                  // uint32_t                   bufferRowLength
			0,                                  // uint32_t                   bufferImageHeight
			{                                   // VkImageSubresourceLayers   imageSubresource
				VK_IMAGE_ASPECT_COLOR_BIT,          // VkImageAspectFlags         aspectMask
				0,                                  // uint32_t                   mipLevel
				0,                                  // uint32_t                   baseArrayLayer
				1                                   // uint32_t                   layerCount
			},
			{                                   // VkOffset3D                 imageOffset
				0,                                  // int32_t                    x
				0,                                  // int32_t                    y
				0                                   // int32_t                    z
			},
			{                                   // VkExtent3D                 imageExtent
				width,                              // uint32_t                   width
				height,                             // uint32_t                   height
				1                                   // uint32_t                   depth
			}
		};
		commandBuffer.copyBufferToImage(m_stagingBuffer.handle(), m_image.image.handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);

		VkImageMemoryBarrier imageMemoryBarrierFromTransferToShaderRead = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags                srcAccessMask
			VK_ACCESS_SHADER_READ_BIT,                // VkAccessFlags                dstAccessMask
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout                oldLayout
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // VkImageLayout                newLayout
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     dstQueueFamilyIndex
			m_image.image.handle(),                   // VkImage                      image
			image_subresource_range                   // VkImageSubresourceRange      subresourceRange
		};
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierFromTransferToShaderRead);

		commandBuffer.end();

		// Submit command buffer and copy data from staging buffer to a vertex buffer
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,            // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			0,                                        // uint32_t                     waitSemaphoreCount
			nullptr,                                  // const VkSemaphore           *pWaitSemaphores
			nullptr,                                  // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                        // uint32_t                     commandBufferCount
			&commandBuffer(),                         // const VkCommandBuffer       *pCommandBuffers
			0,                                        // uint32_t                     signalSemaphoreCount
			nullptr                                   // const VkSemaphore           *pSignalSemaphores
		};

		if (!m_vulkan.getDevice().graphicsQueue().submit(submitInfo, VK_NULL_HANDLE)) {
			return false;
		}

		m_vulkan.getDevice().getHandle().waitIdle();

		return true;
	}

	bool Renderer::createUniformBuffer() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].mainBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				sizeof(UniformBufferObject)
			};
		}

		if (!copyUniformBufferData()) {
			return false;
		}

		return true;
	}

	bool Renderer::copyUniformBufferData() {
		const UniformBufferObject uniformData = getUniformBufferData();

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			RenderingResource& current = m_renderingResources[i];

			current.mainBuffer.copyByStaging(&uniformData, current.mainBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);
		}

		return true;
	}

	UniformBufferObject Renderer::getUniformBufferData() const {
		UniformBufferObject ubo{};

		Vector2ui size = m_renderWindow.size();

		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		return ubo;
	}

	bool Renderer::updateDescriptorSet() {
		VkDescriptorImageInfo imageInfo = {
			m_image.sampler(),                          // VkSampler                      sampler
			m_image.image.view(),                       // VkImageView                    imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL    // VkImageLayout                  imageLayout
		};

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			VkDescriptorBufferInfo bufferInfo = {
				m_renderingResources[i].mainBuffer.handle(),             // VkBuffer         buffer
				0,                                                       // VkDeviceSize     offset
				m_renderingResources[i].mainBuffer.handle.getSize()      // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].mainDescriptor(),   // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType               descriptorType
					&imageInfo,                                 // const VkDescriptorImageInfo   *pImageInfo
					nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,    // VkStructureType     sType
					nullptr,                                   // const void         *pNext
					m_renderingResources[i].mainDescriptor(),  // VkDescriptorSet     dstSet
					1,                                         // uint32_t            dstBinding
					0,                                         // uint32_t            dstArrayElement
					1,                                         // uint32_t            descriptorCount
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType    descriptorType
					nullptr,                                   // const VkDescriptorImageInfo  *pImageInfo
					&bufferInfo,                               // const VkDescriptorBufferInfo *pBufferInfo
					nullptr                                    // const VkBufferView *pTexelBufferView
				}
			};

			// TODO: Check if update methode should be in Device class 
			m_renderingResources[i].mainDescriptor.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());


			VkDescriptorBufferInfo ssboInfo = {
				m_renderingResources[i].ssbo.handle(),         // VkBuffer         buffer
				0,                                             // VkDeviceSize     offset
				m_renderingResources[i].ssbo.handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites2 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].ssboDescriptor(),   // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&ssboInfo,                                  // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_renderingResources[i].ssboDescriptor.update(static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data());
		}

		return true;
	}

	bool Renderer::createRenderingResources() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			if (!m_renderingResources[i].create(m_vulkan.getDevice().getHandle(), m_vulkan.getDevice().presentQueue().index())) {
				std::cerr << "Can't create rendering ressource" << std::endl;
				return false;
			}
		}

		return true;
	}

	bool Renderer::createSSBO() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].ssbo = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ShaderStorageBufferObject)
			};
		}

		return true;
	}
}
