#include "Renderer/Surface.hpp"

#include "Renderer/VkUtil.hpp"
#include "Renderer/PhysicalDevice.hpp"
#include "Window/WindowHandle.hpp"

#include <vector>
#include <iostream>

namespace Nth {
	namespace Vk {
		Surface::Surface(Instance const& instance) :
			m_surface{ VK_NULL_HANDLE },
			m_instance{ instance } { }

		Surface::~Surface() {
			if (m_surface != VK_NULL_HANDLE) {
				m_instance.vkDestroySurfaceKHR(m_instance(), m_surface, nullptr);
			}
		}

		bool Surface::create(WindowHandle const& infos) {
		#if defined(VK_USE_PLATFORM_WIN32_KHR)

			if (infos.protocol != WindowProtocol::Windows) {
				std::cerr << "Error : System detected by Vulkan and for Window management are different" << std::endl;
				return false;
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
				std::cerr << "Error : Can't create surface: " << toString(result) << std::endl;
				return false;
			}

			return true;

		#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			if (infos.protocol != WindowProtocol::X11) {
				std::cerr << "Error : System detected by Vulkan and for Window management are different" << std::endl;
				return false;
			}

			VkXlibSurfaceCreateInfoKHR surface_create_info = {
				VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                sType
				nullptr,                                          // const void                    *pNext
				0,                                                // VkXlibSurfaceCreateFlagsKHR    flags
				static_cast<Display>(infos.x11.dpy),              // Display                       *dpy
				static_cast<Window>(infos.x11.window)             // Window                         window
			};

			VkResult result{ instance.vkCreateXlibSurfaceKHR(m_instance(), &surface_create_info, nullptr, &m_surface) };
			if (result != VkResult::VK_SUCCESS) {
				std::cerr << "Error : Can't create surface: " << toString(result) << std::endl;
				return false;
			}

			return true;
		#else
			return false;
		#endif
		}

		bool Surface::getPhysicalDeviceSurfaceSupport(PhysicalDevice const& device, uint32_t queueFamilyIndex, bool& supported) const {
			VkBool32 presentationSupported{ false };
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceSupportKHR(device(), queueFamilyIndex, m_surface, &presentationSupported) };
			if (result != VkResult::VK_SUCCESS) {
				std::cerr << "Error : Can't query if presentation is supported, " + toString(result) << std::endl;
				return false;
			}

			supported = (presentationSupported == VK_TRUE);

			return true;
		}

		bool Surface::getCapabilities(PhysicalDevice const& physicalDevice, VkSurfaceCapabilitiesKHR& capabilities) const {
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice(), m_surface, &capabilities) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not check presentation surface capabilities, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		bool Surface::getFormats(PhysicalDevice const& physicalDevice, std::vector<VkSurfaceFormatKHR>& formats) const {
			uint32_t formatsCount;
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice(), m_surface, &formatsCount, nullptr) };
			if ((result != VK_SUCCESS) || (formatsCount == 0)) {
				std::cerr << "Error: Could not get surface format count, " << toString(result) << std::endl;
				return false;
			}

			formats = std::vector<VkSurfaceFormatKHR>(formatsCount);
			result = m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice(), m_surface, &formatsCount, formats.data());
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not get surface format, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		bool Surface::getPresentModes(PhysicalDevice const& physicalDevice, std::vector<VkPresentModeKHR>& presentModes) const {
			uint32_t presentModesCount;
			VkResult result{ m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice(), m_surface, &presentModesCount, nullptr) };
			if ((result != VK_SUCCESS) || (presentModesCount == 0)) {
				std::cerr << "Error: Could not get present modes count, " << toString(result) << std::endl;
				return false;
			}

			presentModes = std::vector<VkPresentModeKHR>(presentModesCount);
			result = m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice(), m_surface, &presentModesCount, presentModes.data());
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not get present modes, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		bool Surface::isValid() const {
			return m_surface != VK_NULL_HANDLE;
		}

		VkSurfaceKHR const& Surface::operator()() const {
			return m_surface;
		}
	}
}