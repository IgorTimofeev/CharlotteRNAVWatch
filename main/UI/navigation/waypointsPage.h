#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class WaypointsPage : public MenuPage {
		public:
			WaypointsPage();

			RouteMenuItem addMenuItem;
			RouteMenuItem backMenuItem;

			std::vector<WaypointItem> waypointItems {};
	};
}