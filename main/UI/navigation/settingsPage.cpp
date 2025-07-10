#include "settingsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	SettingsPage::SettingsPage() :
		waypointsItem(RouteMenuItem(L"Waypoints", &Routes::waypoints)),
		PFDSettingsItem(RouteMenuItem(L"PFD options", &Routes::PFDSettings)),
		backItem(RouteMenuItem(L"Back", &Routes::PFD))
	{
		title.setText(L"Menu");

		menu.setItems({
			&waypointsItem,
			&PFDSettingsItem,
			&backItem,
		});
	}
}
