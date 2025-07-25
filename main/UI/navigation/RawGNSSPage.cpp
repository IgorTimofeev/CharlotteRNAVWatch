#include "RawGNSSPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"
#include "utils/string.h"

namespace pizda {
	void RawGNSSPage::onTick() {
		invalidate();
	}

	void RawGNSSPage::onRender(Renderer* renderer, const Bounds& bounds) {
		constexpr static uint8_t width = 140;
		constexpr static uint8_t height = 140;

		auto& rc = RC::getInstance();
		auto pos = Point(bounds.getXCenter() - width / 2, bounds.getYCenter() - height / 2);

		const auto renderLine = [renderer, &pos](const std::wstring_view text) {
			renderer->renderString(
				pos,
				&Theme::fontBig,
				&Theme::fg1,
				text
			);

			pos.setY(pos.getY() + Theme::fontBig.getHeight() + 1);
		};

		switch (mode) {
			case RawGNSSPageMode::raw: {
				const auto rx = rc.ahrs.getRXData();

				if (!rx.empty()) {
					auto str = StringUtils::toWString(std::string(rx.data()));
					std::erase(str, L'\r');

					Theme::fontBig.wrap(
						str,
						1,
						width,
						[&renderLine](const std::wstring_view line, uint16_t) {
							renderLine(line);
						}
					);
				}

				break;
			}
			default: {
				renderLine(std::format(L"LAT: {:.5f} deg", toDegrees(rc.ahrs.getLatitudeRad())));
				renderLine(std::format(L"LON: {:.5f} deg", toDegrees(rc.ahrs.getLongitudeRad())));
				renderLine(std::format(L"ALT: {:.2f} m", rc.ahrs.getAltitudeM()));
				renderLine(std::format(L"CRS: {:.2f} deg", rc.ahrs.getCourseDeg()));
				renderLine(std::format(L"SPD: {:.2f} mps", rc.ahrs.getSpeedMps()));
				renderLine(std::format(L"NAV: {:.2f} deg", rc.waypoint1BearingDeg));
				renderLine(std::format(L"BRG: {:.2f} deg", rc.waypoint2BearingDeg));

				break;
			}
		}
	}

	void RawGNSSPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getButtonType() == KorryButtonType::middle) {
			if (korryEvent->getEventType() == KorryEventType::down) {
				RC::getInstance().setRoute(&Routes::mainMenu);

				event->setHandled(true);
			}
		}
		else {
			if (korryEvent->getEventType() == KorryEventType::down) {
				auto intMode = static_cast<int16_t>(mode) + (korryEvent->getButtonType() == KorryButtonType::down ? 1 : -1);

				if (intMode < static_cast<int16_t>(RawGNSSPageMode::first)) {
					intMode = static_cast<int16_t>(RawGNSSPageMode::last);
				}
				else if (intMode > static_cast<int16_t>(RawGNSSPageMode::last)) {
					intMode = static_cast<int16_t>(RawGNSSPageMode::first);
				}

				mode = static_cast<RawGNSSPageMode>(intMode);

				invalidate();
			}
		}
	}
}
