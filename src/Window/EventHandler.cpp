#include <Window/EventHandler.hpp>

namespace Nth {

	void EventHandler::dispatch(WindowEvent const& event) {
		switch (event.type) {
		case WindowEventType::Quit:
			onQuit();
			break;
		case WindowEventType::Resized:
			onResize();
			break;
		case WindowEventType::KeyDown:
			onKeyDown(event.key);
			break;
		case WindowEventType::KeyUp:
			onKeyUp(event.key);
			break;
		case WindowEventType::MouseMove:
			onMouseMove(event.x, event.y, event.xrel, event.yrel);
			break;
		}
	}

}

