#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class SettingsPage : public MenuPage {
		public:
			SettingsPage();

			RouteMenuItem PFDSettingsItem;
			RouteMenuItem GNSSSettingsItem;
			FunctionMenuItem rebootItem;
			RouteMenuItem backItem;
	};
}