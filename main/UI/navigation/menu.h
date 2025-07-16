#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "hardware/korryButton.h"
#include "UI/navigation/route.h"
#include "UI/theme.h"
#include "types/settingsNav.h"

namespace pizda {
	using namespace YOBA;

	enum class MenuItemState : uint8_t {
		normal,
		hovered,
		active
	};

	class MenuItem : public Control {
		public:
			MenuItem();
			MenuItemState getState() const;
			void setState(MenuItemState state);

			constexpr static uint8_t height = 20;

		protected:
			void onEvent(Event* event) override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			virtual void onKorryEvent(KorryEvent* event);

		private:
			MenuItemState state = MenuItemState::normal;
	};

	class TitleMenuItem : public MenuItem {
		public:
			void setTitle(std::wstring_view value);
			std::wstring_view getTitle() const;

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			std::wstring title {};
	};

	class RouteMenuItem : public TitleMenuItem {
		public:
			RouteMenuItem(std::wstring_view title, const Route* route);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			const Route* route;
	};

	class FunctionMenuItem : public TitleMenuItem {
		public:
			FunctionMenuItem(std::wstring_view title, const std::function<void()>& function);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			std::function<void()> function;
	};

	class BoolMenuItem : public TitleMenuItem {
		public:
			explicit BoolMenuItem(std::wstring_view title);

			Callback<> valueChanged {};

			bool getValue() const;
			void setValue(bool value);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;
			virtual void onValueChanged();

		private:
			bool value = false;
	};

	class ListMenuItem : public TitleMenuItem {
		public:

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;

			virtual bool isSelected() const = 0;
			virtual void onSelectionRequested() = 0;

		private:
	};

	class Menu : public ScrollView {
		public:
			explicit Menu();

			constexpr static uint8_t itemSpacing = 5;

			void setItems(const std::initializer_list<MenuItem*>& items);
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