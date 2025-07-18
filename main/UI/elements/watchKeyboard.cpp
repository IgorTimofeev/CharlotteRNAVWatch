#include "watchKeyboard.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	WatchKeyboardButton::WatchKeyboardButton(const wchar_t text) : text(text) {
		setSize({20, 20});
	}

	void WatchKeyboardButton::onRender(Renderer* renderer, const Bounds& bounds) {
		if (isFocused()) {
			renderer->renderFilledRectangle(
				bounds,
				2,
				&Theme::fg1
			);
		}

		renderer->renderChar(
			Point(
				bounds.getXCenter() - Theme::fontBigThin.getWidth(text) / 2,
				bounds.getYCenter() - Theme::fontBigThin.getHeight() / 2
			),
			&Theme::fontBigThin,
			isFocused() ? &Theme::bg1 : &Theme::fg1,
			text
		);
	}

	void WatchKeyboardButton::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (!isFocused() || korryEvent->getButtonType() != KorryButtonType::middle || korryEvent->getEventType() != KorryEventType::down)
			return;

		onPressed();

		event->setHandled(true);
	}

	WatchKeyboard* WatchKeyboardButton::getKeyboard() const {
		return dynamic_cast<WatchKeyboard*>(dynamic_cast<StackLayout*>(dynamic_cast<WatchKeyboardButtonsRow*>(getParent())->getParent())->getParent()->getParent());
	}

	wchar_t WatchKeyboardButton::getText() const {
		return text;
	}

	TextWatchKeyboardButton::TextWatchKeyboardButton(const wchar_t text) : WatchKeyboardButton(text) {

	}

	void TextWatchKeyboardButton::onPressed() {
		getKeyboard()->onInput(getText());
	}

	BackspaceWatchKeyboardButton::BackspaceWatchKeyboardButton() : WatchKeyboardButton(L'<') {

	}

	void BackspaceWatchKeyboardButton::onPressed() {
		getKeyboard()->onBackspace();
	}

	EnterWatchKeyboardButton::EnterWatchKeyboardButton() : WatchKeyboardButton(L'>') {

	}

	void EnterWatchKeyboardButton::onPressed() {
		getKeyboard()->onEnter();
	}

	WatchKeyboardButtonsRow::WatchKeyboardButtonsRow() {
		setOrientation(Orientation::horizontal);
		setHorizontalAlignment(Alignment::center);
	}

	void WatchKeyboardButtonsRow::addButton(WatchKeyboardButton* button) {
		*this += button;
	}

	WatchKeyboard::WatchKeyboard() {
		ditheredRectangle.setFillColor(&Theme::bg1);
		*this += &ditheredRectangle;

		// Value
		valueBackgroundRect.setFillColor(&Theme::bg3);
		valueLayout += &valueBackgroundRect;

		valueText.setHorizontalAlignment(Alignment::center);
		valueText.setMargin(Margin(0, 3));
		valueText.setFont(&Theme::fontBigThin);
		valueText.setTextColor(&Theme::fg1);
		valueLayout += &valueText;

		*this += &valueLayout;
		setAutoSize(&valueLayout, true);

		// Buttons
		buttonsBackgroundRect.setFillColor(&Theme::bg1);
		buttonsLayout += &buttonsBackgroundRect;

		buttonsLayout += &buttonsRows;

		*this += &buttonsLayout;

		setAutoSize(&buttonsLayout, true);
	}

	WatchKeyboard::~WatchKeyboard() {
		for (auto rowIndex = 0; rowIndex < buttonsRows.getChildrenCount(); rowIndex++) {
			const auto row = dynamic_cast<WatchKeyboardButtonsRow*>(buttonsRows[rowIndex]);

			for (auto buttonIndex = 0; buttonIndex < row->getChildrenCount(); buttonIndex++) {
				const auto button = (*row)[buttonIndex];

				delete button;
			}

			delete row;
		}
	}

	void WatchKeyboard::setAZLayout() {
		addButtonsRow({ L'Q', L'W', L'E', L'R', L'T', L'Y', L'U', L'I', L'O', L'P' });
		addButtonsRow({ L'A', L'S', L'D', L'F', L'G', L'H', L'J', L'K', L'L' });
		addButtonsRow({ L'Z', L'X', L'C', L'V', L'B', L'N', L'M' });

		addButtonsRow({
			new BackspaceWatchKeyboardButton(),
			new EnterWatchKeyboardButton()
		});

		buttonsRows.setMargin(Margin(0, 8, 0, 8));

		focusLast();
	}

	void WatchKeyboard::setIntLayout(const bool allowNegative) {
		addButtonsRow({ L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'0' });

		const auto row = addButtonsRow();

		if (allowNegative)
			row->addButton(new TextWatchKeyboardButton(L'-'));

		row->addButton(new BackspaceWatchKeyboardButton());
		row->addButton(new EnterWatchKeyboardButton());

		buttonsRows.setMargin(Margin(0, 8, 0, 20));

		focusLast();
	}

	void WatchKeyboard::setFloatLayout(const bool allowNegative) {
		addButtonsRow({ L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'0' });

		const auto row = addButtonsRow();

		row->addButton(new TextWatchKeyboardButton(L'-'));

		if (allowNegative)
			row->addButton(new TextWatchKeyboardButton(L'-'));

		row->addButton(new BackspaceWatchKeyboardButton());
		row->addButton(new EnterWatchKeyboardButton());

		buttonsRows.setMargin(Margin(0, 8, 0, 20));

		focusLast();
	}

	void WatchKeyboard::setOnInputFinished(const std::function<void(const std::wstring_view&)>& value) {
		onInputFinished = value;
	}

	std::wstring_view WatchKeyboard::getText() const {
		return valueText.getText();
	}

	void WatchKeyboard::setText(const std::wstring_view& value) {
		valueText.setText(value);
	}

	WatchKeyboard* WatchKeyboard::instance = nullptr;

	WatchKeyboard* WatchKeyboard::show() {
		if (!instance) {
			auto& rc = RC::getInstance();

			instance = new WatchKeyboard();
			rc.application += instance;
		}

		return instance;
	}

	void WatchKeyboard::hide() {
		if (!instance)
			return;

		auto& rc = RC::getInstance();

		rc.application -= instance;
		delete instance;

		instance = nullptr;
	}

	void WatchKeyboard::onEventBeforeChildren(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (
			(korryEvent->getButtonType() == KorryButtonType::up || korryEvent->getButtonType() == KorryButtonType::down)
			&& (korryEvent->getEventType() == KorryEventType::down || korryEvent->getEventType() == KorryEventType::tick)
		) {
			for (auto rowIndex = 0; rowIndex < buttonsRows.getChildrenCount(); rowIndex++) {
				const auto row = dynamic_cast<WatchKeyboardButtonsRow*>(buttonsRows[rowIndex]);

				for (auto buttonIndex = 0; buttonIndex < row->getChildrenCount(); buttonIndex++) {
					const auto button = dynamic_cast<WatchKeyboardButton*>((*row)[buttonIndex]);

					if (button->isFocused()) {
						const auto newIndex = buttonIndex + (korryEvent->getButtonType() == KorryButtonType::down ? 1 : -1);

						if (newIndex < 0) {
							const auto prevRow = dynamic_cast<WatchKeyboardButtonsRow*>(buttonsRows[rowIndex > 0 ? rowIndex - 1 : buttonsRows.getChildrenCount() - 1]);
							const auto prevButton = dynamic_cast<WatchKeyboardButton*>((*prevRow)[prevRow->getChildrenCount() - 1]);
							prevButton->setFocused(true);
						}
						else if (newIndex < row->getChildrenCount()) {
							const auto nextButton = dynamic_cast<WatchKeyboardButton*>((*row)[newIndex]);
							nextButton->setFocused(true);
						}
						else {
							const auto nextRow = dynamic_cast<WatchKeyboardButtonsRow*>(buttonsRows[rowIndex < buttonsRows.getChildrenCount() - 1 ? rowIndex + 1 : 0]);
							const auto nextButton = dynamic_cast<WatchKeyboardButton*>((*nextRow)[0]);
							nextButton->setFocused(true);
						}

						goto break2;
					}
				}
			}

			break2:;
			event->setHandled(true);
		}
	}

	WatchKeyboardButtonsRow* WatchKeyboard::addButtonsRow() {
		const auto row = new WatchKeyboardButtonsRow();
		buttonsRows += row;

		return row;
	}

	WatchKeyboardButtonsRow* WatchKeyboard::addButtonsRow(const std::initializer_list<WatchKeyboardButton*>& buttons) {
		const auto row = addButtonsRow();

		for (const auto button : buttons)
			row->addButton(button);

		return row;
	}

	WatchKeyboardButtonsRow* WatchKeyboard::addButtonsRow(const std::initializer_list<wchar_t>& buttons) {
		const auto row = addButtonsRow();

		for (const auto button : buttons)
			row->addButton(new TextWatchKeyboardButton(button));

		return row;
	}

	void WatchKeyboard::focusLast() const {
		const auto row = dynamic_cast<WatchKeyboardButtonsRow*>(buttonsRows[buttonsRows.getChildrenCount() - 1]);
		const auto button = dynamic_cast<WatchKeyboardButton*>((*row)[row->getChildrenCount() - 1]);

		button->setFocused(true);
	}

	void WatchKeyboard::onInput(const wchar_t ch) {
		setText(std::wstring(getText()) + ch);
	}

	void WatchKeyboard::onBackspace() {
		if (getText().empty())
			return;

		setText(std::wstring(getText()).substr(0, getText().length() - 1));
	}

	void WatchKeyboard::onEnter() const {
		onInputFinished(getText());

		hide();
	}
}
