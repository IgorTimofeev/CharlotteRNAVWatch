#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class AHRSSettingsPage : public MenuPage {
		public:
			AHRSSettingsPage();

			IntInputMenuItem updatingIntervalItem {};

			BoolMenuItem GPSSystemItem {};
			BoolMenuItem BeiDouSystemItem {};
			BoolMenuItem GLONASSSystemItem {};

			BoolMenuItem simulationItem {};

			RouteMenuItem backItem {};
	};
}