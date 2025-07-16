#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() :
		waypointsItem(RouteMenuItem(L"Waypoints", &Routes::waypoints)),
		devItem(RouteMenuItem(L"Debug", &Routes::dev)),
		settingsItem(RouteMenuItem(L"Settings", &Routes::settings)),
		backItem(RouteMenuItem(L"Back", &Routes::PFD))
	{
		title.setText(L"Menu");

		menu.setItems({
			&testItem1,
			&testItem2,
			&testItem3,
			&testItem4,
			&testItem5,
			&testItem6,
			&testItem7,
			&testItem8,

			&waypointsItem,
			&devItem,
			&settingsItem,
			&backItem,
		});
	}
}