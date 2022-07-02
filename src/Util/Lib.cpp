#include "Util/Lib.hpp"

#include "Util/String.hpp"

#include <iostream>

#if defined __unix__
	#include <dlfcn.h>
#endif

namespace Nth{
	Lib::Lib(): 
		m_handle{ nullptr } {}

	Lib::Lib(std::filesystem::path const& path){
		load(path);
	}

	Lib::~Lib(){
		this->free();
	}

	bool Lib::load(std::filesystem::path const& path){
		#if defined(_WIN32)
			m_handle = LoadLibrary(path.generic_u8string().c_str());
		#elif defined(__unix__)
			m_handle = dlopen(path.c_str(), RTLD_NOW);
		#else
			#error "OS not supported"
		#endif

		if (!m_handle) {
			std::cerr << "Error occured during loading " << path.string() << std::endl;
			return false;
		}

		return true;
	}

	bool Lib::isValid() const {
		return m_handle;
	}

	LibFunction Lib::getSymbol(std::string const& name) const {
		#if defined _WIN32
			return reinterpret_cast<LibFunction>(GetProcAddress(m_handle, name.data()));
		#elif defined __unix__
			return reinterpret_cast<LibFunction>(dlsym(m_handle, name.c_str()));
		#else
			#error "OS not supported"
		#endif
	}

	void Lib::free(){
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

