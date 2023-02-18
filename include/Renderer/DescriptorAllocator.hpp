#ifndef NTH_RENDERER_DESCRIPTORALLOCATOR_HPP
#define NTH_RENDERER_DESCRIPTORALLOCATOR_HPP

#include <Renderer/Vulkan/DescriptorPool.hpp>

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
		DescriptorAllocator(const DescriptorAllocator&) = delete;
		DescriptorAllocator(DescriptorAllocator&&) = default;
		~DescriptorAllocator() = default;

		void init(const Vk::Device& device);
		void reset_pools();

		Vk::DescriptorSet allocate(const Vk::DescriptorSetLayout& layout);

		DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
		DescriptorAllocator& operator=(DescriptorAllocator&&) = default;

	private:
		Vk::DescriptorPool create_pool(uint32_t count, VkDescriptorPoolCreateFlags flags);
		Vk::DescriptorPool grab_pool();

		Vk::Device const* m_device;
		Vk::DescriptorPool* m_current_pool;
		std::vector<Vk::DescriptorPool> m_used_pools;
		std::vector<Vk::DescriptorPool> m_free_pools;
	};
}

#endif