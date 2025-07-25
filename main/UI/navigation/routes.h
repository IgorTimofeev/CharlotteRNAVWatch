#pragma once

#include "route.h"

#include "UI/navigation/PFDPage.h"
#include "UI/navigation/mainMenuPage.h"
#include "UI/navigation/waypointsPage.h"
#include "UI/navigation/waypointPage.h"
#include "UI/navigation/waypointEditPage.h"
#include "UI/navigation/RawGNSSPage.h"
#include "UI/navigation/MCPPage.h"
#include "UI/navigation/AHRSSettingsPage.h"
#include "UI/navigation/themeSettingsPage.h"
#include "UI/navigation/performanceProfileSettingsPage.h"
#include "UI/navigation/chronoPage.h"

namespace pizda {
	class Routes {
		public:
			constexpr static ElementRoute<PFDPage> PFD {};

			constexpr static ElementRoute<MainMenuPage> mainMenu {};
			constexpr static ElementRoute<WaypointsPage> waypoints {};
			constexpr static ElementRoute<WaypointPage> waypoint {};
			constexpr static ElementRoute<WaypointEditPage> waypointEdit {};
			constexpr static ElementRoute<RawGNSSPage> rawGNSS {};
			constexpr static ElementRoute<MCPPage> MCP {};
			constexpr static ElementRoute<ChronoPage> chrono {};

			constexpr static ElementRoute<ThemeSettingsPage> themeSettings {};
			constexpr static ElementRoute<PerformanceProfileSettingsPage> performanceProfile {};
			constexpr static ElementRoute<AHRSSettingsPage> AHRSSettings {};
	};
}