#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			EnglishTextMenuItem nameItem;
			NumberMenuItem latItem;
			NumberMenuItem lonItem;
			NumberMenuItem altItem;

			RouteMenuItem waypointsItem;
			RouteMenuItem devItem;
			RouteMenuItem settingsItem;
			RouteMenuItem backItem;
	};
}