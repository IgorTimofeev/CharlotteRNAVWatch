#include "mainMenuPage.h"
#include "UI/navigation/routes.h"
#include <esp_sleep.h>

#include "rc.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		title.setText(L"Main menu");

		// Waypoints
		waypointsItem.setTitle(L"Waypoints");
		waypointsItem.setRoute(&Routes::waypoints);
		menu.addItem(&waypointsItem);

		// Face
		faceItem.setTitle(L"Face");
		faceItem.setRoute(&Routes::face);
		menu.addItem(&faceItem);

		// PFD
		PFDSettingsItem.setTitle(L"PFD");
		PFDSettingsItem.setRoute(&Routes::PFDSettings);
		menu.addItem(&PFDSettingsItem);

		// AHRS
		AHRSSettingsItem.setTitle(L"AHRS");
		AHRSSettingsItem.setRoute(&Routes::AHRSSettings);
		menu.addItem(&AHRSSettingsItem);

		// Theme
		themeItem.setTitle(L"Theme");
		themeItem.setRoute(&Routes::themeSettings);
		menu.addItem(&themeItem);

		// Sleep
		sleepItem.setTitle(L"Shutdown");
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
