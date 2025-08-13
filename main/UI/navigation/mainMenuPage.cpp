#include "mainMenuPage.h"
#include "UI/navigation/routes.h"
#include <esp_sleep.h>

#include "rc.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		title.setText(L"Main menu");

		// Face
		faceItem.setTitle(L"Display");
		faceItem.setRoute(&Routes::face);
		menu.addItem(&faceItem);

		// Waypoints
		waypointsItem.setTitle(L"Waypoints");
		waypointsItem.setRoute(&Routes::waypoints);
		menu.addItem(&waypointsItem);

		// MCP
		MCPItem.setTitle(L"MCP");
		MCPItem.setRoute(&Routes::MCP);
		menu.addItem(&MCPItem);

		// AHRS
		AHRSSettingsItem.setTitle(L"AHRS");
		AHRSSettingsItem.setRoute(&Routes::AHRSSettings);
		menu.addItem(&AHRSSettingsItem);

		// Theme
		themeItem.setTitle(L"Theme");
		themeItem.setRoute(&Routes::themeSettings);
		menu.addItem(&themeItem);

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

		backItem.setOnPress([] {
			RC::getInstance().setFaceRouteFromSettings();
		});

		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
