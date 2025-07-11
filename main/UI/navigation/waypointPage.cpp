#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointPage::WaypointPage() :
		navWaypointItem(FunctionMenuItem(L"As destination", [] {
			auto& rc = RC::getInstance();

			rc.settings.nav.navWaypointIndex = WaypointItem::getLastWaypointIndex();
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		})),
		bearingWaypointItem(FunctionMenuItem(L"As bearing", [] {
			auto& rc = RC::getInstance();

			rc.settings.nav.bearingWaypointIndex = WaypointItem::getLastWaypointIndex();
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		})),
		removeMenuItem(FunctionMenuItem(L"Delete", [] {
			auto& rc = RC::getInstance();

			const auto waypointIndex = WaypointItem::getLastWaypointIndex();

			if (waypointIndex <= rc.settings.nav.navWaypointIndex) {
				rc.settings.nav.navWaypointIndex = 0;
			}

			if (waypointIndex <= rc.settings.nav.bearingWaypointIndex) {
				rc.settings.nav.bearingWaypointIndex = 0;
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
			&navWaypointItem,
			&bearingWaypointItem,
			&removeMenuItem,
			&backMenuItem
		});
	}
}
