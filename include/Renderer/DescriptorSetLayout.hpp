#pragma once

#ifndef NTH_RENDERER_VK_DESCRIPTORSETLAYOUT_HPP
#define NTH_RENDERER_VK_DESCRIPTORSETLAYOUT_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class DescriptorSetLayout {
		public:
			DescriptorSetLayout();
			DescriptorSetLayout(DescriptorSetLayout const&) = delete;
			DescriptorSetLayout(DescriptorSetLayout&& object) noexcept;
			~DescriptorSetLayout();

			bool create(Device const& device, VkDescriptorSetLayoutCreateInfo const& descriptorSetLayoutInfo);

			VkDescriptorSetLayout const& operator()() const;

			DescriptorSetLayout& operator=(DescriptorSetLayout const&) = delete;
			DescriptorSetLayout& operator=(DescriptorSetLayout&& object) noexcept;

		private:
			VkDescriptorSetLayout m_descriptorSetLayout;
			Device const* m_device;
		};
	}
}

#endif
