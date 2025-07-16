#include "menu.h"

#include "hardware/korryButton.h"
#include "rc.h"

namespace pizda {
	MenuItem::MenuItem() {
		setHeight(height);
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

	RouteMenuItem::RouteMenuItem(const std::wstring_view title, const Route* route) : route(route) {
		setTitle(title);
	}

	void RouteMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			RC::getInstance().setRoute(route);

			event->setHandled(true);
		}
	}

	FunctionMenuItem::FunctionMenuItem(const std::wstring_view title, const std::function<void()>& function) : function(function) {
		setTitle(title);
	}

	void FunctionMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			function();

			event->setHandled(true);
		}
	}

	BoolMenuItem::BoolMenuItem(const std::wstring_view title) {
		setTitle(title);
	}

	bool BoolMenuItem::getValue() const {
		return value;
	}

	void BoolMenuItem::setValue(const bool value) {
		this->value = value;

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

	void ListMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		TitleMenuItem::onRender(renderer, bounds);

		if (isSelected()) {
			renderer->renderFilledRectangle(
				Bounds(
					bounds.getX2() - 2 - 7,
					bounds.getYCenter() - 1,
					7,
					4
				),
				2,
				&Theme::green
			);
		}
	}

	void ListMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getEventType() == KorryEventType::down && event->getButtonType() == KorryButtonType::middle) {
			onSelectionRequested();
		}
	}

	Menu::Menu() {
		setScrollBarSize(3);
		setScrollBarCornerRadius(1);
		setScrollBarOffset(0);
		setScrollBarThumbColor(&Theme::bg3);
		setHorizontalScrollMode(ScrollMode::disabled);
		setVerticalScrollMode(ScrollMode::computed);

		itemsLayout.setMargin({ 8, 0, 8, 0 });
		itemsLayout.setSpacing(itemSpacing);
		*this += &itemsLayout;
	}

	void Menu::addItem(MenuItem* item) {
		itemsLayout += item;
	}

	void Menu::setItems(const std::initializer_list<MenuItem*>& items) {
		for (const auto item : items)
			addItem(item);

		setSelectedIndex(0);
	}

	uint16_t Menu::getItemsCount() const {
		return itemsLayout.getChildrenCount();
	}

	MenuItem* Menu::getItemAt(const uint16_t index) const {
		return dynamic_cast<MenuItem*>(itemsLayout.getChildAt(index));
	}

	uint16_t Menu::getSelectedIndex() const {
		return selectedIndex;
	}

	MenuItem* Menu::getSelectedItem() const {
		return getItemAt(selectedIndex);
	}

	void Menu::setSelectedIndex(const uint16_t value) {
		selectedIndex = value;
		updateItemsFromSelection();

		invalidate();
	}

	void Menu::onEventBeforeChildren(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (
			(korryEvent->getButtonType() == KorryButtonType::up || korryEvent->getButtonType() == KorryButtonType::down)
			&& (korryEvent->getEventType() == KorryEventType::down || korryEvent->getEventType() == KorryEventType::tick)
		) {
			if (getSelectedItem()->getState() == MenuItemState::active)
				return;

			auto newIndex = static_cast<int16_t>(selectedIndex + (korryEvent->getButtonType() == KorryButtonType::down ? 1 : -1));

			// Cycling
			if (newIndex < 0) {
				newIndex = static_cast<int16_t>(getItemsCount() - 1);
			}
			else if (newIndex >= getItemsCount()) {
				newIndex = 0;
			}

			setSelectedIndex(static_cast<uint16_t>(newIndex));
			scrollTo(getSelectedItem());

			event->setHandled(true);
		}
	}

	void Menu::updateItemsFromSelection() const {
		for (uint16_t i = 0; i < getItemsCount(); i++) {
			getItemAt(i)->setState(i == selectedIndex ? MenuItemState::hovered : MenuItemState::normal);
		}
	}
}
