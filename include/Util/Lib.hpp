#ifndef NTH_UTIL_LIB_HPP
#define NTH_UTIL_LIB_HPP

#include <filesystem>

#if defined _WIN32
	#include <Windows.h>

	using LibHandle = HMODULE;
#elif defined __unix__
	using LibHandle = void*;
#else
	#error "OS not supported"
#endif

namespace Nth {

	using LibFunction = void (*)(void);

	class Lib {
	public:
		Lib();
		Lib(std::filesystem::path const& path);
		~Lib();

		bool load(std::filesystem::path const& path);
		bool isValid() const;

		LibFunction getSymbol(std::string const& name) const;

		void free();

	private:
		LibHandle m_handle;
	};
		
}

#endif
