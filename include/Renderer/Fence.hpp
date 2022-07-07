#pragma once

#ifndef NTH_FENCE_HPP
#define NTH_FENCE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	class Device;

	class Fence {
	public:
		Fence();
		Fence(Fence const&) = delete;
		Fence(Fence&& object) noexcept;
		~Fence();

		bool create(Device const& device, VkFenceCreateFlags flags);
		bool wait(uint64_t timeout) const;
		bool reset() const;

		VkFence operator()() const;

		Fence& operator=(Fence const&) = delete;
		Fence& operator=(Fence&&) = delete;

	private:
		VkFence m_fence;
		Device const* m_device;
	};
}

#endif
