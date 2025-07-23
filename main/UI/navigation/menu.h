#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "hardware/korryButton.h"
#include "UI/navigation/route.h"
#include "types/settingsNav.h"

#include "UI/elements/watchKeyboard.h"

#include "UI/theme.h"

namespace pizda {
	using namespace YOBA;

	class MenuItem : public Control, public ActiveElement {
		public:
			MenuItem();

			constexpr static uint8_t height = 20;
			constexpr static uint8_t textMargin = 10;

			void setTitle(std::wstring_view value);
			std::wstring_view getTitle() const;

			const Color* getActiveBackgroundColor() const;
			void setActiveBackgroundColor(const Color* value);

			const Color* getDefaultTitleColor() const;
			void setDefaultTitleColor(const Color* value);

			const Color* getActiveTitleColor() const;
			void setActiveTitleColor(const Color* value);

			void setSecondaryColors();

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;
			void renderSelectionBackground(Renderer* renderer, const Bounds& bounds) const;
			void renderTitleOnCenter(Renderer* renderer, const Bounds& bounds) const;
			static void renderSideIndicator(Renderer* renderer, const Bounds& bounds, const Color* color);
			virtual void onKorryEvent(KorryEvent* event);

		private:
			std::wstring title {};

			const Color* activeBackgroundColor = &Theme::fg1;

			const Color* defaultTitleColor = &Theme::fg3;
			const Color* activeTitleColor = &Theme::bg1;
	};

	template<typename T>
	class ValueMenuItem : public virtual MenuItem {
		public:
			T getValue() const;
			void setValue(T value);
			void setOnValueChanged(const std::function<void()>& callback);

		private:
			T value;
			std::function<void()> valueChanged = nullptr;
	};

	template<typename T>
	T ValueMenuItem<T>::getValue() const {
		return value;
	}

	template<typename T>
	void ValueMenuItem<T>::setValue(T value) {
		if (value == this->value)
			return;

		this->value = value;

		if (valueChanged)
			valueChanged();

		invalidate();
	}

	template<typename T>
	void ValueMenuItem<T>::setOnValueChanged(const std::function<void()>& callback) {
		valueChanged = callback;
	}

	class RouteMenuItem : public MenuItem {
		public:
			void setRoute(const Route* route);
			void setBackStyle();

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			const Route* route = nullptr;
	};

	class FunctionMenuItem : public MenuItem {
		public:
			void setOnPress(const std::function<void()>& press);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			std::function<void()> press = nullptr;
	};

	class BoolMenuItem : public ValueMenuItem<bool> {
		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;
	};

	class ListMenuItem : public MenuItem {
		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;

			virtual bool isSelected() const = 0;
			virtual void onSelectionRequested() = 0;
	};

	class InputMenuItem : public virtual MenuItem, public TextElement {
		public:
			void setOnInput(const std::function<void()>& callback);
			std::wstring getSuffix() const;
			void setSuffix(const std::wstring& value);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;
			virtual void onKeyboardShown(WatchKeyboard* keyboard) = 0;

		private:
			std::wstring suffix = L"";
			std::function<void()> input = nullptr;
	};

	class AZTextMenuItem : public InputMenuItem {
		protected:
			void onKeyboardShown(WatchKeyboard* keyboard) override;
	};

	template<typename T>
	class ValueInputMenuItem : public InputMenuItem {
		public:
			T getValue();
			void setValue(T value);

		protected:
			virtual T textToValue() const = 0;
	};

	template <typename T>
	T ValueInputMenuItem<T>::getValue() {
		return textToValue();
	}

	template <typename T>
	void ValueInputMenuItem<T>::setValue(T value) {
		this->setText(std::to_wstring(value));
	}

	template <typename T>
	class NumericValueInputMenuItem : public ValueInputMenuItem<T> {
		public:
			bool getAllowNegative() const;
			void setAllowNegative(bool value);

		private:
			bool allowNegative = false;
	};

	template <typename T>
	bool NumericValueInputMenuItem<T>::getAllowNegative() const {
		return allowNegative;
	}

	template <typename T>
	void NumericValueInputMenuItem<T>::setAllowNegative(bool value) {
		this->allowNegative = value;
	}

	class IntInputMenuItem : public NumericValueInputMenuItem<int32_t> {
		protected:
			int32_t textToValue() const override;
			void onKeyboardShown(WatchKeyboard* keyboard) override;
	};

	class FloatInputMenuItem : public NumericValueInputMenuItem<float> {
		protected:
			float textToValue() const override;
			void onKeyboardShown(WatchKeyboard* keyboard) override;
	};

	class Menu : public ScrollView {
		public:
			explicit Menu();

			constexpr static uint8_t itemSpacing = 5;

			uint16_t getItemsCount() const;
			void addItem(MenuItem* item);
			MenuItem* getItemAt(uint16_t index) const;
			uint16_t getSelectedIndex() const;
			MenuItem* getSelectedItem() const;
			void setSelectedIndex(uint16_t value);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			uint16_t selectedIndex = 0;
			StackLayout itemsLayout {};

			void updateItemsFromSelection() const;
	};
}