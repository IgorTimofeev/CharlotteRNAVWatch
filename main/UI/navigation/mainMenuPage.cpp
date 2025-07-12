#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() :
		waypointsItem(RouteMenuItem(L"Waypoints", &Routes::waypoints)),
		PFDSettingsItem(RouteMenuItem(L"PFD", &Routes::PFDSettings)),
		GNSSSettingsItem(RouteMenuItem(L"GNSS", &Routes::GNSSSettings)),
		rebootItem(FunctionMenuItem(L"Reboot", [] {
			esp_restart();
		})),
		backItem(RouteMenuItem(L"Back", &Routes::PFD))
	{
		title.setText(L"Menu");

		menu.setItems({
			&waypointsItem,
			&PFDSettingsItem,
			&GNSSSettingsItem,
			&rebootItem,
			&backItem,
		});
	}
}
