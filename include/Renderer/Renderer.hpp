#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include "Renderer/Vulkan/VulkanInstance.hpp"
#include "Renderer/Vulkan/DescriptorSetLayout.hpp"
#include "Renderer/RenderWindow.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/DescriptorAllocator.hpp"

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
		
		Vk::DescriptorSetLayout getMainDescriptorLayout() const;
		Vk::DescriptorSetLayout getSSBODescriptorLayout() const;

		// TODO: Move out, used for sync destructor
		void waitIdle() const;

		Renderer& operator=(Renderer const&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		Vk::VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		std::vector<Material> m_materials;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: Maybe move it out ?
		Vk::DescriptorSetLayout m_mainDescriptorLayout;
		Vk::DescriptorSetLayout m_ssboDescriptorLayout;
	};
}

#endif