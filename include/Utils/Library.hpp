#ifndef NTH_UTILS_LIBRARY_HPP
#define NTH_UTILS_LIBRARY_HPP

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

	class Library {
	public:
		Library();
		Library(const std::filesystem::path& path);
		Library(Library&&) = delete;
		Library(const Library&) = delete;
		~Library();

		void free();

		LibFunction get_symbol(std::string_view name) const;

		bool is_valid() const;
		void load(const std::filesystem::path& path);
		
		Library& operator=(Library&&) = delete;
		Library& operator=(const Library&) = delete;
	private:
		LibHandle m_handle;
	};

}

#endif
