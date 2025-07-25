#include "facePage.h"
#include "UI/navigation/routes.h"
#include "rc.h"

namespace pizda {
	FaceMenuItem::FaceMenuItem(const SettingsInterfaceFace face) : face(face) {
		switch (face) {
			case SettingsInterfaceFace::PFD:
				setTitle(L"PFD");
				break;
			case SettingsInterfaceFace::analog:
				setTitle(L"Analog");
				break;
			case SettingsInterfaceFace::GNSSRaw:
				setTitle(L"Raw GNSS");
				break;
		}
	}

	bool FaceMenuItem::isSelected() const {
		return RC::getInstance().settings.interface.face == face;
	}

	void FaceMenuItem::onSelectionRequested() {
		auto& rc = RC::getInstance();
		rc.settings.interface.face = face;
		rc.settings.interface.scheduleWrite();

		rc.application.scheduleOnTick([&rc] {
			rc.setFaceRouteFromSettings();
		});
	}

	FacePage::FacePage() {
		title.setText(L"Face");

		menu.addItem(&PFDItem);
		menu.addItem(&analogItem);
		menu.addItem(&rawItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
