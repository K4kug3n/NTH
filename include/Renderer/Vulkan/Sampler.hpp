#ifndef NTH_RENDERER_VK_SAMPLER_HPP
#define NTH_RENDERER_VK_SAMPLER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Sampler {
		public:
			Sampler();
			Sampler(Sampler const&) = delete;
			Sampler(Sampler&& object) noexcept;
			~Sampler();

			bool create(Device const& device, VkSamplerCreateInfo const& info);

			VkSampler operator()() const;

			Sampler& operator=(Sampler const&) = delete;
			Sampler& operator=(Sampler&&) = delete;

		private:
			VkSampler m_sampler;
			Device const* m_device;
		};
	}
}

#endif
