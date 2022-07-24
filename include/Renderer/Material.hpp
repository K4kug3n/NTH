#pragma once

#ifndef NTH_RENDERER_MATERIAL_HPP
#define NTH_RENDERER_MATERIAL_HPP

#include "Renderer/Pipeline.hpp"
#include "Renderer/PipelineLayout.hpp"

namespace Nth {
	class Material {
	public:
		Material() = default;
		Material(Material const&) = delete;
		Material(Material&&) = default;
		~Material() = default;

		Material& operator=(Material const&) = delete;
		Material& operator=(Material&&) = default;

		Vk::Pipeline pipeline;
		Vk::PipelineLayout pipelineLayout;
	};
}

#endif