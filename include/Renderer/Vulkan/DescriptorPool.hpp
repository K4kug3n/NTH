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
			DescriptorPool(DescriptorPool const&) = delete;
			DescriptorPool(DescriptorPool&& object) noexcept;
			~DescriptorPool();

			bool create(Device const& device, VkDescriptorPoolCreateInfo const& info);
			void destroy();

			VkDescriptorPool operator()() const;

			DescriptorPool& operator=(DescriptorPool const&) = delete;
			DescriptorPool& operator=(DescriptorPool&& object) noexcept;

		private:
			VkDescriptorPool m_descriptorPool;
			Device const* m_device;
		};
	}
}

#endif
