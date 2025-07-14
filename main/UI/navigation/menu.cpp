#include "menu.h"

#include "hardware/korryButton.h"
#include "rc.h"

namespace pizda {
	MenuItem::MenuItem() {
		setHeight(20);
	}

	MenuItemState MenuItem::getState() const {
		return state;
	}

	void MenuItem::setState(const MenuItemState state) {
		this->state = state;

		invalidate();
	}

	void MenuItem::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID || getState() == MenuItemState::normal)
			return;

		onKorryEvent(reinterpret_cast<KorryEvent*>(event));
	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		if (getState() != MenuItemState::normal) {
			renderer->renderFilledRectangle(bounds, &Theme::bg3);

			renderer->renderFilledRectangle(
				Bounds(
					bounds.getX(),
					bounds.getY(),
					2,
					bounds.getHeight()
				),
				&Theme::fg1
			);
		}
	}

	void MenuItem::onKorryEvent(KorryEvent* event) {

	}

	void TitleMenuItem::setTitle(const std::wstring_view value) {
		title = value;
	}

	std::wstring_view TitleMenuItem::getTitle() const {
		return title;
	}

	void TitleMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		renderer->renderString(
			Point(
				bounds.getXCenter() - Theme::fontNormal.getWidth(getTitle()) / 2,
				bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::fg4,
			getTitle()
		);
	}

	RouteMenuItem::RouteMenuItem(const std::wstring_view title, const Route* route) : _route(route) {
		setTitle(title);
	}

	void RouteMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			RC::getInstance().setRoute(_route);

			event->setHandled(true);
		}
	}

	FunctionMenuItem::FunctionMenuItem(const std::wstring_view title, const std::function<void()>& function) : _function(function) {
		setTitle(title);
	}

	void FunctionMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			_function();

			event->setHandled(true);
		}
	}

	BoolMenuItem::BoolMenuItem(const std::wstring_view title) {
		setTitle(title);
	}

	bool BoolMenuItem::getValue() const {
		return _value;
	}

	void BoolMenuItem::setValue(const bool value) {
		_value = value;

		onValueChanged();
		valueChanged();

		invalidate();
	}

	void BoolMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		TitleMenuItem::onRender(renderer, bounds);

		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX2() - 2 - 7,
				bounds.getYCenter() - 1,
				7,
				4
			),
			2,
			getValue() ? &Theme::green : &Theme::red
		);
	}

	void BoolMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getEventType() == KorryEventType::down && event->getButtonType() == KorryButtonType::middle) {
			setValue(!getValue());
		}
	}

	void BoolMenuItem::onValueChanged() {

	}

	Menu::Menu() {
		setWidth(120);
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

	MenuItem* Menu::getItemAt(const uint16_t index) const {
		return dynamic_cast<MenuItem*>(getChildAt(index));
	}

	uint16_t Menu::getSelectedIndex() const {
		return _selectedIndex;
	}

	MenuItem* Menu::getSelectedItem() const {
		return getItemAt(_selectedIndex);
	}

	void Menu::setSelectedIndex(const uint16_t value) {
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
				const auto selectedItem = getSelectedItem();

				if (selectedItem->getState() == MenuItemState::active)
					return;

				auto newIndex = static_cast<int16_t>(_selectedIndex + (korryEvent->getButtonType() == KorryButtonType::down ? 1 : -1));

				// Cycling
				if (newIndex < 0) {
					newIndex = static_cast<int16_t>(getChildrenCount() - 1);
				}
				else if (newIndex >= getChildrenCount()) {
					newIndex = 0;
				}

				setSelectedIndex(static_cast<uint16_t>(newIndex));

				event->setHandled(true);
			}
		}
	}

	void Menu::updateItemsFromSelection() const {
		for (uint16_t i = 0; i < static_cast<uint16_t>(getChildrenCount()); i++) {
			getItemAt(i)->setState(i == _selectedIndex ? MenuItemState::hovered : MenuItemState::normal);
		}
	}
}
