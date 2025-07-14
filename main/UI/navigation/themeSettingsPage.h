#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "settings/settingsInterface.h"
#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class ThemeMenuItem : public TitleMenuItem {
		public:
			explicit ThemeMenuItem(SettingsInterfaceTheme theme);

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			SettingsInterfaceTheme theme;
	};

	class ThemeSettingsPage : public MenuPage {
		public:
			ThemeSettingsPage();

			ThemeMenuItem darkItem { SettingsInterfaceTheme::dark };
			ThemeMenuItem brightItem { SettingsInterfaceTheme::bright };
			RouteMenuItem backItem;
	};
}