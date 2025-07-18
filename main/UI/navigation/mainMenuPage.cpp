#include "mainMenuPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		title.setText(L"Settings");

		waypointsItem.setTitle(L"Waypoints");
		waypointsItem.setRoute(&Routes::waypoints);
		menu.addItem(&waypointsItem);

		PFDSettingsItem.setTitle(L"PFD");
		PFDSettingsItem.setRoute(&Routes::PFDSettings);
		menu.addItem(&PFDSettingsItem);

		GNSSSettingsItem.setTitle(L"GNSS");
		GNSSSettingsItem.setRoute(&Routes::GNSSSettings);
		menu.addItem(&GNSSSettingsItem);

		themeItem.setTitle(L"Theme");
		themeItem.setRoute(&Routes::themeSettings);
		menu.addItem(&themeItem);

		devItem.setTitle(L"Dev");
		devItem.setRoute(&Routes::dev);
		menu.addItem(&devItem);

		backItem.setBackStyle();
		backItem.setRoute(&Routes::PFD);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}