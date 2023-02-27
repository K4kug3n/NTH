#ifndef NTH_RENDERER_RENDERDEVICE_HPP
#define NTH_RENDERER_RENDERDEVICE_HPP

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>


namespace Nth {
	namespace Vk {
		class PhysicalDevice;
		class CommandBuffer;
	}

	class RenderInstance;

	class RenderDevice {
	public:
		RenderDevice(const RenderInstance& instance);
		RenderDevice() = delete;
		RenderDevice(const RenderDevice&) = delete;
		RenderDevice(RenderDevice&&) = delete;
		~RenderDevice();

		//TODO: Review this
		void create(Vk::PhysicalDevice physicalDevice, const VkDeviceCreateInfo& infos, uint32_t present_queue_family_index, uint32_t graphic_queue_family_index);

		Vk::CommandBuffer allocate_command_buffer() const;

		Vk::Queue& present_queue();
		const Vk::Queue& present_queue() const;
		Vk::Queue& graphics_queue();
		const Vk::Queue& graphics_queue() const;
		Vk::Device& get_handle();
		const Vk::Device& get_handle() const;

		RenderDevice& operator=(const RenderDevice&) = delete;
		RenderDevice& operator=(RenderDevice&&) = delete;
	private:
		const RenderInstance& m_instance;
		Vk::Queue m_present_queue;
		Vk::Queue m_graphics_queue;

		Vk::CommandPool m_pool;

		Vk::Device m_device;
	};
}

#endif