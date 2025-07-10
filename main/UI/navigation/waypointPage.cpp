#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointPage::WaypointPage() :
		directMenuItem(FunctionMenuItem(L"Direct to", [] {
			auto& rc = RC::getInstance();

			rc.settings.nav.waypointIndex = *static_cast<const uint16_t*>(rc.getRouteArg());
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		})),
		removeMenuItem(FunctionMenuItem(L"Delete", [] {
			auto& rc = RC::getInstance();

			const auto waypointIndex = *static_cast<const uint16_t*>(rc.getRouteArg());

			if (waypointIndex <= rc.settings.nav.waypointIndex) {
				rc.settings.nav.waypointIndex = 0;
			}

			// rc.settings.nav.waypoints.erase(rc.settings.nav.waypoints.begin() + waypointIndex);
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		})),
		backMenuItem(RouteMenuItem(L"Back", &Routes::waypoints))
	{
		const auto& rc = RC::getInstance();
		const auto waypointIndex = *static_cast<const uint16_t*>(rc.getRouteArg());
		const auto& waypoint = rc.settings.nav.waypoints[waypointIndex];

		title.setText(waypoint.name);

		menu.setItems({
			&directMenuItem,
			&removeMenuItem,
			&backMenuItem
		});
	}
}
