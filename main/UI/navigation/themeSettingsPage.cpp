#include "themeSettingsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"

namespace pizda {
	ThemeMenuItem::ThemeMenuItem(const SettingsInterfaceTheme theme): theme(theme) {
		setTitle(theme == SettingsInterfaceTheme::dark ? L"Dark" : L"Bright");
	}

	void ThemeMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() != KorryButtonType::middle || event->getEventType() != KorryEventType::down)
			return;

		auto& rc = RC::getInstance();
		rc.settings.interface.theme = theme;
		rc.settings.interface.scheduleWrite();

		rc.updateThemeFromSettings();

		invalidate();
	}

	ThemeSettingsPage::ThemeSettingsPage() :
		backItem(RouteMenuItem(L"Back", &Routes::interfaceSettings))
	{
		title.setText(L"Theme");

		menu.setItems({
			&darkItem,
			&brightItem,
			&backItem
		});
	}
}
