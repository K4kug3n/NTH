#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include "Renderer/Vulkan/VulkanInstance.hpp"
#include "Renderer/Vulkan/DescriptorSetLayout.hpp"
#include "Renderer/RenderWindow.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/DescriptorAllocator.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/VulkanTexture.hpp"

#include <vector>
#include <string_view>

namespace Nth {
	class RenderObject;

	class Renderer {
	public:
		Renderer();
		Renderer(Renderer const&) = delete;
		Renderer(Renderer&&) = default;
		~Renderer() = default;

		RenderWindow& getWindow(VideoMode const& mode, const std::string_view title);

		Material& createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName);
		void createMesh(Mesh& mesh);

		void draw(std::vector<RenderObject> const& objects);

		// TODO: review it
		Vk::DescriptorSetLayout getMainDescriptorLayout() const;
		Vk::DescriptorSetLayout getSSBODescriptorLayout() const;

		// TODO: Move out, used for sync destructor
		void waitIdle() const;

		Renderer& operator=(Renderer const&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		bool createTexture();
		bool copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		bool copyBufferByStaging(VulkanBuffer& target, VulkanBuffer& staging, std::function<void(void*)> copyFunction);
		bool createUniformBuffer();
		bool copyUniformBufferData();
		UniformBufferObject getUniformBufferData() const;
		bool updateDescriptorSet();
		bool createRenderingResources();
		bool createSSBO();

		Vk::VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		std::vector<Material> m_materials;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: Maybe move it out ?
		Vk::DescriptorSetLayout m_mainDescriptorLayout;
		Vk::DescriptorSetLayout m_ssboDescriptorLayout;

		VulkanBuffer m_stagingBuffer;
		VulkanTexture m_image;

		std::vector<RenderingResource> m_renderingResources;
		size_t m_resourceIndex;
	};
}

#endif