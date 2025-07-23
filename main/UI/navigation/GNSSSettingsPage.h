#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class GNSSSettingsPage : public MenuPage {
		public:
			GNSSSettingsPage();

			IntInputMenuItem updatingIntervalItem {};
			IntInputMenuItem dataSamplingZeroingIntervalItem {};
			IntInputMenuItem dataSamplingDistanceItem {};

			BoolMenuItem GPSSystemItem {};
			BoolMenuItem BeiDouSystemItem {};
			BoolMenuItem GLONASSSystemItem {};

			BoolMenuItem simulationItem {};

			RouteMenuItem backItem {};
	};
}