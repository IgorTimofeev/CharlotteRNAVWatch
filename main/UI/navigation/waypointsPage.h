#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class WaypointMenuItem : public MenuItem {
		public:
			explicit WaypointMenuItem(uint16_t waypointIndex);

			static uint16_t getLastWaypointIndex();

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onKorryEvent(KorryEvent* event) override;

		private:
			static uint16_t lastWaypointIndex;
			uint16_t waypointIndex;
	};

	class WaypointsPage : public MenuPage {
		public:
			WaypointsPage();

			std::vector<WaypointMenuItem> waypointItems {};

			FunctionMenuItem createItem {};
			RouteMenuItem backItem {};
	};
}