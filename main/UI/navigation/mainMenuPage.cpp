#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() :
		nameItem(EnglishTextMenuItem(L"Name", { 0, 0, 0, 0 })),
		latItem(NumberMenuItem(L"Latitude", { 0, 0, 0, 0, 0, 0, 0 })),
		lonItem(NumberMenuItem(L"Longitude", { 0, 0, 0, 0, 0, 0, 0 })),
		altItem(NumberMenuItem(L"Altitude", { 0, 0, 0, 0, 0 })),

		waypointsItem(RouteMenuItem(L"Waypoints", &Routes::waypoints)),
		devItem(RouteMenuItem(L"Debug", &Routes::dev)),
		settingsItem(RouteMenuItem(L"Settings", &Routes::settings)),
		backItem(RouteMenuItem(L"Back", &Routes::PFD))
	{
		title.setText(L"Menu");

		menu.setItems({
			&nameItem,
			&latItem,
			&lonItem,
			&altItem,
			&waypointsItem,
			&devItem,
			&settingsItem,
			&backItem,
		});
	}
}