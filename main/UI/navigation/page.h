#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/theme.h"
#include "UI/navigation/menu.h"

namespace pizda {
	using namespace YOBA;

	class Page : public Layout {
		public:
			Page();

			StackLayout rows {};
			Text title {};
	};

	class MenuPage : public Page {
		public:
			MenuPage();

			constexpr static uint8_t menuWidth = 140;

			constexpr static uint8_t menuItemCount = 5;
			constexpr static uint8_t menuHeight = (MenuItem::height + Menu::itemSpacing) * menuItemCount - Menu::itemSpacing;

			Menu menu {};
	};
}