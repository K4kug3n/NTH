#pragma once

#ifndef NTH_RENDERER_VK_DESCRIPTORSET_HPP
#define NTH_RENDERER_VK_DESCRIPTORSET_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class DescriptorSet {
		public:
			DescriptorSet();
			DescriptorSet(const DescriptorSet&) = delete;
			DescriptorSet(DescriptorSet&& object) noexcept;
			~DescriptorSet();

			VkResult allocate(const Device& device, const VkDescriptorSetAllocateInfo& info);
			
			bool is_valid();

			void update(uint32_t nb_set_write, VkWriteDescriptorSet const* set_writes);

			VkDescriptorSet operator()() const;

			DescriptorSet& operator=(const DescriptorSet&) = delete;
			DescriptorSet& operator=(DescriptorSet&& object) noexcept;

		private:
			VkDescriptorSet m_descriptor_set;
			Device const* m_device;
		};
	}
}

#endif
