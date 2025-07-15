#include "PFDSettingsPage.h"
#include "UI/navigation/routes.h"

namespace pizda {
	PFDSettingsPage::PFDSettingsPage() :
		performanceProfileMenuItem(RouteMenuItem(L"Perf profile", &Routes::performanceProfile)),
		backMenuItem(RouteMenuItem(L"Back", &Routes::mainMenu))

	{
		title.setText(L"PFD settings");

		menu.setItems({
			&performanceProfileMenuItem,
			&backMenuItem
		});
	}
}
