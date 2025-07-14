#include "interfaceSettingsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	InterfaceSettingsPage::InterfaceSettingsPage() :
		themeItem(RouteMenuItem(L"Theme", &Routes::themeSettings)),
		backItem(RouteMenuItem(L"Back", &Routes::settings))
	{
		title.setText(L"UI settings");

		menu.setItems({
			&themeItem,
			&backItem
		});
	}
}
