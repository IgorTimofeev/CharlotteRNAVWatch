#pragma once

#include "route.h"

#include "UI/navigation/PFDPage.h"
#include "UI/navigation/mainMenuPage.h"
#include "UI/navigation/waypointsPage.h"
#include "UI/navigation/waypointPage.h"
#include "UI/navigation/waypointEditPage.h"
#include "UI/navigation/GNSSRawPage.h"
#include "UI/navigation/PFDSettingsPage.h"
#include "UI/navigation/AHRSSettingsPage.h"
#include "UI/navigation/themeSettingsPage.h"
#include "UI/navigation/performanceProfileSettingsPage.h"
#include "UI/navigation/facePage.h"
#include "UI/navigation/analogPage.h"

namespace pizda {
	class Routes {
		public:
			constexpr static ElementRoute<PFDPage> PFD {};

			constexpr static ElementRoute<MainMenuPage> mainMenu {};
			constexpr static ElementRoute<WaypointsPage> waypoints {};
			constexpr static ElementRoute<WaypointPage> waypoint {};
			constexpr static ElementRoute<WaypointEditPage> waypointEdit {};
			constexpr static ElementRoute<GNSSRawPage> GNSSRaw {};
			constexpr static ElementRoute<PFDSettingsPage> PFDSettings {};
			constexpr static ElementRoute<FacePage> face {};
			constexpr static ElementRoute<AnalogPage> analog {};

			constexpr static ElementRoute<ThemeSettingsPage> themeSettings {};
			constexpr static ElementRoute<PerformanceProfileSettingsPage> performanceProfile {};
			constexpr static ElementRoute<AHRSSettingsPage> AHRSSettings {};
	};
}