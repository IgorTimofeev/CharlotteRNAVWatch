#include "menu.h"

#include "hardware/korryButton.h"
#include "rc.h"
#include "resources/images.h"

namespace pizda {
	MenuItem::MenuItem() {
		setHeight(height);
	}

	void MenuItem::setTitle(const std::wstring_view value) {
		title = value;
	}

	std::wstring_view MenuItem::getTitle() const {
		return title;
	}

	const Color* MenuItem::getDefaultTitleColor() const {
		return defaultTitleColor;
	}

	void MenuItem::setDefaultTitleColor(const Color* value) {
		this->defaultTitleColor = value;
	}

	const Color* MenuItem::getActiveTitleColor() const {
		return activeTitleColor;
	}

	void MenuItem::setActiveTitleColor(const Color* value) {
		this->activeTitleColor = value;
	}

	void MenuItem::setSecondaryColors() {
		setDefaultTitleColor(&Theme::fg7);
	}

	const Color* MenuItem::getActiveBackgroundColor() const {
		return activeBackgroundColor;
	}

	void MenuItem::setActiveBackgroundColor(const Color* value) {
		this->activeBackgroundColor = value;
	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		renderSelectionBackground(renderer, bounds);
		renderTitleOnCenter(renderer, bounds);
	}

	void MenuItem::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID || !isActive())
			return;

		onKorryEvent(reinterpret_cast<KorryEvent*>(event));
	}

	void MenuItem::renderSelectionBackground(Renderer* renderer, const Bounds& bounds) const {
		if (isActive()) {
			renderer->renderFilledRectangle(bounds, 2, getActiveBackgroundColor());
		}
	}

	void MenuItem::renderTitleOnCenter(Renderer* renderer, const Bounds& bounds) const {
		renderer->renderString(
			Point(
				bounds.getXCenter() - Theme::fontNormal.getWidth(getTitle()) / 2,
				bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			isActive() ? getActiveTitleColor() : getDefaultTitleColor(),
			getTitle()
		);
	}

	void MenuItem::renderSideIndicator(Renderer* renderer, const Bounds& bounds, const Color* color) {
		constexpr static uint8_t radius = 2;

		renderer->renderFilledCircle(
			Point(
				bounds.getX2() - radius / 2 - 10,
				bounds.getYCenter() - radius / 2
			),
			radius,
			color
		);
	}

	void MenuItem::onKorryEvent(KorryEvent* event) {

	}

	void RouteMenuItem::setRoute(const Route* route) {
		this->route = route;
	}

	void RouteMenuItem::setBackStyle() {
		setSecondaryColors();
		setTitle(L"Back");
	}

	void RouteMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			RC::getInstance().setRoute(route);

			event->setHandled(true);
		}
	}

	void FunctionMenuItem::setOnPress(const std::function<void()>& press) {
		this->press = press;
	}

	void FunctionMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			press();

			event->setHandled(true);
		}
	}

	void BoolMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		renderSelectionBackground(renderer, bounds);
		renderTitleOnCenter(renderer, bounds);
		renderSideIndicator(renderer, bounds, getValue() ? &Theme::good : &Theme::bg4);
	}

	void BoolMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getEventType() == KorryEventType::down && event->getButtonType() == KorryButtonType::middle) {
			setValue(!getValue());
		}
	}

	// List
	void ListMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		renderSelectionBackground(renderer, bounds);
		renderTitleOnCenter(renderer, bounds);

		if (isSelected())
			renderSideIndicator(renderer, bounds, &Theme::green);
	}

	void ListMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getEventType() == KorryEventType::down && event->getButtonType() == KorryButtonType::middle) {
			onSelectionRequested();
		}
	}

	void InputMenuItem::setOnInput(const std::function<void()>& callback) {
		input = callback;
	}

	void InputMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		renderSelectionBackground(renderer, bounds);

		const auto titleWidth = Theme::fontNormal.getWidth(getTitle());
		const auto textWidth = Theme::fontNormal.getWidth(getText());

		// Title
		auto pos = Point(
			bounds.getXCenter() - (titleWidth + textWidth) / 2,
			bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
		);

		renderer->renderString(
			pos,
			&Theme::fontNormal,
			isActive() ? getActiveTitleColor() : getDefaultTitleColor(),
			getTitle()
		);

		// Text
		pos.setX(pos.getX() + titleWidth);

		renderer->renderString(
			pos,
			&Theme::fontNormal,
			isActive() ? &Theme::bg6 : &Theme::fg6,
			getText()
		);
	}

	// Text
	void InputMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() != KorryButtonType::middle || event->getEventType() != KorryEventType::down)
			return;

		const auto keyboard = WatchKeyboard::show();

		keyboard->setText(getText());

		keyboard->setOnInputFinished([this](const std::wstring_view text) {
			setText(text);

			input();
		});

		onKeyboardShown(keyboard);

		event->setHandled(true);
	}

	// AZ
	void AZTextMenuItem::onKeyboardShown(WatchKeyboard* keyboard) {
		keyboard->setAZLayout();
	}

	// Int
	int32_t IntInputMenuItem::textToValue() const {
		return std::wcstol(getText().data(), nullptr, 0);
	}

	void IntInputMenuItem::onKeyboardShown(WatchKeyboard* keyboard) {
		keyboard->setIntLayout();
	}

	// Float
	float FloatInputMenuItem::textToValue() const {
		return std::wcstof(getText().data(), nullptr);
	}

	void FloatInputMenuItem::onKeyboardShown(WatchKeyboard* keyboard) {
		keyboard->setFloatLayout();
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
			getItemAt(i)->setActive(i == selectedIndex);
		}
	}
}
