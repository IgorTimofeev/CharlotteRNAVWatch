#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointPage::WaypointPage() :
		directMenuItem(FunctionMenuItem(L"Direct to", [] {
			auto& rc = RC::getInstance();

			rc.settings.nav.waypointIndex = WaypointItem::getLastWaypointIndex();
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		})),
		removeMenuItem(FunctionMenuItem(L"Delete", [] {
			auto& rc = RC::getInstance();

			const auto waypointIndex = WaypointItem::getLastWaypointIndex();

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
		const auto& waypoint = rc.settings.nav.waypoints[WaypointItem::getLastWaypointIndex()];

		title.setText(waypoint.name);

		menu.setItems({
			&directMenuItem,
			&removeMenuItem,
			&backMenuItem
		});
	}
}
