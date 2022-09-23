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
		RenderInstance(RenderInstance const&) = delete;
		RenderInstance(RenderInstance&&) = delete;
		~RenderInstance();

		bool createDevice(Vk::Surface& surface);

		Vk::Instance& getHandle();
		Vk::Instance const& getHandle() const;
		RenderDevice& getDevice();
		RenderDevice const& getDevice() const;

		RenderInstance& operator=(RenderInstance const&) = delete;
		RenderInstance& operator=(RenderInstance&&) = delete;
	private:
		bool checkPhysicalDeviceProperties(Vk::PhysicalDevice& physicalDevice, Vk::Surface& surface, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);

		Vk::Instance m_instance;
		RenderDevice m_device;
	};
}

#endif