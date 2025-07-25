#include "chronoPage.h"

#include <format>
#include "rc.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"
#include "utils/string.h"

namespace pizda {
	ChronoPageTimerState ChronoPage::timerState = ChronoPageTimerState::zero;
	int64_t ChronoPage::timerTime = 0;
	bool ChronoPage::timerView = true;

	void ChronoPage::onTick() {
		invalidate();
	}

	void ChronoPage::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();

		const auto center = bounds.getCenter();
		const auto radius = bounds.getHeight() / 2;

		constexpr static uint8_t hourLineToMargin = 17;
		constexpr static uint8_t hourLineLength = 3;
		constexpr static uint8_t hourLineTextCenterMargin = 11;
		constexpr static uint8_t hourHandLength = 22;
		const auto hourLineRadius = radius - hourLineToMargin;

		constexpr static uint8_t minuteLineToMargin = 7;
		constexpr static uint8_t minuteLineLengthSmall = 1;
		constexpr static uint8_t minuteLineLengthBig = 3;
		constexpr static uint8_t minuteLineTextCenterMargin = 11;
		constexpr static uint8_t minuteHandLength = 22;
		const auto minuteLineRadius = radius - hourLineToMargin - minuteLineToMargin - minuteLineLengthBig;

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
				isBig ? &Theme::purple1 : &Theme::purple2
			);

			if (isBig) {
				const auto text = std::format(L"{:02}", minute);
				const auto textWidth = Theme::fontNormal.getWidth(text);
				const auto textPos = center + static_cast<Point>(lineVecFrom - lineVecNorm * minuteLineTextCenterMargin);

				renderer->renderString(
					Point(
						textPos.getX() - textWidth / 2,
						textPos.getY() - Theme::fontNormal.getHeight() / 2
					),
					&Theme::fontNormal,
					&Theme::purple1,
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
				&Theme::fg5
			);

			const auto text = hour == 0 ? L"24" : std::to_wstring(hour);
			const auto textWidth = Theme::fontNormal.getWidth(text);
			const auto textPos = center + static_cast<Point>(lineVecTo + lineVecNorm * hourLineTextCenterMargin);

			renderer->renderString(
				Point(
					textPos.getX() - textWidth / 2,
					textPos.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Hands
		const auto renderHand = [renderer, &center](const uint8_t radiusTo, const uint16_t length, const Color* color, const uint8_t thickness, const float value01) {
			const auto lineVecNorm = Vector2F(0, -1).rotate(value01 * std::numbers::pi_v<float> * 2);
			const auto lineVecTo = lineVecNorm * radiusTo;
			const auto lineVecFrom = lineVecTo - lineVecNorm * length;

			renderer->renderLine(
				center + static_cast<Point>(lineVecFrom),
				center + static_cast<Point>(lineVecTo),
				color,
				thickness
			);
		};

		renderHand(radius, hourHandLength, &Theme::fg1, 2, (static_cast<float>(rc.ahrs.getTimeHours()) + static_cast<float>(rc.ahrs.getTimeMinutes()) / 60.f) / 24.f);
		renderHand(minuteLineRadius, minuteHandLength, &Theme::purple1, 2, (static_cast<float>(rc.ahrs.getTimeMinutes()) + static_cast<float>(rc.ahrs.getTimeSeconds()) / 60.f) / 60.f);

		// CYKA
		{
			std::wstring text = timerView ? L"Chrono" : L"Uptime";
			const auto y = center.getY() - Theme::fontNormal.getHeight() / 2 - 14;

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text) / 2,
					y
				),
				&Theme::fontNormal,
				&Theme::fg4,
				text
			);

			int64_t timeUs;

			if (timerView) {
				switch (timerState) {
					case ChronoPageTimerState::zero: {
						timeUs = 0;
						break;
					}
					case ChronoPageTimerState::active: {
						timeUs = esp_timer_get_time() - timerTime;
						break;
					}
					default: {
						timeUs = timerTime;
						break;
					}
				}
			}
			else {
				timeUs = esp_timer_get_time();
			}

			text = std::format(L"{:02}:{:02}:{:02}", timeUs / 3600'000'000, timeUs / 60'000'000 % 60, timeUs / 1'000'000 % 60);

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text, 2) / 2,
					y + Theme::fontNormal.getHeight() + 1
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text,
				2
			);
		}
	}

	void ChronoPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getEventType() != KorryEventType::down)
			return;

		if (korryEvent->getButtonType() == KorryButtonType::up) {
			switch (timerState) {
				case ChronoPageTimerState::zero: {
					timerState = ChronoPageTimerState::active;
					timerTime = esp_timer_get_time();

					break;
				}
				case ChronoPageTimerState::active: {
					timerState = ChronoPageTimerState::paused;
					timerTime = esp_timer_get_time() - timerTime;

					break;
				}
				case ChronoPageTimerState::paused: {
					timerState = ChronoPageTimerState::zero;
					timerTime = 0;

					break;
				}
			}

			invalidate();
		}
		else if (korryEvent->getButtonType() == KorryButtonType::down) {
			timerView = !timerView;
			invalidate();
		}
		else {
			RC::getInstance().setRoute(&Routes::mainMenu);
		}

		event->setHandled(true);
	}
}