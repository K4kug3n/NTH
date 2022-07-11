#include "Renderer/Sampler.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"

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

		bool Sampler::create(Device const& device, VkSamplerCreateInfo const& info) {
			VkResult result{ device.vkCreateSampler(device(), &info, nullptr, &m_sampler) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create sampler, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		VkSampler Sampler::operator()() const {
			return m_sampler;
		}
	}
}
