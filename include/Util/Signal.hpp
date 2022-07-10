#ifndef NTH_UTIL_SIGNAL_HPP
#define NTH_UTIL_SIGNAL_HPP

#include <functional>
#include <memory>
#include <vector>

namespace Nth {
	template<typename... Args>
	class Signal {
	public:
		using Callback = std::function<void(Args...)>;

		Signal() = default;
		~Signal() = default;

		void connect(Callback const& function);
		void connect(Callback&& function);

		void clear();

		void operator()(Args... args);

	private:

		struct Slot {
			Callback callback;
		};

		std::vector<Slot> m_slots;
	};
}

#include "Signal.inl"

#endif
