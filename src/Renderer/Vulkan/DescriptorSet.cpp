#include <Renderer/Vulkan/DescriptorSet.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		DescriptorSet::DescriptorSet() :
			m_descriptor_set(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorSet::DescriptorSet(DescriptorSet&& object) noexcept :
			m_descriptor_set(object.m_descriptor_set),
			m_device(object.m_device) {
			object.m_descriptor_set = VK_NULL_HANDLE;
		}

		DescriptorSet::~DescriptorSet() {
		}

		VkResult DescriptorSet::allocate(const Device& device, const VkDescriptorSetAllocateInfo& info) {
			m_device = &device;

			return device.vkAllocateDescriptorSets(device(), &info, &m_descriptor_set);
		}

		void DescriptorSet::update(uint32_t nbSetWrite, VkWriteDescriptorSet const* setWrites) {
			assert(m_device != nullptr);

			m_device->vkUpdateDescriptorSets((*m_device)(), nbSetWrite, setWrites, 0, nullptr);
		}		

		bool DescriptorSet::is_valid() {
			return m_descriptor_set != VK_NULL_HANDLE;
		}

		VkDescriptorSet DescriptorSet::operator()() const {
			return m_descriptor_set;
		}

		DescriptorSet& DescriptorSet::operator=(DescriptorSet&& object) noexcept {
			m_descriptor_set = object.m_descriptor_set;
			m_device = object.m_device;

			object.m_descriptor_set = VK_NULL_HANDLE;

			return *this;
		}
	}
}
