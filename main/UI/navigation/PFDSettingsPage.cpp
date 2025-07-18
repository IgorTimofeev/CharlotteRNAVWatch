#include "PFDSettingsPage.h"
#include "UI/navigation/routes.h"
#include "rc.h"

namespace pizda {
	PFDSettingsPage::PFDSettingsPage() {
		title.setText(L"PFD settings");

		auto& rc = RC::getInstance();

		// Speed
		speedItem.setTitle(L"Speed: ");
		speedItem.setValue(rc.settings.PFD.speedKt);

		speedItem.setOnInput([&rc, this] {
			rc.settings.PFD.speedKt = speedItem.getValue();
			rc.settings.PFD.scheduleWrite();
		});

		menu.addItem(&speedItem);

		// Altitude
		altitudeItem.setTitle(L"Altitude: ");
		altitudeItem.setValue(rc.settings.PFD.altitudeFt);

		altitudeItem.setOnInput([&rc, this] {
			rc.settings.PFD.altitudeFt = altitudeItem.getValue();
			rc.settings.PFD.scheduleWrite();
		});

		menu.addItem(&altitudeItem);

		// Pressure
		pressureItem.setTitle(L"Baro: ");
		pressureItem.setValue(rc.settings.PFD.pressureHPA);

		pressureItem.setOnInput([&rc, this] {
			rc.settings.PFD.pressureHPA = pressureItem.getValue();
			rc.settings.PFD.scheduleWrite();
		});

		menu.addItem(&pressureItem);

		// Minimums
		minimumsItem.setTitle(L"Minimums: ");
		minimumsItem.setValue(rc.settings.PFD.minimumsFt);

		minimumsItem.setOnInput([&rc, this] {
			rc.settings.PFD.minimumsFt = minimumsItem.getValue();
			rc.settings.PFD.scheduleWrite();
		});

		menu.addItem(&minimumsItem);

		// Performance profile
		performanceProfileMenuItem.setTitle(L"Perf profile");
		performanceProfileMenuItem.setRoute(&Routes::performanceProfile);
		menu.addItem(&performanceProfileMenuItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
