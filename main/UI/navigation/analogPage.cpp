#include "analogPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"
#include "utils/string.h"

namespace pizda {
	void AnalogPage::onTick() {
		invalidate();
	}

	void AnalogPage::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();

		const auto center = bounds.getCenter();
		const auto radius = bounds.getHeight() / 2;

		constexpr static uint8_t minuteLineToMargin = 18;
		constexpr static uint8_t minuteLineLengthSmall = 2;
		constexpr static uint8_t minuteLineLengthBig = 4;
		constexpr static uint8_t minuteLineTextMargin = 3;
		constexpr static uint8_t minuteHandLength = 40;

		const auto minuteLineRadius = radius - minuteLineToMargin;

		constexpr static uint8_t hourLineToMargin = 8;
		constexpr static uint8_t hourLineLength = 3;
		constexpr static uint8_t hourLineTextMargin = 3;
		constexpr static uint8_t hourHandLength = 30;

		const auto hourLineRadius = minuteLineRadius - hourLineToMargin - minuteLineLengthBig;

		// Minute lines
		for (uint8_t minute = 0; minute < 60; minute++) {
			const auto isBig = minute % 5 == 0;
			const auto lineLength = isBig ? minuteLineLengthBig : minuteLineLengthSmall;
			
			const auto lineVecNorm = Vector2F(0, -1).rotate(static_cast<float>(minute) / 60.f * std::numbers::pi_v<float> * 2);
			const auto lineVecTo = lineVecNorm * static_cast<float>(minuteLineRadius);
			const auto lineVecFrom = lineVecTo - lineVecNorm * lineLength;

			renderer->renderLine(
				center + static_cast<Point>(lineVecFrom),
				center + static_cast<Point>(lineVecTo),
				&Theme::purple
			);

			if (isBig) {
				const auto text = std::format(L"{:02}", minute);
				const auto textWidth = Theme::fontNormal.getWidth(text);
				const auto textVec = center + static_cast<Point>(lineVecTo + lineVecNorm * (static_cast<float>(minuteLineTextMargin) + static_cast<float>(textWidth) / 2));

				renderer->renderString(
					Point(
						textVec.getX() - textWidth / 2,
						textVec.getY() - Theme::fontNormal.getHeight() / 2
					),
					&Theme::fontNormal,
					&Theme::purple,
					text
				);
			}
		}

		// Hour lines
		for (uint8_t hour = 0; hour < 24; hour++) {
			const auto lineVecNorm = Vector2F(0, -1).rotate(static_cast<float>(hour) / 24.f * std::numbers::pi_v<float> * 2);
			const auto lineVecTo = lineVecNorm * static_cast<float>(hourLineRadius);
			const auto lineVecFrom = lineVecTo - lineVecNorm * hourLineLength;

			renderer->renderLine(
				center + static_cast<Point>(lineVecFrom),
				center + static_cast<Point>(lineVecTo),
				&Theme::fg4
			);

			const auto text = hour == 0 ? L"24" : std::to_wstring(hour);
			const auto textWidth = Theme::fontNormal.getWidth(text);
			const auto textVec = center + static_cast<Point>(lineVecFrom - lineVecNorm * (static_cast<float>(hourLineTextMargin) + static_cast<float>(textWidth) / 2));

			renderer->renderString(
				Point(
					textVec.getX() - textWidth / 2,
					textVec.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		const auto renderHand = [renderer, &center, &rc](const uint16_t length, const Color* color, const uint8_t thickness, const float value01) {
			const auto lineVecNorm = Vector2F(0, -1).rotate(value01 * std::numbers::pi_v<float> * 2);
			const auto lineVecTo = lineVecNorm * static_cast<float>(radius);
			const auto lineVecFrom = lineVecTo - lineVecNorm * length;

			renderer->renderLine(
				center + static_cast<Point>(lineVecFrom),
				center + static_cast<Point>(lineVecTo),
				color,
				thickness
			);
		};

		// Hour hand
		renderHand(hourHandLength, &Theme::fg1, 2, static_cast<float>(rc.ahrs.getTimeHours()) / 24.f);

		// Minute hand
		renderHand(minuteHandLength, &Theme::yellow, 2, (static_cast<float>(rc.ahrs.getTimeMinutes()) + static_cast<float>(rc.ahrs.getTimeSeconds()) / 60.f) / 60.f);
	}

	void AnalogPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getButtonType() == KorryButtonType::up || korryEvent->getButtonType() == KorryButtonType::down) {

		}
		else {
			if (korryEvent->getEventType() == KorryEventType::down) {
				RC::getInstance().setRoute(&Routes::mainMenu);

				event->setHandled(true);
			}
		}
	}
}
