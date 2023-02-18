#include <Renderer/Vulkan/DescriptorSetLayout.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		DescriptorSetLayout::DescriptorSetLayout() :
			m_descriptor_set_layout(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& object) noexcept :
			m_descriptor_set_layout(object.m_descriptor_set_layout),
			m_device(object.m_device) {
			object.m_descriptor_set_layout = VK_NULL_HANDLE;
		}

		DescriptorSetLayout::~DescriptorSetLayout() {
			destroy();
		}

		void DescriptorSetLayout::create(const Device& device, const VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_info) {
			VkResult result{ device.vkCreateDescriptorSetLayout(device(), &descriptor_set_layout_info, nullptr, &m_descriptor_set_layout) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create Descriptor Set Layout, " + to_string(result));
			}

			m_device = &device;
		}

		void DescriptorSetLayout::destroy() {
			if (m_descriptor_set_layout != VK_NULL_HANDLE) {
				m_device->vkDestroyDescriptorSetLayout((*m_device)(), m_descriptor_set_layout, nullptr);
				m_descriptor_set_layout = VK_NULL_HANDLE;
			}
		}

		VkDescriptorSetLayout DescriptorSetLayout::operator()() const {
			return m_descriptor_set_layout;
		}

		DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& object) noexcept {
			destroy();

			m_descriptor_set_layout = object.m_descriptor_set_layout;
			m_device = object.m_device;

			object.m_descriptor_set_layout = VK_NULL_HANDLE;

			return *this;
		}
	}
}
