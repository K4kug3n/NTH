set_project("NTH")
add_requires("vulkan-memory-allocator", "vulkan-headers", "libsdl", "tinyobjloader", "stb", "catch2", "assimp")
add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

add_includedirs("include")
add_headerfiles("include/**.hpp", "include/**.inl")
add_files("src/**.cpp")

set_languages("cxx17")
set_warnings("allextra")

add_defines("VK_NO_PROTOTYPES")

if is_plat("linux") then
	add_defines("NTH_UNIX", "VK_USE_PLATFORM_XLIB_KHR")
end

if is_plat("windows") then
	add_defines("NTH_WINDOWS", "VK_USE_PLATFORM_WIN32_KHR")
end

target("basic")
	add_files("exemples/basic/main.cpp")

	set_rundir("assets")

	add_packages("vulkan-memory-allocator", "vulkan-headers", "libsdl", "tinyobjloader", "stb", "assimp")


target("tests")
	add_files("tests/**.cpp")

	add_packages("catch2", "vulkan-memory-allocator", "vulkan-headers", "libsdl", "tinyobjloader", "stb", "assimp")