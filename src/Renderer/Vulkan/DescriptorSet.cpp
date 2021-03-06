#include "Renderer/Vulkan/DescriptorSet.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/VkUtil.hpp"

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		DescriptorSet::DescriptorSet() :
			m_descriptorSet(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorSet::DescriptorSet(DescriptorSet&& object) noexcept :
			m_descriptorSet(object.m_descriptorSet),
			m_device(object.m_device) {
			object.m_descriptorSet = VK_NULL_HANDLE;
		}

		DescriptorSet::~DescriptorSet() {
		}

		VkResult DescriptorSet::allocate(Device const& device, VkDescriptorSetAllocateInfo const& info) {
			m_device = &device;

			return device.vkAllocateDescriptorSets(device(), &info, &m_descriptorSet);
		}

		void DescriptorSet::update(uint32_t nbSetWrite, VkWriteDescriptorSet const* setWrites) {
			assert(m_device != nullptr);

			m_device->vkUpdateDescriptorSets((*m_device)(), nbSetWrite, setWrites, 0, nullptr);
		}		

		bool DescriptorSet::isValid() {
			return m_descriptorSet != VK_NULL_HANDLE;
		}

		VkDescriptorSet DescriptorSet::operator()() const {
			return m_descriptorSet;
		}

		DescriptorSet& DescriptorSet::operator=(DescriptorSet&& object) noexcept {
			m_descriptorSet = object.m_descriptorSet;
			m_device = object.m_device;

			object.m_descriptorSet = VK_NULL_HANDLE;

			return *this;
		}
	}
}
