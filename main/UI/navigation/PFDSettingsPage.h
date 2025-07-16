#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class PFDSettingsPage : public MenuPage {
		public:
			PFDSettingsPage();

			RouteMenuItem performanceProfileMenuItem;
			RouteMenuItem themeItem;
			RouteMenuItem backMenuItem;
	};
}