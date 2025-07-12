#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class WaypointMenuItem : public TitleMenuItem {
		public:
			explicit WaypointMenuItem(uint16_t waypointIndex);

			static uint16_t getLastWaypointIndex();

		protected:
			void onKorryEvent(KorryEvent* event) override;

		private:
			static uint16_t _lastWaypointIndex;
			uint16_t _waypointIndex;
	};

	class WaypointsPage : public MenuPage {
		public:
			WaypointsPage();

			RouteMenuItem addMenuItem;
			RouteMenuItem backMenuItem;

			std::vector<WaypointMenuItem> waypointItems {};
	};
}