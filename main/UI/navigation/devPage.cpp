#include "devPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"
#include "utils/string.h"

namespace pizda {
	void DevPage::onTick() {
		invalidate();
	}

	void DevPage::onRender(Renderer* renderer, const Bounds& bounds) {
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
			case DevPageMode::raw: {
				const auto rx = rc.gnss.getRXData();
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

				break;
			}
			default: {
				renderLine(std::format(L"LAT: {:.5f} deg", toDegrees(rc.gnss.getLatitudeRad())));
				renderLine(std::format(L"LON: {:.5f} deg", toDegrees(rc.gnss.getLongitudeRad())));
				renderLine(std::format(L"ALT: {:.2f} m", rc.gnss.getAltitudeM()));
				renderLine(std::format(L"CRS: {:.2f} deg", rc.gnss.getCourseDeg()));
				renderLine(std::format(L"SPD: {:.2f} mps", rc.gnss.getSpeedMps()));
				renderLine(std::format(L"NAV: {:.2f} deg", rc.waypoint1BearingDeg));
				renderLine(std::format(L"BRG: {:.2f} deg", rc.waypoint2BearingDeg));

				break;
			}
		}
	}

	void DevPage::onEvent(Event* event) {
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

				if (intMode < static_cast<int16_t>(DevPageMode::first)) {
					intMode = static_cast<int16_t>(DevPageMode::last);
				}
				else if (intMode > static_cast<int16_t>(DevPageMode::last)) {
					intMode = static_cast<int16_t>(DevPageMode::first);
				}

				mode = static_cast<DevPageMode>(intMode);

				invalidate();
			}
		}
	}
}
