#ifndef NTH_RENDERER_VK_SAMPLER_HPP
#define NTH_RENDERER_VK_SAMPLER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Sampler {
		public:
			Sampler();
			Sampler(const Sampler&) = delete;
			Sampler(Sampler&& object) noexcept;
			~Sampler();

			void create(const Device& device, const VkSamplerCreateInfo& info);

			VkSampler operator()() const;

			Sampler& operator=(const Sampler&) = delete;
			Sampler& operator=(Sampler&&) = delete;

		private:
			VkSampler m_sampler;
			Device const* m_device;
		};
	}
}

#endif
