#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class SettingsPage : public MenuPage {
		public:
			SettingsPage();

			RouteMenuItem waypointsItem;
			RouteMenuItem PFDSettingsItem;
			RouteMenuItem backItem;
	};
}