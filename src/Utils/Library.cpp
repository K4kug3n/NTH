#include <Utils/Library.hpp>

#include <iostream>

#if defined __unix__
#include <dlfcn.h>
#endif

namespace Nth {
	Library::Library() :
		m_handle{ nullptr } {}

	Library::Library(const std::filesystem::path& path) {
		load(path);
	}

	Library::~Library() {
		free();
	}

	void Library::load(const std::filesystem::path& path) {
	#if defined(_WIN32)
			m_handle = LoadLibrary(path.generic_u8string().c_str());
	#elif defined(__unix__)
			m_handle = dlopen(path.c_str(), RTLD_NOW);
	#else
		#error "OS not supported"
	#endif

		if (!m_handle) {
			throw std::runtime_error("Error occured during loading " + path.string());
		}
	}

	bool Library::is_valid() const {
		return m_handle;
	}

	LibFunction Library::get_symbol(std::string_view name) const {
	#if defined _WIN32
		return reinterpret_cast<LibFunction>(GetProcAddress(m_handle, name.data()));
	#elif defined __unix__
		return reinterpret_cast<LibFunction>(dlsym(m_handle, name.data()));
	#else
		#error "OS not supported"
	#endif
	}

	void Library::free() {
		if (m_handle) {
		#if defined _WIN32
			FreeLibrary(m_handle);
		#elif defined __unix__
			dlclose(m_handle);
		#endif

			m_handle = nullptr;
		}
	}
}

