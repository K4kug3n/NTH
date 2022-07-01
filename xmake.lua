set_project("NTH")
add_requires("vulkan-memory-allocator", "vulkan-headers", "libsdl", "tinyobjloader", "stb", "catch2")
add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

add_includedirs("include")
add_headerfiles("include/**.hpp", "include/**.inl")
add_files("src/**.cpp")

set_languages("cxx17")
set_warnings("allextra")

target("basic_render")
	add_defines("VK_NO_PROTOTYPES")

	add_files("exemples/basic_render/main.cpp")

	add_packages("vulkan-memory-allocator", "vulkan-headers", "libsdl", "tinyobjloader", "stb")


target("test")
	add_files("tests/**.cpp")

	add_packages("catch2")