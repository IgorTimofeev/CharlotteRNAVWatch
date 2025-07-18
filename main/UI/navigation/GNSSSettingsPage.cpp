#include "GNSSSettingsPage.h"
#include "UI/navigation/routes.h"
#include "rc.h"

namespace pizda {
	GNSSSettingsPage::GNSSSettingsPage() {
		title.setText(L"GNSS settings");

		auto& rc = RC::getInstance();

		// GPS
		GPSSystemItem.setTitle(L"GPS");
		GPSSystemItem.setValue(rc.settings.GNSS.GPS);

		GPSSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.GNSS.GPS = GPSSystemItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.gnss.updateSystemsFromSettings();
		});

		menu.addItem(&GPSSystemItem);

		// BeiDou
		BeiDouSystemItem.setTitle(L"BeiDou");
		BeiDouSystemItem.setValue(rc.settings.GNSS.BeiDou);

		BeiDouSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.GNSS.BeiDou = BeiDouSystemItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.gnss.updateSystemsFromSettings();
		});

		menu.addItem(&BeiDouSystemItem);

		// GLONASS
		GLONASSSystemItem.setTitle(L"GLONASS");
		GLONASSSystemItem.setValue(rc.settings.GNSS.GLONASS);

		GLONASSSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.GNSS.GLONASS = GLONASSSystemItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.gnss.updateSystemsFromSettings();
		});

		menu.addItem(&GLONASSSystemItem);

		// Simulation
		simulationItem.setTitle(L"Simulation");
		simulationItem.setValue(rc.settings.GNSS.simulation);

		simulationItem.setOnValueChanged([this, &rc] {
			rc.settings.GNSS.simulation = simulationItem.getValue();
			rc.settings.GNSS.scheduleWrite();
		});

		menu.addItem(&simulationItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
