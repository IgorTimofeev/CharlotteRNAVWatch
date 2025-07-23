#include "mainMenuPage.h"
#include "UI/navigation/routes.h"
#include <esp_sleep.h>

#include "rc.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		title.setText(L"Settings");

		// Waypoints
		waypointsItem.setTitle(L"Waypoints");
		waypointsItem.setRoute(&Routes::waypoints);
		menu.addItem(&waypointsItem);

		// PFD
		PFDSettingsItem.setTitle(L"PFD");
		PFDSettingsItem.setRoute(&Routes::PFDSettings);
		menu.addItem(&PFDSettingsItem);

		// GNSS
		GNSSSettingsItem.setTitle(L"GNSS");
		GNSSSettingsItem.setRoute(&Routes::GNSSSettings);
		menu.addItem(&GNSSSettingsItem);

		// Theme
		themeItem.setTitle(L"Theme");
		themeItem.setRoute(&Routes::themeSettings);
		menu.addItem(&themeItem);

		// Dev
		devItem.setTitle(L"Dev");
		devItem.setRoute(&Routes::dev);
		menu.addItem(&devItem);

		// Sleep
		sleepItem.setTitle(L"Sleep");
		sleepItem.setDefaultTitleColor(&Theme::speedBarRed);
		sleepItem.setActiveBackgroundColor(&Theme::speedBarRed);
		sleepItem.setActiveTitleColor(&Theme::bg1);

		sleepItem.setOnPress([] {
			RC::getInstance().sleep();
		});

		menu.addItem(&sleepItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::PFD);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
