#include "PFDSettingsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	PFDSettingsPage::PFDSettingsPage() :
		performanceProfileMenuItem(RouteMenuItem(L"Perf profile", &Routes::performanceProfile)),
		themeItem(RouteMenuItem(L"Theme", &Routes::themeSettings)),
		backMenuItem(RouteMenuItem(L"Back", &Routes::settings))

	{
		title.setText(L"PFD settings");

		menu.setItems({
			&performanceProfileMenuItem,
			&themeItem,
			&backMenuItem
		});
	}
}
