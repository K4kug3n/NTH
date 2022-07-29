#include "Renderer/RenderingResource.hpp"

#include "Renderer/Vulkan/CommandPool.hpp"
#include "Renderer/Vulkan/Device.hpp"

#include <iostream>

namespace Nth {
	bool RenderingResource::create(Vk::Device const& device, uint32_t index) {
		if (!commandPool.create(device, index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) {
			std::cerr << "Can't create command buffer pool" << std::endl;
			return false;
		}

		if (!commandPool.allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffer)) {
			std::cerr << "Can't allocate command buffer" << std::endl;
			return false;
		}

		if (!imageAvailableSemaphore.create(device)) {
			std::cerr << "Error: Can't create image available semaphore" << std::endl;
			return false;
		}

		if (!finishedRenderingSemaphore.create(device)) {
			std::cerr << "Error: Can't create rendering finished semaphore" << std::endl;
			return false;
		}

		if (!fence.create(device, VK_FENCE_CREATE_SIGNALED_BIT)) {
			std::cerr << "Could not create a fence!" << std::endl;
			return false;
		}

		return true;
	}
}
