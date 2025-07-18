#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class WaypointEditPage : public MenuPage {
		public:
			WaypointEditPage();

			static int32_t waypointIndex;

			AZTextMenuItem nameItem {};
			FloatInputMenuItem latItem {};
			FloatInputMenuItem lonItem {};
			IntInputMenuItem altItem {};

			FunctionMenuItem confirmItem {};
			RouteMenuItem backItem {};
	};
}