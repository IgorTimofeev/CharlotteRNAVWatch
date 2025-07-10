#pragma once

#include "route.h"

#include "UI/navigation/PFDPage.h"
#include "UI/navigation/settingsPage.h"
#include "UI/navigation/waypointsPage.h"
#include "UI/navigation/waypointPage.h"
#include "UI/navigation/PFDSettingsPage.h"

namespace pizda {
	class Routes {
		public:
			constexpr static ElementRoute<PFDPage> PFD {};
			constexpr static ElementRoute<SettingsPage> settings {};
			constexpr static ElementRoute<WaypointsPage> waypoints {};
			constexpr static ElementRoute<WaypointPage> waypoint {};
			constexpr static ElementRoute<PFDSettingsPage> PFDSettings {};
	};
}