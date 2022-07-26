#include "Renderer/DescriptorAllocator.hpp"

#include "Renderer/Vulkan/DescriptorSet.hpp"
#include "Renderer/Vulkan/DescriptorSetLayout.hpp"
#include "Renderer/Vulkan/VkUtil.hpp"

#include <iostream>
#include <cassert>

namespace Nth {
	DescriptorAllocator::DescriptorAllocator() :
		m_currentPool(nullptr),
		m_device(nullptr) { }
	void DescriptorAllocator::init(Vk::Device const& device) {
		m_device = &device;
	}

	void DescriptorAllocator::resetPools() {
		for (auto&& pools : m_usedPools) {
			if (!pools.reset()) {
				throw std::runtime_error("Can't reset descriptor pool");
			}

			m_freePools.emplace_back(std::move(pools));
		}

		m_usedPools.clear();

		m_currentPool = nullptr;
	}

	Vk::DescriptorSet DescriptorAllocator::allocate(Vk::DescriptorSetLayout const& layout) {
		assert(m_device != nullptr);

		if (m_currentPool == nullptr) {
			m_usedPools.emplace_back(grabPool());
			m_currentPool = &m_usedPools.back();
		}

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			(*m_currentPool)(),                             // VkDescriptorPool               descriptorPool
			1,                                              // uint32_t                       descriptorSetCount
			&layout()                                       // const VkDescriptorSetLayout   *pSetLayouts
		};

		Vk::DescriptorSet set;
		VkResult allocResult = set.allocate(*m_device, descriptorSetAllocateInfo);
		bool needReallocate = false;

		switch (allocResult) {
		case VK_SUCCESS:
			return set;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			needReallocate = true;
			break;
		default:
			throw std::runtime_error("Can't allocate new descriptor set " + Vk::toString(allocResult));
		}

		if (needReallocate) {
			m_usedPools.emplace_back(grabPool());
			m_currentPool = &m_usedPools.back();

			allocResult = set.allocate(*m_device, descriptorSetAllocateInfo);

			if (allocResult == VK_SUCCESS) {
				return set;
			}
		}

		throw std::runtime_error("Can't allocate new descriptor set " + Vk::toString(allocResult));
	}

	Vk::DescriptorPool DescriptorAllocator::createPool(uint32_t count, VkDescriptorPoolCreateFlags flags) {
		PoolSizes poolSizes;

		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(poolSizes.sizes.size());
		for (auto sz : poolSizes.sizes) {
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

		Vk::DescriptorPool descriptorPool;
		if (!descriptorPool.create(*m_device, descriptorPoolCreateInfo)) {
			throw std::runtime_error("Can't create new descriptor pool");
		}

		return descriptorPool;
	}

	Vk::DescriptorPool DescriptorAllocator::grabPool() {
		if (m_freePools.size() > 0) {
			Vk::DescriptorPool pool = std::move(m_freePools.back());
			m_freePools.pop_back();

			return pool;
		}

		return createPool(1000, 0);
	}
}
