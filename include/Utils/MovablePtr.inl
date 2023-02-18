#include <Utils/MovablePtr.hpp>

#include <utility>
#include <cassert>

namespace Nth {
	template<typename T>
	inline MovablePtr<T>::MovablePtr() :
		m_handle(nullptr) { }

	template<typename T>
	inline MovablePtr<T>::MovablePtr(T* ptr) :
		m_handle(ptr) { }

	template<typename T>
	inline MovablePtr<T>::MovablePtr(MovablePtr<T>&& other) :
		m_handle(nullptr) {
		std::swap(m_handle, other.m_handle)
	}

	template<typename T>
	inline T* MovablePtr<T>::get() {
		return m_handle;
	}

	template<typename T>
	inline const T* MovablePtr<T>::get() const {
		return m_handle;
	}

	template<typename T>
	inline T& MovablePtr<T>::operator*() {
		assert(m_handle != nullptr)
		return *m_handle;
	}

	template<typename T>
	inline const T& MovablePtr<T>::operator*() const {
		assert(m_handle != nullptr)
		return *m_handle;
	}

	template<typename T>
	inline MovablePtr<T>::operator bool() const {
		return m_handle != nullptr;
	}

	template<typename T>
	inline MovablePtr<T>& MovablePtr<T>::operator=(MovablePtr&& other) {
		std::swap(m_handle, other.m_handle)

		return *this;
	}
}