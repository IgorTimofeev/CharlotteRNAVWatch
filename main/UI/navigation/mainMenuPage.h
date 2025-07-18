#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem waypointsItem {};
			RouteMenuItem PFDSettingsItem {};
			RouteMenuItem GNSSSettingsItem {};
			RouteMenuItem themeItem {};
			RouteMenuItem devItem {};
			RouteMenuItem backItem {};
	};
}