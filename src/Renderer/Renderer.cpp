#include "Renderer/Renderer.hpp"

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		m_renderWindow.create(mode, title);

		return m_renderWindow;
	}

	Material& Renderer::createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName) {
		Material material;
		//material.createPipeline(m_vulkan.getDevice(), , vertexShaderName, fragmentShaderName);

		return m_materials.emplace_back(std::move(material));
	}
}
