#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() :
		waypointsItem(RouteMenuItem(L"Waypoints", &Routes::waypoints)),
		settingsItem(RouteMenuItem(L"Settings", &Routes::settings)),
		backItem(RouteMenuItem(L"Back", &Routes::PFD))
	{
		title.setText(L"Menu");

		menu.setItems({
			&waypointsItem,
			&settingsItem,
			&backItem,
		});
	}
}