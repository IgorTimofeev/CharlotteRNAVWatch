#include "GNSSSettingsPage.h"
#include "UI/navigation/routes.h"
#include "rc.h"

namespace pizda {
	GNSSSettingsPage::GNSSSettingsPage() :
		GPSSystemItem(BoolMenuItem(L"GPS")),
		BDSSystemItem(BoolMenuItem(L"BDS")),
		GLONASSSystemItem(BoolMenuItem(L"GLONASS")),
		
		simulationItem(BoolMenuItem(L"Simulation")),
		
		backItem(RouteMenuItem(L"Back", &Routes::mainMenu))
	{
		title.setText(L"GNSS settings");

		auto& rc = RC::getInstance();

		// GPS
		GPSSystemItem.setValue(rc.settings.GNSS.GPS);

		GPSSystemItem.valueChanged += [this, &rc] {
			rc.settings.GNSS.GPS = simulationItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.updateGNSSSystemsFromSettings();
		};

		// BDS
		BDSSystemItem.setValue(rc.settings.GNSS.BDS);

		BDSSystemItem.valueChanged += [this, &rc] {
			rc.settings.GNSS.BDS = simulationItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.updateGNSSSystemsFromSettings();
		};

		// GLONASS
		GLONASSSystemItem.setValue(rc.settings.GNSS.GLONASS);

		GLONASSSystemItem.valueChanged += [this, &rc] {
			rc.settings.GNSS.GLONASS = simulationItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.updateGNSSSystemsFromSettings();
		};
		
		// Simulation
		simulationItem.setValue(rc.settings.GNSS.simulation);

		simulationItem.valueChanged += [this, &rc] {
			rc.settings.GNSS.simulation = simulationItem.getValue();
			rc.settings.GNSS.scheduleWrite();

			rc.updateGNSSSimulationFromSettings();
		};

		menu.setItems({
			&simulationItem,
			&GPSSystemItem,
			&BDSSystemItem,

			&GLONASSSystemItem,
			
			&backItem,
		});
	}
}
