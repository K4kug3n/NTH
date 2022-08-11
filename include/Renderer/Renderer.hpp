#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include "Renderer/VulkanInstance.hpp"
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
		VulkanTexture createTexture(const std::string_view name);
		Material createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName);

		void createMesh(Mesh& mesh);

		void draw(std::vector<RenderObject> const& objects);

		// TODO: review it
		Vk::DescriptorSetLayout getMainDescriptorLayout() const;
		Vk::DescriptorSetLayout getTextureDescriptorLayout() const;
		Vk::DescriptorSetLayout getSSBODescriptorLayout() const;

		// TODO: Move out, used for sync destructor
		void waitIdle() const;

		Renderer& operator=(Renderer const&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		bool createUniformBuffer();
		bool copyUniformBufferData();
		UniformBufferObject getUniformBufferData() const;
		bool updateDescriptorSet();
		bool createRenderingResources();
		bool createSSBO();

		VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: Move it out
		Vk::DescriptorSetLayout m_mainDescriptorLayout;
		Vk::DescriptorSetLayout m_textureDescriptorLayout;
		Vk::DescriptorSetLayout m_ssboDescriptorLayout;

		std::vector<RenderingResource> m_renderingResources;
		size_t m_resourceIndex;
	};
}

#endif