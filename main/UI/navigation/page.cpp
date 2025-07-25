#include "page.h"

namespace pizda {
	Page::Page() {
		rows.setSpacing(10);
		rows.setAlignment(Alignment::center);

		title.setHorizontalAlignment(Alignment::center);
		title.setTextColor(&Theme::fg1);
		title.setFont(&Theme::fontNormal);
		rows += &title;

		*this += &rows;
	}

	MenuPage::MenuPage() {
		menu.setSize({ menuWidth, menuHeight });
		rows += &menu;
	}
}
