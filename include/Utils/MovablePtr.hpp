#ifndef NTH_UTILS_MOVABLEPTR_HPP
#define NTH_UTILS_MOVABLEPTR_HPP

namespace Nth {
	template<typename T>
	class MovablePtr {
	public:
		MovablePtr();
		MovablePtr(T* ptr);
		MovablePtr(MovablePtr<T>&& other);
		MovablePtr(const MovablePtr<T>& other) = delete;
		~MovablePtr() = default;
		
		T* get();
		const T* get() const;

		T& operator*();
		const T& operator*() const;
		operator bool() const;

		MovablePtr& operator=(const MovablePtr&) = delete;
		MovablePtr& operator=(MovablePtr&& other);
	private:
		T* m_handle;
	};
}

#include <Utils/MovablePtr.inl>

#endif