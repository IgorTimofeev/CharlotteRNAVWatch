#include "devPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"

namespace pizda {
	void DevPage::onTick() {
		invalidate();
	}

	void DevPage::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();
		auto pos = Point(bounds.getXCenter() - 60, 45);

		const auto renderLine = [renderer, &pos](const std::wstring_view text) {
			renderer->renderString(
				pos,
				&Theme::fontBig,
				&Theme::fg1,
				text
			);

			pos.setY(pos.getY() + Theme::fontBig.getHeight() + 1);
		};

		renderLine(std::format(L"LAT: {:.5f}", toDegrees(rc.gnss.getLatitudeRad())));
		renderLine(std::format(L"LON: {:.5f}", toDegrees(rc.gnss.getLongitudeRad())));
		renderLine(std::format(L"SPD: {:.2f}", rc.gnss.getSpeedMps()));
		renderLine(std::format(L"ALT: {:.2f}", rc.gnss.getAltitudeM()));
		renderLine(std::format(L"CR1: {:.2f}", rc.gnss.getCourseDeg()));
		renderLine(std::format(L"CR2: {:.2f}", rc.gnss.getComputedCourseDeg()));
		renderLine(std::format(L"NAV: {:.2f}", rc.navWaypointBearingDeg));
		renderLine(std::format(L"BRG: {:.2f}", rc.bearingWaypointBearingDeg));
	}

	void DevPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getButtonType() == KorryButtonType::middle && korryEvent->getEventType() == KorryEventType::down) {
			if (korryEvent->getEventType() == KorryEventType::down) {
				RC::getInstance().setRoute(&Routes::mainMenu);

				event->setHandled(true);
			}
		}
	}
}
