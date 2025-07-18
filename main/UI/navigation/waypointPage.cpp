#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointPage::WaypointPage() {
		auto& rc = RC::getInstance();
		const auto waypointIndex = WaypointMenuItem::getLastWaypointIndex();
		const auto& waypoint = rc.settings.nav.waypoints[waypointIndex];

		title.setText(waypoint.name);

		// Nav
		navItem.setTitle(L"As destination");

		navItem.setOnPress([&rc, waypointIndex] {
			rc.settings.nav.waypoint1Index = waypointIndex;
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		});

		menu.addItem(&navItem);

		// Bearing
		bearingItem.setTitle(L"As bearing");

		bearingItem.setOnPress([&rc, waypointIndex] {
			rc.settings.nav.waypoint2Index = waypointIndex;
			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::PFD);
		});

		menu.addItem(&bearingItem);

		// Edit
		editItem.setTitle(L"Edit");

		editItem.setOnPress([&rc, waypointIndex] {
			WaypointEditPage::waypointIndex = waypointIndex;

			rc.setRoute(&Routes::waypointEdit);
		});

		menu.addItem(&editItem);

		// Delete
		if (rc.settings.nav.waypoints.size() > 1) {
			deleteItem.setTitle(L"Delete");
			deleteItem.setActiveBackgroundColor(&Theme::red);
			deleteItem.setDefaultTitleColor(&Theme::red);
			deleteItem.setActiveTitleColor(&Theme::bg1);

			deleteItem.setOnPress([&rc, waypointIndex] {
				if (waypointIndex <= rc.settings.nav.waypoint1Index)
					rc.settings.nav.waypoint1Index = 0;

				if (waypointIndex <= rc.settings.nav.waypoint2Index)
					rc.settings.nav.waypoint2Index = 0;

				rc.settings.nav.waypoints.erase(rc.settings.nav.waypoints.begin() + waypointIndex);
				rc.settings.nav.scheduleWrite();

				rc.setRoute(&Routes::waypoints);
			});

			menu.addItem(&deleteItem);
		}

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
