#ifndef NTH_RENDERER_RENDERDEVICE_HPP
#define NTH_RENDERER_RENDERDEVICE_HPP

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>


namespace Nth {
	namespace Vk {
		class Surface;
		class PhysicalDevice;
		class CommandBuffer;
	}

	class RenderInstance;

	class RenderDevice {
	public:
		RenderDevice(RenderInstance const& instance);
		RenderDevice() = delete;
		RenderDevice(RenderDevice const&) = delete;
		RenderDevice(RenderDevice&&) = delete;
		~RenderDevice();

		//TODO: Review this
		void create(Vk::PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex);

		Vk::CommandBuffer allocateCommandBuffer() const;

		Vk::Queue& presentQueue();
		Vk::Queue const& presentQueue() const;
		Vk::Queue& graphicsQueue();
		Vk::Queue const& graphicsQueue() const;
		Vk::Device& getHandle();
		Vk::Device const& getHandle() const;

		RenderDevice& operator=(RenderDevice const&) = delete;
		RenderDevice& operator=(RenderDevice&&) = delete;
	private:
		RenderInstance const& m_instance;
		Vk::Queue m_presentQueue;
		Vk::Queue m_graphicsQueue;

		Vk::CommandPool m_pool;

		Vk::Device m_device;
	};
}

#endif