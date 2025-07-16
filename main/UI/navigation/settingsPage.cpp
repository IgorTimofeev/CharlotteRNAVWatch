#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	SettingsPage::SettingsPage() :
		performanceProfileMenuItem(RouteMenuItem(L"Perf profile", &Routes::performanceProfile)),
		themeItem(RouteMenuItem(L"Theme", &Routes::themeSettings)),
		GNSSSettingsItem(RouteMenuItem(L"GNSS", &Routes::GNSSSettings)),
		backItem(RouteMenuItem(L"Back", &Routes::mainMenu))
	{
		title.setText(L"Settings");

		menu.setItems({
			&performanceProfileMenuItem,
			&themeItem,
			&GNSSSettingsItem,
			&backItem
		});
	}
}