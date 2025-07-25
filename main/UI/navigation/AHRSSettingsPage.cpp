#include "AHRSSettingsPage.h"
#include "UI/navigation/routes.h"
#include "rc.h"

namespace pizda {
	AHRSSettingsPage::AHRSSettingsPage() {
		title.setText(L"AHRS settings");

		auto& rc = RC::getInstance();

		// Updating interval
		updatingIntervalItem.setTitle(L"UPD: ");
		updatingIntervalItem.setValue(rc.settings.AHRS.updatingIntervalMs);
		updatingIntervalItem.setSuffix(L" ms");

		updatingIntervalItem.setOnInput([this, &rc] {
			rc.settings.AHRS.updatingIntervalMs = updatingIntervalItem.getValue();
			rc.settings.AHRS.scheduleWrite();

			rc.ahrs.updateUpdatingIntervalFromSettings();
		});

		menu.addItem(&updatingIntervalItem);

		// GPS
		GPSSystemItem.setTitle(L"GPS");
		GPSSystemItem.setValue(rc.settings.AHRS.GPS);

		GPSSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.AHRS.GPS = GPSSystemItem.getValue();
			rc.settings.AHRS.scheduleWrite();

			rc.ahrs.updateSystemsFromSettings();
		});

		menu.addItem(&GPSSystemItem);

		// BeiDou
		BeiDouSystemItem.setTitle(L"BeiDou");
		BeiDouSystemItem.setValue(rc.settings.AHRS.BeiDou);

		BeiDouSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.AHRS.BeiDou = BeiDouSystemItem.getValue();
			rc.settings.AHRS.scheduleWrite();

			rc.ahrs.updateSystemsFromSettings();
		});

		menu.addItem(&BeiDouSystemItem);

		// GLONASS
		GLONASSSystemItem.setTitle(L"GLONASS");
		GLONASSSystemItem.setValue(rc.settings.AHRS.GLONASS);

		GLONASSSystemItem.setOnValueChanged([this, &rc] {
			rc.settings.AHRS.GLONASS = GLONASSSystemItem.getValue();
			rc.settings.AHRS.scheduleWrite();

			rc.ahrs.updateSystemsFromSettings();
		});

		menu.addItem(&GLONASSSystemItem);

		// Simulation
		simulationItem.setTitle(L"Simulation");
		simulationItem.setValue(rc.settings.AHRS.simulation);

		simulationItem.setOnValueChanged([this, &rc] {
			rc.settings.AHRS.simulation = simulationItem.getValue();
			rc.settings.AHRS.scheduleWrite();
		});

		menu.addItem(&simulationItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
