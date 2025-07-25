#include "waypointEditPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointEditPage::WaypointEditPage() {
		auto& rc = RC::getInstance();
		const auto waypointPtr = waypointIndex >= 0 ? &rc.settings.nav.waypoints[waypointIndex] : nullptr;

		title.setText(waypointPtr ? L"Edit waypoint" : L"New waypoint");

		// )))
		nameItem.setTitle(L"Name: ");
		nameItem.setText(waypointPtr ? waypointPtr->name : L"");
		menu.addItem(&nameItem);

		latItem.setTitle(L"Lat: ");
		latItem.setValue(toDegrees(waypointPtr ? waypointPtr->coordinates.getLatitude() : rc.ahrs.getLatitudeRad()));
		menu.addItem(&latItem);

		lonItem.setTitle(L"Lon: ");
		lonItem.setValue(toDegrees(waypointPtr ? waypointPtr->coordinates.getLongitude() : rc.ahrs.getLongitudeRad()));
		menu.addItem(&lonItem);

		altItem.setTitle(L"Alt: ");
		altItem.setValue(static_cast<int32_t>(waypointPtr ? waypointPtr->coordinates.getAltitude() : rc.ahrs.getAltitudeM()));
		menu.addItem(&altItem);

		// Confirm
		confirmItem.setTitle(L"Save");
		confirmItem.setActiveBackgroundColor(&Theme::red);
		confirmItem.setDefaultTitleColor(&Theme::red);
		confirmItem.setActiveTitleColor(&Theme::bg1);

		confirmItem.setOnPress([this, &rc, waypointPtr] {
			const auto newWaypoint = SettingsNavWaypoint(
				nameItem.getText(),
				{
					toRadians(latItem.getValue()),
					toRadians(lonItem.getValue()),
					static_cast<float>(altItem.getValue())
				}
			);

			// Edit
			if (waypointPtr) {
				*waypointPtr = newWaypoint;
			}
			// New
			else {
				rc.settings.nav.waypoints.push_back(newWaypoint);
			}

			rc.settings.nav.scheduleWrite();

			rc.setRoute(&Routes::waypoints);
		});

		menu.addItem(&confirmItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::waypoints);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}

	int32_t WaypointEditPage::waypointIndex = -1;
}
