#include <Renderer/Vulkan/Surface.hpp>

#include <Renderer/Vulkan/VkUtils.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Window/WindowHandle.hpp>

#include <vector>
#include <iostream>

namespace Nth {
	namespace Vk {
		Surface::Surface(const Instance& instance) :
			m_surface{ VK_NULL_HANDLE },
			m_instance{ instance } { }

		Surface::~Surface() {
			if (m_surface != VK_NULL_HANDLE) {
				m_instance.vkDestroySurfaceKHR(m_instance(), m_surface, nullptr);
			}
		}

		void Surface::create(const WindowHandle& infos) {
		#if defined(VK_USE_PLATFORM_WIN32_KHR)
			if (infos.subsystem != WindowProtocol::Windows) {
				throw std::runtime_error("System detected by Vulkan and for Window management are different");
			}

			VkWin32SurfaceCreateInfoKHR surface_create_info = {
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                  sType
				nullptr,                                          // const void                      *pNext
				0,                                                // VkWin32SurfaceCreateFlagsKHR     flags
				static_cast<HINSTANCE>(infos.windows.hinstance),  // HINSTANCE                        hinstance
				static_cast<HWND>(infos.windows.hwnd)             // HWND                             hwnd
			};

			VkResult result{ m_instance.vkCreateWin32SurfaceKHR(m_instance(), &surface_create_info, nullptr, &m_surface) };
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Can't create surface, " + to_string(result));
			}

		#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			if (infos.subsystem != WindowProtocol::X11) {
				throw std::runtime_error("System detected by Vulkan and for Window management are different");
			}

			VkXlibSurfaceCreateInfoKHR surface_create_info = {
				VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                sType
				nullptr,                                          // const void                    *pNext
				0,                                                // VkXlibSurfaceCreateFlagsKHR    flags
				static_cast<Display*>(infos.x11.dpy),             // Display                       *dpy
				static_cast<Window>(infos.x11.window)             // Window                         window
			};

			VkResult result{ m_instance.vkCreateXlibSurfaceKHR(m_instance(), &surface_create_info, nullptr, &m_surface) };
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Can't create surface, " + to_string(result));
			}
		#else
			#error "Platform not supported"
		#endif
		}

		bool Surface::get_physical_device_surface_support(const PhysicalDevice& device, uint32_t queueFamilyIndex) const {
			VkBool32 presentationSupported{ false };
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceSupportKHR(device(), queueFamilyIndex, m_surface, &presentationSupported) };
			if (result != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Can't query if presentation is supported, " + to_string(result));
			}

			return (presentationSupported == VK_TRUE);
		}

		VkSurfaceCapabilitiesKHR Surface::get_capabilities(const PhysicalDevice& physicalDevice) const {
			VkSurfaceCapabilitiesKHR capabilities;
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice(), m_surface, &capabilities) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not check presentation surface capabilities, " + to_string(result));
			}

			return capabilities;
		}

		std::vector<VkSurfaceFormatKHR> Surface::get_formats(const PhysicalDevice& physicalDevice) const {
			uint32_t formatsCount;
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice(), m_surface, &formatsCount, nullptr) };
			if ((result != VK_SUCCESS) || (formatsCount == 0)) {
				throw std::runtime_error("Could not get surface format count, " + to_string(result));
			}

			std::vector<VkSurfaceFormatKHR> formats(formatsCount);
			result = m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice(), m_surface, &formatsCount, formats.data());
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not get surface format, " + to_string(result));
			}

			return formats;
		}

		std::vector<VkPresentModeKHR> Surface::get_present_modes(const PhysicalDevice& physicalDevice) const {
			uint32_t presentModesCount;
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice(), m_surface, &presentModesCount, nullptr) };
			if ((result != VK_SUCCESS) || (presentModesCount == 0)) {
				throw std::runtime_error("Could not get present modes count, " + to_string(result));
			}

			std::vector<VkPresentModeKHR> presentModes(presentModesCount);
			result = m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice(), m_surface, &presentModesCount, presentModes.data());
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not get present modes, " + to_string(result));
			}

			return presentModes;
		}

		bool Surface::is_valid() const {
			return m_surface != VK_NULL_HANDLE;
		}

		VkSurfaceKHR Surface::operator()() const {
			return m_surface;
		}
	}
}