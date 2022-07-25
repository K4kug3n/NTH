#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include "Renderer/Vulkan/VulkanInstance.hpp"
#include "Renderer/RenderWindow.hpp"
#include "Renderer/Material.hpp"

#include <string_view>

namespace Nth {
	class Renderer {
	public:
		Renderer();
		Renderer(Renderer const&) = delete;
		Renderer(Renderer&&) = default;
		~Renderer() = default;

		RenderWindow& getWindow(VideoMode const& mode, const std::string_view title);

		Material& createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName);

		Renderer& operator=(Renderer const&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		Vk::VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		std::vector<Material> m_materials;
	};
}

#endif