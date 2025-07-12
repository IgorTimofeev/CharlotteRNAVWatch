#pragma once

#include "route.h"

#include "UI/navigation/PFDPage.h"
#include "UI/navigation/mainMenuPage.h"
#include "UI/navigation/waypointsPage.h"
#include "UI/navigation/waypointPage.h"
#include "UI/navigation/PFDSettingsPage.h"
#include "UI/navigation/GNSSSettingsPage.h"

namespace pizda {
	class Routes {
		public:
			constexpr static ElementRoute<PFDPage> PFD {};
			constexpr static ElementRoute<MainMenuPage> mainMenu {};
			constexpr static ElementRoute<WaypointsPage> waypoints {};
			constexpr static ElementRoute<WaypointPage> waypoint {};
			constexpr static ElementRoute<PFDSettingsPage> PFDSettings {};
			constexpr static ElementRoute<GNSSSettingsPage> GNSSSettings {};
	};
}