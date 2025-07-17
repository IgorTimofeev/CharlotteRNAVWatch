#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/menu.h"

namespace pizda {
	using namespace YOBA;

	class Page : public Layout {
		public:
			Page() {
				rows.setSpacing(10);
				rows.setAlignment(Alignment::center);

				title.setHorizontalAlignment(Alignment::center);
				title.setTextColor(&Theme::fg1);
				title.setFont(&Theme::fontNormal);
				rows += &title;

				*this += &rows;
			}

			StackLayout rows {};
			Text title {};
	};

	class MenuPage : public Page {
		public:
			MenuPage() {
				menu.setSize({ menuWidth, menuHeight });
				rows += &menu;
			}

			constexpr static uint8_t menuWidth = 170;

			constexpr static uint8_t menuItemCount = 5;
			constexpr static uint8_t menuHeight = (MenuItem::height + Menu::itemSpacing) * menuItemCount - Menu::itemSpacing;

			Menu menu {};
	};
}