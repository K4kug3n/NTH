#ifndef NTH_RENDERER_RENDERINSTANCE_HPP
#define NTH_RENDERER_RENDERINSTANCE_HPP

#include <Renderer/Vulkan/Instance.hpp>
#include <Renderer/Vulkan/Queue.hpp>

#include <Renderer/RenderDevice.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Nth {
	namespace Vk {
		class Surface;
	}

	// TODO: Clean this imple
	class RenderInstance {
	public:
		RenderInstance();
		RenderInstance(const RenderInstance&) = delete;
		RenderInstance(RenderInstance&&) = delete;
		~RenderInstance();

		void create_device(Vk::Surface& surface);

		Vk::Instance& get_handle();
		const Vk::Instance& get_handle() const;
		RenderDevice& get_device();
		const RenderDevice& get_device() const;

		RenderInstance& operator=(const RenderInstance&) = delete;
		RenderInstance& operator=(RenderInstance&&) = delete;
	private:
		bool check_physical_device_properties(Vk::PhysicalDevice& physicalDevice, Vk::Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);

		Vk::Instance m_instance;
		RenderDevice m_device;
	};
}

#endif