#include "waypointsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	PFDSettingsPage::PFDSettingsPage() :
		autopilotSpeedMenuItem(RouteMenuItem(L"Speed", &Routes::settings)),
		autopilotAltitudeMenuItem(RouteMenuItem(L"Altitude", &Routes::settings)),
		minimumsMenuItem(RouteMenuItem(L"Minimums", &Routes::settings)),
		scaleMenuItem(RouteMenuItem(L"Nav mode", &Routes::settings)),
		backMenuItem(RouteMenuItem(L"Back", &Routes::settings))

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
