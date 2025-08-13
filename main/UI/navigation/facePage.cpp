#include "facePage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	FacePage::FacePage() {
		title.setText(L"Theme");

		auto& rc = RC::getInstance();

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

		// RAW GNSS
		rawGNSSItem.setTitle(L"Raw GNSS");

		rawGNSSItem.setOnPress([&rc] {
			rc.settings.interface.face = SettingsInterfaceFace::rawGNSS;
			rc.settings.interface.scheduleWrite();

			rc.setFaceRouteFromSettings();
		});

		menu.addItem(&rawGNSSItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
