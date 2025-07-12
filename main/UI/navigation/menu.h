#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "hardware/korryButton.h"
#include "UI/navigation/route.h"
#include "UI/theme.h"
#include "settings/settingsNav.h"

namespace pizda {
	using namespace YOBA;

	enum class MenuItemState : uint8_t {
		normal,
		hovered,
		active
	};

	class MenuItem : public Control, public TextElement {
		public:
			MenuItem();

			MenuItemState getState() const;
			void setState(const MenuItemState state);

		protected:
			void onEvent(Event* event) override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			virtual void onKorryEvent(KorryEvent* event);

		private:
			MenuItemState state = MenuItemState::normal;
	};

	class RouteMenuItem : public MenuItem {
		public:
			RouteMenuItem(std::wstring_view text, const Route* route);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			const Route* _route;
	};

	class WaypointItem : public MenuItem {
		public:
			explicit WaypointItem(uint16_t waypointIndex);

			static uint16_t getLastWaypointIndex();

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			static uint16_t _lastWaypointIndex;
			uint16_t _waypointIndex;
	};

	class FunctionMenuItem : public MenuItem {
		public:
			FunctionMenuItem(std::wstring_view text, const std::function<void()>& function);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			std::function<void()> _function;
	};

	class BoolMenuItem : public MenuItem {
		public:
			explicit  BoolMenuItem(std::wstring_view text);

			Callback<> valueChanged {};

			bool getValue() const;
			void setValue(bool value);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;
			virtual void onValueChanged();

		private:
			bool _value = false;
	};

	class Menu : public StackLayout {
		public:
			explicit Menu();

			void setItems(const std::initializer_list<MenuItem*>& items);
			void addItem(MenuItem* item);
			MenuItem* getItemAt(int16_t index) const;
			int16_t getSelectedIndex() const;
			void setSelectedIndex(int16_t value);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			int16_t _selectedIndex = -1;

			void updateItemsFromSelection() const;
	};
}