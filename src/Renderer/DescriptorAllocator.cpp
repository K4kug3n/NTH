#include <Renderer/DescriptorAllocator.hpp>

#include <Renderer/Vulkan/DescriptorSet.hpp>
#include <Renderer/Vulkan/DescriptorSetLayout.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>
#include <cassert>

namespace Nth {
	DescriptorAllocator::DescriptorAllocator() :
		m_current_pool(nullptr),
		m_device(nullptr) { }
	void DescriptorAllocator::init(const Vk::Device& device) {
		m_device = &device;
	}

	void DescriptorAllocator::reset_pools() {
		for (auto&& pools : m_used_pools) {
			pools.reset();

			m_free_pools.emplace_back(std::move(pools));
		}

		m_used_pools.clear();

		m_current_pool = nullptr;
	}

	Vk::DescriptorSet DescriptorAllocator::allocate(const Vk::DescriptorSetLayout& layout) {
		assert(m_device != nullptr);

		if (m_current_pool == nullptr) {
			m_used_pools.emplace_back(grab_pool());
			m_current_pool = &m_used_pools.back();
		}

		const VkDescriptorSetLayout vk_layout = layout();
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			(*m_current_pool)(),                            // VkDescriptorPool               descriptorPool
			1,                                              // uint32_t                       descriptorSetCount
			&vk_layout                                      // const VkDescriptorSetLayout   *pSetLayouts
		};

		Vk::DescriptorSet set;
		VkResult allocResult = set.allocate(*m_device, descriptor_set_allocate_info);
		bool needReallocate = false;

		switch (allocResult) {
		case VK_SUCCESS:
			return set;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			needReallocate = true;
			break;
		default:
			throw std::runtime_error("Can't allocate new descriptor set, " + Vk::to_string(allocResult));
		}

		if (needReallocate) {
			m_used_pools.emplace_back(grab_pool());
			m_current_pool = &m_used_pools.back();

			allocResult = set.allocate(*m_device, descriptor_set_allocate_info);

			if (allocResult == VK_SUCCESS) {
				return set;
			}
		}

		throw std::runtime_error("Can't allocate new descriptor set, " + Vk::to_string(allocResult));
	}

	Vk::DescriptorPool DescriptorAllocator::create_pool(uint32_t count, VkDescriptorPoolCreateFlags flags) {
		PoolSizes pool_sizes;

		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(pool_sizes.sizes.size());
		for (auto sz : pool_sizes.sizes) {
			sizes.push_back({ sz.first, static_cast<uint32_t>(sz.second * count) });
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,  // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			flags,                                          // VkDescriptorPoolCreateFlags    flags
			count,                                          // uint32_t                       maxSets
			static_cast<uint32_t>(sizes.size()),            // uint32_t                       poolSizeCount
			sizes.data()                                    // const VkDescriptorPoolSize    *pPoolSizes
		};

		Vk::DescriptorPool descriptor_pool;
		descriptor_pool.create(*m_device, descriptorPoolCreateInfo);

		return descriptor_pool;
	}

	Vk::DescriptorPool DescriptorAllocator::grab_pool() {
		if (m_free_pools.size() > 0) {
			Vk::DescriptorPool pool = std::move(m_free_pools.back());
			m_free_pools.pop_back();

			return pool;
		}

		return create_pool(1000, 0);
	}
}
