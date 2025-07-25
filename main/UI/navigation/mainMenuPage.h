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
			RouteMenuItem faceItem {};
			RouteMenuItem PFDSettingsItem {};
			RouteMenuItem AHRSSettingsItem {};
			RouteMenuItem themeItem {};
			FunctionMenuItem sleepItem {};
			FunctionMenuItem backItem {};
	};
}