#pragma once

#ifndef NTH_RENDERER_VK_DESCRIPTORPOOL_HPP
#define NTH_RENDERER_VK_DESCRIPTORPOOL_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class DescriptorPool {
		public:
			DescriptorPool();
			DescriptorPool(const DescriptorPool&) = delete;
			DescriptorPool(DescriptorPool&& object) noexcept;
			~DescriptorPool();

			void create(const Device& device, const VkDescriptorPoolCreateInfo& info);
			void destroy();
			void reset() const;
			
			VkDescriptorPool operator()() const;

			DescriptorPool& operator=(const DescriptorPool&) = delete;
			DescriptorPool& operator=(DescriptorPool&& object) noexcept;

		private:
			VkDescriptorPool m_descriptor_pool;
			Device const* m_device;
		};
	}
}

#endif
