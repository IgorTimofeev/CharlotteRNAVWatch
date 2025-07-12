#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	WaypointsPage::WaypointsPage() :
		addMenuItem(RouteMenuItem(L"Add", &Routes::mainMenu)),
		backMenuItem(RouteMenuItem(L"Back", &Routes::mainMenu))
	{
		title.setText(L"Waypoints");

		const auto& settings = RC::getInstance().settings;

		waypointItems.reserve(settings.nav.waypoints.size());

		for (uint16_t i = 0; i < static_cast<uint16_t>(settings.nav.waypoints.size()); i++) {
			waypointItems.emplace_back(i);
		}

		for (auto& waypointItem : waypointItems) {
			menu.addItem(&waypointItem);
		}

		menu.addItem(&addMenuItem);
		menu.addItem(&backMenuItem);
		menu.setSelectedIndex(0);
	}
}
