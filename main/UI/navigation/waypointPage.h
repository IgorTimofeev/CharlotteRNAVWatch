#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class WaypointPage : public MenuPage {
		public:
			WaypointPage();

			FunctionMenuItem navWaypointItem;
			FunctionMenuItem bearingWaypointItem;
			FunctionMenuItem removeMenuItem;
			RouteMenuItem backMenuItem;
	};
}