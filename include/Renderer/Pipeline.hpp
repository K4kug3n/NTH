#pragma once

#ifndef NTH_PIPELINE_HPP
#define NTH_PIPELINE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	class Device;

	class Pipeline {
	public:
		Pipeline();
		Pipeline(Pipeline const&) = delete;
		Pipeline(Pipeline&& object) noexcept;
		~Pipeline();

		bool createGraphics(Device const& device, VkPipelineCache cache, VkGraphicsPipelineCreateInfo const& infos);
		void destroy();

		VkPipeline const& operator()() const;

		Pipeline& operator=(Pipeline const&) = delete;
		Pipeline& operator=(Pipeline&& object) noexcept;

	private:
		VkPipeline m_pipeline;
		Device const* m_device;
	};
}

#endif
