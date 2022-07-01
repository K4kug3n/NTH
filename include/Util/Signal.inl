namespace Nth {
	template<typename ...Args>
	inline void Signal<Args...>::connect(Callback const& function)
	{
		Connect(Callback(function));
	}

	template<typename ...Args>
	inline void Signal<Args...>::connect(Callback&& function)
	{
		m_slots.emplace_back(Slot{ function });
	}

	template<typename ...Args>
	inline void Signal<Args...>::clear()
	{
		m_slots.clear();
	}

	template<typename ...Args>
	inline void Signal<Args...>::operator()(Args ...args)
	{
		for (size_t i{ 0 }; i < m_slots.size(); ++i) {
			m_slots[i].callback(args...);
		}
	}

}
