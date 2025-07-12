#include "PFDSettingsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	PFDSettingsPage::PFDSettingsPage() :
		autopilotSpeedMenuItem(RouteMenuItem(L"Speed", &Routes::mainMenu)),
		autopilotAltitudeMenuItem(RouteMenuItem(L"Altitude", &Routes::mainMenu)),
		minimumsMenuItem(RouteMenuItem(L"Minimums", &Routes::mainMenu)),
		scaleMenuItem(RouteMenuItem(L"Nav mode", &Routes::mainMenu)),
		backMenuItem(RouteMenuItem(L"Back", &Routes::mainMenu))

	{
		title.setText(L"PFD settings");

		menu.setItems({
			&autopilotSpeedMenuItem,
			&autopilotAltitudeMenuItem,
			&minimumsMenuItem,
			&scaleMenuItem,
			&backMenuItem
		});
	}
}
