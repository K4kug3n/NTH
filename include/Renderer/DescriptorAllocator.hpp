#ifndef NTH_RENDERER_DESCRIPTORALLOCATOR_HPP
#define NTH_RENDERER_DESCRIPTORALLOCATOR_HPP

#include "Renderer/Vulkan/DescriptorPool.hpp"

#include <vector>

namespace Nth {
	namespace Vk {
		class Device;
		class DescriptorSet;
		class DescriptorSetLayout;
	}

	class DescriptorAllocator {
	public:
		struct PoolSizes {
			std::vector<std::pair<VkDescriptorType, float>> sizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
			};
		};

		DescriptorAllocator();
		DescriptorAllocator(DescriptorAllocator const&) = delete;
		DescriptorAllocator(DescriptorAllocator&&) = default;
		~DescriptorAllocator() = default;

		void init(Vk::Device const& device);
		void resetPools();

		Vk::DescriptorSet allocate(Vk::DescriptorSetLayout const& layout);

		DescriptorAllocator& operator=(DescriptorAllocator const&) = delete;
		DescriptorAllocator& operator=(DescriptorAllocator&&) = default;

	private:
		Vk::DescriptorPool createPool(uint32_t count, VkDescriptorPoolCreateFlags flags);
		Vk::DescriptorPool grabPool();

		Vk::Device const* m_device;
		Vk::DescriptorPool* m_currentPool;
		std::vector<Vk::DescriptorPool> m_usedPools;
		std::vector<Vk::DescriptorPool> m_freePools;
	};
}

#endif