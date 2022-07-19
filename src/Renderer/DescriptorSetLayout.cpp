#include "Renderer/DescriptorSetLayout.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		DescriptorSetLayout::DescriptorSetLayout() :
			m_descriptorSetLayout(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& object) noexcept :
			m_descriptorSetLayout(object.m_descriptorSetLayout),
			m_device(object.m_device) {
			object.m_descriptorSetLayout = VK_NULL_HANDLE;
		}

		DescriptorSetLayout::~DescriptorSetLayout() {
			destroy();
		}

		bool DescriptorSetLayout::create(Device const& device, VkDescriptorSetLayoutCreateInfo const& descriptorSetLayoutInfo) {
			VkResult result{ device.vkCreateDescriptorSetLayout(device(), &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create Descriptor Set Layout, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void DescriptorSetLayout::destroy() {
			if (m_descriptorSetLayout != VK_NULL_HANDLE) {
				m_device->vkDestroyDescriptorSetLayout((*m_device)(), m_descriptorSetLayout, nullptr);
				m_descriptorSetLayout = VK_NULL_HANDLE;
			}
		}

		VkDescriptorSetLayout const& DescriptorSetLayout::operator()() const {
			return m_descriptorSetLayout;
		}

		DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& object) noexcept {
			destroy();

			m_descriptorSetLayout = object.m_descriptorSetLayout;
			m_device = object.m_device;

			object.m_descriptorSetLayout = VK_NULL_HANDLE;

			return *this;
		}
	}
}
