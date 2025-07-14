#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class InterfaceSettingsPage : public MenuPage {
		public:
			InterfaceSettingsPage();

			RouteMenuItem themeItem;
			RouteMenuItem backItem;
	};
}