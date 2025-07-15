#include "performanceProfileSettingsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	PerformanceProfileMenuItem::PerformanceProfileMenuItem(const SettingsNavPerformanceProfile performanceProfile) : performanceProfile(performanceProfile) {
		switch (performanceProfile) {
			case SettingsNavPerformanceProfile::cycling:
				setTitle(L"Cycling");
				break;
			case SettingsNavPerformanceProfile::diamondDA40:
				setTitle(L"Diamond DA-40");
				break;
		}
	}

	bool PerformanceProfileMenuItem::isSelected() const {
		return RC::getInstance().settings.nav.performanceProfile == performanceProfile;
	}

	void PerformanceProfileMenuItem::onSelectionRequested() {
		auto& rc = RC::getInstance();
		rc.settings.nav.performanceProfile = performanceProfile;
		rc.settings.nav.scheduleWrite();

		rc.updatePerformanceProfileFromSettings();

		invalidate();
	}

	PerformanceProfileSettingsPage::PerformanceProfileSettingsPage() :
		backItem({ L"Back", &Routes::PFD })
	{
		title.setText(L"Performance profile");

		menu.setItems({
			&cyclingItem,
			&diamondDA40Item,
			&backItem
		});
	}
}
