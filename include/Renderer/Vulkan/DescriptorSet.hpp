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
			DescriptorSet(DescriptorSet const&) = delete;
			DescriptorSet(DescriptorSet&& object) noexcept;
			~DescriptorSet();

			VkResult allocate(Device const& device, VkDescriptorSetAllocateInfo const& info);
			
			void update(uint32_t nbSetWrite, VkWriteDescriptorSet const* setWrites);

			bool isValid();

			VkDescriptorSet operator()() const;

			DescriptorSet& operator=(DescriptorSet const&) = delete;
			DescriptorSet& operator=(DescriptorSet&& object) noexcept;

		private:
			VkDescriptorSet m_descriptorSet;
			Device const* m_device;
		};
	}
}

#endif
