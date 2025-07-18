#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "hardware/korryButton.h"

namespace pizda {
	using namespace YOBA;

	class WatchKeyboard;

	class WatchKeyboardButton : public Control {
		public:
			explicit WatchKeyboardButton(wchar_t text);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;
			virtual void onPressed() = 0;

			WatchKeyboard* getKeyboard() const;

		public:
			wchar_t getText() const;

		private:
			wchar_t text;
	};

	class TextWatchKeyboardButton : public WatchKeyboardButton {
		public:
			explicit TextWatchKeyboardButton(wchar_t text);

			void onPressed() override;
	};

	class BackspaceWatchKeyboardButton : public WatchKeyboardButton {
		public:
			explicit BackspaceWatchKeyboardButton();

			void onPressed() override;
	};

	class EnterWatchKeyboardButton : public WatchKeyboardButton {
		public:
			explicit EnterWatchKeyboardButton();

			void onPressed() override;
	};

	class WatchKeyboardButtonsRow : public StackLayout {
		public:
			WatchKeyboardButtonsRow();

			void addButton(WatchKeyboardButton* button);
	};

	class WatchKeyboard : public RelativeStackLayout {
		friend class TextWatchKeyboardButton;
		friend class BackspaceWatchKeyboardButton;
		friend class EnterWatchKeyboardButton;

		public:
			explicit WatchKeyboard();
			~WatchKeyboard() override;

			std::wstring_view getText() const;
			void setText(const std::wstring_view& value);

			static WatchKeyboard* show();
			static void hide();

			void setAZLayout();
			void setIntLayout(bool allowNegative = true);
			void setFloatLayout(bool allowNegative = true);
			void setOnInputFinished(const std::function<void(const std::wstring_view&)>& value);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			static WatchKeyboard* instance;

			std::function<void(const std::wstring_view&)> onInputFinished {};

			DitheredRectangle ditheredRectangle {};

			Layout valueLayout {};
			Rectangle valueBackgroundRect {};
			Text valueText {};

			Layout buttonsLayout {};
			Rectangle buttonsBackgroundRect {};
			StackLayout buttonsRows {};

			WatchKeyboardButtonsRow* addButtonsRow();
			WatchKeyboardButtonsRow* addButtonsRow(const std::initializer_list<WatchKeyboardButton*>& buttons);
			WatchKeyboardButtonsRow* addButtonsRow(const std::initializer_list<wchar_t>& buttons);

			void focusLast() const;
			void onInput(wchar_t ch);
			void onBackspace();
			void onEnter() const;
	};
}
