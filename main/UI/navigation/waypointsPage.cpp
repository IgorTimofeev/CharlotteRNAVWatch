#include "waypointsPage.h"

#include "rc.h"
#include "UI/navigation/routes.h"
#include "UI/navigation/waypointEditPage.h"

namespace pizda {
	WaypointMenuItem::WaypointMenuItem(const uint16_t waypointIndex) : waypointIndex(waypointIndex) {
		const auto& waypoint = RC::getInstance().settings.nav.waypoints[waypointIndex];
		setTitle(waypoint.name);
	}

	uint16_t WaypointMenuItem::lastWaypointIndex = 0;

	uint16_t WaypointMenuItem::getLastWaypointIndex() {
		return lastWaypointIndex;
	}

	void WaypointMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		const auto& rc = RC::getInstance();

		renderSelectionBackground(renderer, bounds);
		renderTitleOnCenter(renderer, bounds);

		if (waypointIndex == rc.settings.nav.waypoint1Index || waypointIndex == rc.settings.nav.waypoint2Index)
			renderSideIndicator(renderer, bounds, waypointIndex == rc.settings.nav.waypoint1Index ? &Theme::purple1 : &Theme::ocean);
	}

	void WaypointMenuItem::onKorryEvent(KorryEvent* event) {
		if (event->getButtonType() == KorryButtonType::middle && event->getEventType() == KorryEventType::down) {
			lastWaypointIndex = waypointIndex;
			RC::getInstance().setRoute(&Routes::waypoint);

			event->setHandled(true);
		}
	}

	WaypointsPage::WaypointsPage() {
		title.setText(L"Waypoints");

		auto& rc = RC::getInstance();

		// Waypoints
		waypointItems.reserve(rc.settings.nav.waypoints.size());

		for (uint16_t i = 0; i < static_cast<uint16_t>(rc.settings.nav.waypoints.size()); i++) {
			waypointItems.emplace_back(i);
			menu.addItem(&waypointItems[i]);
		}

		// Add
		createItem.setSecondaryColors();
		createItem.setTitle(L"Create");

		createItem.setOnPress([&rc] {
			WaypointEditPage::waypointIndex = -1;

			rc.setRoute(&Routes::waypointEdit);
		});

		menu.addItem(&createItem);

		// Back
		backItem.setBackStyle();
		backItem.setRoute(&Routes::mainMenu);
		menu.addItem(&backItem);

		menu.setSelectedIndex(0);
	}
}
