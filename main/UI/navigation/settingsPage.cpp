#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	SettingsPage::SettingsPage() :
		PFDSettingsItem(RouteMenuItem(L"PFD", &Routes::PFDSettings)),
		GNSSSettingsItem(RouteMenuItem(L"GNSS", &Routes::GNSSSettings)),
		interfaceItem(RouteMenuItem(L"Interface", &Routes::interfaceSettings)),
		backItem(RouteMenuItem(L"Back", &Routes::mainMenu))
	{
		title.setText(L"Settings");

		menu.setItems({
			&PFDSettingsItem,
			&GNSSSettingsItem,
			&interfaceItem,
			&backItem
		});
	}
}