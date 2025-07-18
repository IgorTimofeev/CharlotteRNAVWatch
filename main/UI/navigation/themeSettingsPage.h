#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "types/settingsInterface.h"
#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class ThemeMenuItem : public ListMenuItem {
		public:
			explicit ThemeMenuItem(SettingsInterfaceTheme theme);

		protected:
			bool isSelected() const override;
			void onSelectionRequested() override;

		private:
			SettingsInterfaceTheme theme;
	};

	class ThemeSettingsPage : public MenuPage {
		public:
			ThemeSettingsPage();

			ThemeMenuItem darkItem { SettingsInterfaceTheme::dark };
			ThemeMenuItem brightItem { SettingsInterfaceTheme::bright };
			RouteMenuItem backItem {};
	};
}