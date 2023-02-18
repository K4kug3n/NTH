#include <Renderer/Vulkan/Sampler.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		Sampler::Sampler() :
			m_sampler(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Sampler::Sampler(Sampler&& object) noexcept :
			m_sampler(object.m_sampler),
			m_device(object.m_device) {
			object.m_sampler = VK_NULL_HANDLE;
		}

		Sampler::~Sampler() {
			if (m_sampler != VK_NULL_HANDLE) {
				m_device->vkDestroySampler((*m_device)(), m_sampler, nullptr);
			}
		}

		void Sampler::create(const Device& device, const VkSamplerCreateInfo& info) {
			VkResult result{ device.vkCreateSampler(device(), &info, nullptr, &m_sampler) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create sampler, " + to_string(result));
			}

			m_device = &device;
		}

		VkSampler Sampler::operator()() const {
			return m_sampler;
		}
	}
}
