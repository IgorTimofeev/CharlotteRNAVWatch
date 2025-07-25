#include "analogPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"
#include "utils/string.h"

namespace pizda {
	bool AnalogPage::timerMode = true;
	int64_t AnalogPage::timerTime = 0;

	void AnalogPage::onTick() {
		invalidate();
	}

	void AnalogPage::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();

		const auto center = bounds.getCenter();
		const auto radius = bounds.getHeight() / 2;

		constexpr static uint8_t minuteLineToMargin = 7;
		constexpr static uint8_t minuteLineLength = 3;
		constexpr static uint8_t minuteLineTextMargin = 4;
		constexpr static uint8_t minuteHandLength = 25;

		constexpr static uint8_t hourLineToMargin = 18;
		constexpr static uint8_t hourLineLength = 3;
		constexpr static uint8_t hourLineTextMargin = 5;
		constexpr static uint8_t hourHandLength = 25;

		const auto minuteLineRadius = radius - hourLineToMargin - minuteLineToMargin - minuteLineLength;
		const auto hourLineRadius = radius - hourLineToMargin;

		// Minute lines
		for (uint8_t minute = 0; minute < 60; minute += 5) {
			const auto lineVecNorm = Vector2F(0, -1).rotate(static_cast<float>(minute) / 60.f * std::numbers::pi_v<float> * 2);
			const auto lineVecTo = lineVecNorm * static_cast<float>(minuteLineRadius);
			const auto lineVecFrom = lineVecTo - lineVecNorm * minuteLineLength;

			renderer->renderLine(
				center + static_cast<Point>(lineVecFrom),
				center + static_cast<Point>(lineVecTo),
				&Theme::purple2
			);

			const auto text = std::format(L"{:02}", minute);
			const auto textWidth = Theme::fontNormal.getWidth(text);
			const auto textVec = center + static_cast<Point>(lineVecFrom - lineVecNorm * (static_cast<float>(minuteLineTextMargin) + static_cast<float>(textWidth) / 2));

			renderer->renderString(
				Point(
					textVec.getX() - textWidth / 2,
					textVec.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::purple1,
				text
			);
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
			const auto textVec = center + static_cast<Point>(lineVecTo + lineVecNorm * (static_cast<float>(hourLineTextMargin) + static_cast<float>(textWidth) / 2));

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

		// Hands
		const auto renderHand = [renderer, &center, &rc, radius](const uint16_t length, const Color* color, const uint8_t thickness, const float value01) {
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

		renderHand(hourHandLength, &Theme::fg1, 2, (static_cast<float>(rc.ahrs.getTimeHours()) + static_cast<float>(rc.ahrs.getTimeMinutes()) / 60.f) / 24.f);
		renderHand(minuteHandLength, &Theme::purple1, 2, (static_cast<float>(rc.ahrs.getTimeMinutes()) + static_cast<float>(rc.ahrs.getTimeSeconds()) / 60.f) / 60.f);

		// Uptime
		{
			std::wstring text = timerMode ? L"Chrono" : L"Uptime";
			const auto y = center.getY() - 15 - Theme::fontNormal.getHeight() / 2;

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text) / 2,
					y
				),
				&Theme::fontNormal,
				&Theme::fg4,
				text
			);

			const auto timeUs =
				timerMode
				? (timerTime > 0 ? esp_timer_get_time() - timerTime : 0)
				: esp_timer_get_time();

			text = std::format(L"{:02}:{:02}:{:02}", timeUs / 3600'000'000, timeUs / 60'000'000 % 60, timeUs / 1'000'000 % 60);

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text, 2) / 2,
					y + Theme::fontNormal.getHeight() + 4
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text,
				2
			);
		}
	}

	void AnalogPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getEventType() != KorryEventType::down)
			return;

		if (korryEvent->getButtonType() == KorryButtonType::up) {
			if (timerTime == 0) {
				timerTime = esp_timer_get_time();
			}
			else {
				timerTime = 0;
			}

			invalidate();
		}
		else if (korryEvent->getButtonType() == KorryButtonType::down) {
			timerMode = !timerMode;

			invalidate();
		}
		else {
			RC::getInstance().setRoute(&Routes::mainMenu);
		}

		event->setHandled(true);
	}
}
