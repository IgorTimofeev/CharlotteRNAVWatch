#include "menu.h"

#include "hardware/korryButton.h"
#include "rc.h"

namespace pizda {
	MenuItem::MenuItem() {
		setHeight(20);
	}

	void MenuItem::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID || !isActive())
			return;

		onKorryEvent(reinterpret_cast<KorryEvent*>(event));
	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		if (isActive())
			renderer->renderFilledRectangle(bounds, 3, &Theme::fg1);

		renderer->renderString(
			Point(
				bounds.getXCenter() - Theme::fontNormal.getWidth(getText()) / 2,
				bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			isActive() ? &Theme::bg1 : &Theme::fg5,
			getText()
		);
	}

	void MenuItem::onKorryEvent(KorryEvent* event) {

	}

	RouteMenuItem::RouteMenuItem(const std::wstring_view text, const Route* route) : _route(route) {
		setText(text);
	}

	void RouteMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			RC::getInstance().setRoute(_route);

			event->setHandled(true);
		}
	}

	WaypointItem::WaypointItem(const uint16_t waypointIndex): _waypointIndex(waypointIndex) {
		const auto& waypoint = RC::getInstance().settings.nav.waypoints[_waypointIndex];

		setText(waypoint.name);
	}

	void WaypointItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			RC::getInstance().setRoute(&Routes::waypoint, &_waypointIndex);

			event->setHandled(true);
		}
	}

	FunctionMenuItem::FunctionMenuItem(const std::wstring_view text, const std::function<void()>& function): _function(function) {
		setText(text);
	}

	void FunctionMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			_function();

			event->setHandled(true);
		}
	}

	Menu::Menu() {
		setWidth(100);
		setSpacing(5);
	}

	void Menu::setItems(const std::initializer_list<MenuItem*>& items) {
		for (const auto item : items) {
			addItem(item);
		}

		setSelectedIndex(0);
	}

	void Menu::addItem(MenuItem* item) {
		*this += item;
	}

	MenuItem* Menu::getItemAt(const int16_t index) const {
		return dynamic_cast<MenuItem*>(getChildAt(index));
	}

	int16_t Menu::getSelectedIndex() const {
		return _selectedIndex;
	}

	void Menu::setSelectedIndex(const int16_t value) {
		_selectedIndex = value;
		updateItemsFromSelection();

		invalidate();
	}

	void Menu::onEventBeforeChildren(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getEventType() == KorryEventType::down || korryEvent->getEventType() == KorryEventType::tick) {
			if (korryEvent->getButtonType() == KorryButtonType::up || korryEvent->getButtonType() == KorryButtonType::down) {
				if (getChildrenCount() == 0)
					return;

				auto newIndex = static_cast<int16_t>(_selectedIndex + (korryEvent->getButtonType() == KorryButtonType::down ? 1 : -1));

				if (newIndex < 0) {
					newIndex = static_cast<int16_t>(getChildrenCount() - 1);
				}
				else if (newIndex >= getChildrenCount()) {
					newIndex = 0;
				}

				setSelectedIndex(newIndex);

				event->setHandled(true);
			}
		}
	}

	void Menu::updateItemsFromSelection() const {
		for (int16_t i = 0; i < static_cast<int16_t>(getChildrenCount()); i++) {
			getItemAt(i)->setActive(i == _selectedIndex);
		}
	}
}
