#include "themeSettingsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	ThemeMenuItem::ThemeMenuItem(const SettingsInterfaceTheme theme): theme(theme) {
		setTitle(theme == SettingsInterfaceTheme::dark ? L"Dark" : L"Bright");
	}

	bool ThemeMenuItem::isSelected() const {
		return RC::getInstance().settings.interface.theme == theme;
	}

	void ThemeMenuItem::onSelectionRequested() {
		auto& rc = RC::getInstance();
		rc.settings.interface.theme = theme;
		rc.settings.interface.scheduleWrite();

		rc.updateThemeFromSettings();

		invalidate();
	}

	ThemeSettingsPage::ThemeSettingsPage() {
		title.setText(L"Theme");

		menu.addItem(&darkItem);
		menu.addItem(&brightItem);

		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
