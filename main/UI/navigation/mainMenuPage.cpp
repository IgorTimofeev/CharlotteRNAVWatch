#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() :
		nameItem({ L"Name: ", L"ULLI" }),
		latItem({ L"Latitude: ", L"60.000" }),
		lonItem({ L"Longitude: ", L"60.000" }),
		altItem({ L"Altitude: ", L"32000" }),

		waypointsItem({L"Waypoints", &Routes::waypoints}),
		devItem({L"Debug", &Routes::dev}),
		settingsItem({L"Settings", &Routes::settings}),
		backItem({L"Back", &Routes::PFD})
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