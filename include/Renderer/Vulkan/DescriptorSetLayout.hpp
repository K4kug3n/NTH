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
			DescriptorSetLayout(const DescriptorSetLayout&) = delete;
			DescriptorSetLayout(DescriptorSetLayout&& object) noexcept;
			~DescriptorSetLayout();

			void create(const Device& device, const VkDescriptorSetLayoutCreateInfo& descriptorSetLayoutInfo);
			void destroy();

			VkDescriptorSetLayout operator()() const;

			DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
			DescriptorSetLayout& operator=(DescriptorSetLayout&& object) noexcept;

		private:
			VkDescriptorSetLayout m_descriptor_set_layout;
			Device const* m_device;
		};
	}
}

#endif
