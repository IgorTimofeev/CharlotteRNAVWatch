#include "mainMenuPage.h"
#include "UI/navigation/routes.h"
#include <esp_sleep.h>

#include "rc.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		title.setText(L"Main menu");

		auto& rc = RC::getInstance();

		// Waypoints
		waypointsItem.setTitle(L"Flight plan");
		waypointsItem.setRoute(&Routes::waypoints);
		menu.addItem(&waypointsItem);

		// PFD
		PFDItem.setTitle(L"PFD");

		PFDItem.setOnPress([&rc] {
			rc.settings.interface.face = SettingsInterfaceFace::PFD;
			rc.settings.interface.scheduleWrite();

			rc.setFaceRouteFromSettings();
		});

		menu.addItem(&PFDItem);

		// Chrono
		chronoItem.setTitle(L"Chrono");

		chronoItem.setOnPress([&rc] {
			rc.settings.interface.face = SettingsInterfaceFace::chrono;
			rc.settings.interface.scheduleWrite();

			rc.setFaceRouteFromSettings();
		});

		menu.addItem(&chronoItem);

		// PFD
		rawGNSSItem.setTitle(L"Raw GNSS");

		rawGNSSItem.setOnPress([&rc] {
			rc.settings.interface.face = SettingsInterfaceFace::rawGNSS;
			rc.settings.interface.scheduleWrite();

			rc.setFaceRouteFromSettings();
		});

		menu.addItem(&rawGNSSItem);

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

		backItem.setOnPress([] {
			RC::getInstance().setFaceRouteFromSettings();
		});

		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
