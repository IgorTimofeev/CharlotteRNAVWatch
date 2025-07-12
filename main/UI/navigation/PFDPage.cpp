#include "PFDPage.h"

#include "rc.h"
#include "hardware/korryButton.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"

namespace pizda {
	void PFDPage::onTick() {
		invalidate();
	}

	Vector2F PFDPage::getSpeedVec(const float speed) {
		const auto& rc = RC::getInstance();

		return Vector2F(displayRadius, 0).rotate(toRadians(180) - (rc.speedKt - speed) * speedStepRadPerKt);
	}

	Vector2F PFDPage::getAltitudeVec(const float altitude) {
		const auto& rc = RC::getInstance();
		const auto angleRad = (rc.altitudeFt - altitude) * altitudeStepRadPerFt;

		return Vector2F(displayRadius, 0).rotate(angleRad);
	}

	void PFDPage::renderTrendArrow(Renderer* renderer, const Point& center, const float trend, const float radPerTrendUnit, const float offsetRad) {
		if (std::abs(trend) <= 1)
			return;

		const auto angleRad = std::clamp(trend * radPerTrendUnit, toRadians(-70.f), toRadians(70.f));

		auto angleFrom = offsetRad;
		auto angleTo = offsetRad + angleRad;

		while (angleFrom < 0)
			angleFrom += std::numbers::pi_v<float> * 2;

		while (angleTo < 0)
			angleTo += std::numbers::pi_v<float> * 2;

		if (std::abs(angleTo - angleFrom) > std::numbers::pi_v<float>) {
			std::swap(angleFrom, angleTo);

			if (angleTo == 0)
				angleTo = toRadians(359.99);
		}

		// ESP_LOGI("pizda", "angleRad: %f, angleFrom: %f, angleTo: %f", toDegrees(angleRad), toDegrees(angleFrom), toDegrees(angleTo));

		renderer->renderArc(
			center,
			static_cast<uint16_t>(compassRadius + 2),
			angleFrom,
			angleTo,
			&Theme::green
		);
	}

	void PFDPage::renderUnderlayText(Renderer* renderer, const int32_t x, const int32_t y, const Color* color, const std::wstring_view text) {
		renderer->renderString(
			Point(
				x - Theme::fontNormal.getWidth(text) / 2,
				y + sidebarUnderlayHeight / 2 - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			color,
			text
		);
	}

	void PFDPage::renderSpeed(Renderer* renderer, const Bounds& sidebarBounds, const Point& center) const {
		const auto& rc = RC::getInstance();

		const auto value = rc.speedKt;
		const auto snappedLineValue = static_cast<int32_t>(value / speedStep) * speedStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto computeLineTo = [&lineValue, &lineTo, &center, this] {
			lineTo = center + static_cast<Point>(getSpeedVec(static_cast<float>(lineValue)));
		};

		const auto renderLine = [&lineValue, &lineTo, renderer, this] {
			const auto isBig = lineValue % speedStepBig == 0;
			const auto lineLength = isBig ? sidebarLineLength1 : sidebarLineLength2;

			renderer->renderHorizontalLine(
				lineTo,
				lineLength,
				&Theme::fg6
			);

			if (isBig) {
				renderer->renderString(
					Point(
						lineTo.getX() + lineLength + sidebarLineTextOffset,
						lineTo.getY() - Theme::fontSmall.getHeight() / 2
					),
					&Theme::fontSmall,
					&Theme::fg6,
					std::to_wstring(lineValue)
				);
			}

			return false;
		};

		// Down
		while (true) {
			computeLineTo();

			// ESP_LOGI("pizda", "DOWN lineValue: %f, linetoY: %f", (float) lineValue, (float) lineTo.getY());

			if (lineTo.getY() > sidebarBounds.getY2())
				break;

			renderLine();

			lineValue -= speedStep;

			if (lineValue < 0)
				break;
		}

		// Up
		lineValue = snappedLineValue + speedStep;

		while (true) {
			computeLineTo();

			// ESP_LOGI("pizda", "UP lineValue: %f, linetoY: %f", (float) lineValue, (float) lineTo.getY());

			if (lineTo.getY() < sidebarBounds.getY())
				break;

			renderLine();

			lineValue += speedStep;
		}

		// Trend
		renderTrendArrow(
			renderer,
			center,
			-rc.speedTrendKt,
			speedStepRadPerKt,
			toRadians(180)
		);

		// Value
		{
			const auto bg = rc.gnss.haveSpeed() ? &Theme::bg4 : &Theme::bgRed2;
			const auto text = rc.gnss.haveSpeed() ? std::format(L"{:03}", static_cast<int16_t>(value)) : L"---";

			const auto valueBounds  = Bounds(
			   sidebarBounds.getX(),
			   center.getY() - sidebarValueHeight / 2,
			   sidebarValueWidth,
			   sidebarValueHeight
		   );

			renderer->renderFilledRectangle(
				valueBounds,
				bg
			);

			renderer->renderFilledTriangle(
				Point(
					valueBounds.getX2() + 1,
					valueBounds.getY()
				),
				Point(
					valueBounds.getX2() + 1,
					valueBounds.getY2()
				),
				Point(
					valueBounds.getX2() + 1 + sidebarValueTriangleWidth - 1,
					center.getY()
				),
				bg
			);

			renderer->renderString(
				Point(
					valueBounds.getX() + sidebarValueMargin,
					center.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Underlay
		{
			const auto underlayX = sidebarBounds.getX() + sidebarUnderlayTextCenterMargin;
			renderUnderlayText(renderer, underlayX, sidebarBounds.getY() - sidebarUnderlayHeight, &Theme::ocean, std::to_wstring(rc.settings.PFD.speedKt));
			renderUnderlayText(renderer, underlayX, sidebarBounds.getY2() + 1, &Theme::purple, L"GS");
		}
	}

	void PFDPage::renderAltitude(Renderer* renderer, const Bounds& sidebarBounds, const Point& center) const {
		const auto& rc = RC::getInstance();

		const auto value = rc.altitudeFt;
		const auto snappedLineValue = static_cast<int32_t>(value / altitudeStep) * altitudeStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto computeLineTo = [&lineValue, &lineTo, &center, this] {
			lineTo = center + static_cast<Point>(getAltitudeVec(static_cast<float>(lineValue)));
		};

		const auto renderLine = [&lineValue, &lineTo, renderer, this] {
			const auto isBig = lineValue % altitudeStepBig == 0;
			const auto lineLength = isBig ? sidebarLineLength1 : sidebarLineLength2;

			renderer->renderHorizontalLine(
				Point(
					lineTo.getX() - lineLength,
					lineTo.getY()
				),
				lineLength,
				&Theme::fg6
			);

			if (isBig) {
				const auto text = std::to_wstring(lineValue);

				renderer->renderString(
					Point(
						lineTo.getX() - lineLength - sidebarLineTextOffset - Theme::fontSmall.getWidth(text),
						lineTo.getY() - Theme::fontSmall.getHeight() / 2
					),
					&Theme::fontSmall,
					&Theme::fg6,
					text
				);
			}
		};

		// Down
		while (true) {
			computeLineTo();

			if (lineTo.getY() > sidebarBounds.getY2())
				break;

			renderLine();

			lineValue -= altitudeStep;

			if (lineValue < 0)
				break;
		}

		// Up
		lineValue = snappedLineValue + altitudeStep;

		while (true) {
			computeLineTo();

			if (lineTo.getY() < sidebarBounds.getY())
				break;

			renderLine();

			lineValue += altitudeStep;
		}

		// Trend
		renderTrendArrow(
			renderer,
			center,
			rc.altitudeTrendFt,
			altitudeStepRadPerFt,
			0
		);

		// Minimums
		{
			const auto minimumsPt = center + static_cast<Point>(getAltitudeVec(rc.settings.PFD.altitudeFt));

			if (minimumsPt.getY() >= sidebarBounds.getY() && minimumsPt.getY() <= sidebarBounds.getY2()) {
				renderer->renderHorizontalLine(
					Point(
						minimumsPt.getX() - minimumsLineWidth,
						minimumsPt.getY()
					),
					minimumsLineWidth,
					&Theme::yellow
				);

				renderer->renderTriangle(
					Point(
						minimumsPt.getX() - minimumsLineWidth - minimumsTriangleWidth,
						minimumsPt.getY() - minimumsTriangleHeight
					),
					Point(
						minimumsPt.getX() - minimumsLineWidth - minimumsTriangleWidth,
						minimumsPt.getY() + minimumsTriangleHeight
					),
					Point(
						minimumsPt.getX() - minimumsLineWidth,
						minimumsPt.getY()
					),
					&Theme::yellow
				);
			}
		}

		// Value
		{
			const auto bg = rc.gnss.haveSpeed() ? &Theme::bg4 : &Theme::bgRed2;
			const auto text = rc.gnss.haveAltitude() ? std::format(L"{:04}", static_cast<int16_t>(value)) : L"----";

			const auto valueBounds  = Bounds(
			   sidebarBounds.getX2() - sidebarValueWidth + 1,
			   center.getY() - sidebarValueHeight / 2,
			   sidebarValueWidth,
			   sidebarValueHeight
		   );

			renderer->renderFilledRectangle(
				valueBounds,
				bg
			);

			renderer->renderFilledTriangle(
				Point(
					valueBounds.getX() - 1,
					valueBounds.getY()
				),
				Point(
					valueBounds.getX() - 1,
					valueBounds.getY2()
				),
				Point(
					valueBounds.getX() - sidebarValueTriangleWidth + 1,
					center.getY()
				),
				bg
			);

			renderer->renderString(
				Point(
					valueBounds.getX2() - sidebarValueMargin + 2 - Theme::fontNormal.getWidth(text),
					center.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Underlay
		{
			const auto underlayX = sidebarBounds.getX2() - sidebarUnderlayTextCenterMargin + 1;
			renderUnderlayText(renderer, underlayX, sidebarBounds.getY() - sidebarUnderlayHeight, &Theme::ocean, std::to_wstring(rc.settings.PFD.altitudeFt));
			renderUnderlayText(renderer, underlayX, sidebarBounds.getY2() + 1, &Theme::yellow, std::to_wstring(rc.settings.PFD.pressureHPA));
		}
	}

	void PFDPage::renderCompass(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		// Normal mode
		if (rc.gnss.haveLocation()) {
			renderer->renderFilledCircle(
			   center,
			   compassRadius,
			   &Theme::bg1
			);

			const auto centerVec = static_cast<Vector2F>(center);
			const auto headingRad = toRadians(rc.courseDeg);

			for (uint16_t lineDeg = 0; lineDeg < 360; lineDeg += 5) {
				const auto lineVec = Vector2F(0, -compassRadius).rotate(toRadians(lineDeg) - headingRad);
				const auto lineVecNorm = lineVec.normalize();

				// Line
				uint8_t lineLength;

				if (lineDeg % 30 == 0) {
					lineLength = compassLineLength1;
				}
				else if (lineDeg % 10 == 0) {
					lineLength = compassLineLength2;
				}
				else {
					lineLength = compassLineLength3;
				}

				renderer->renderLine(
				   static_cast<Point>(centerVec + lineVec),
				   static_cast<Point>(centerVec + lineVec - lineVecNorm * lineLength),
				   &Theme::fg2
			   );

				// Text for big
				if (lineLength == compassLineLength1) {
					std::wstring text;

					switch (lineDeg) {
						case 0: text = L"N"; break;
						case 90: text = L"E"; break;
						case 180: text = L"S"; break;
						case 270: text = L"W"; break;
						default: text = std::to_wstring(lineDeg / 10); break;
					}

					const auto textVec = centerVec + lineVec - lineVecNorm * (static_cast<float>(lineLength) + compassLineTexOffset);

					renderer->renderString(
						Point(
							static_cast<int32_t>(textVec.getX()) - Theme::fontNormal.getWidth(text) / 2,
							static_cast<int32_t>(textVec.getY()) - Theme::fontNormal.getHeight() / 2
						),
						&Theme::fontNormal,
						&Theme::fg1,
						text
					);
				}
			}

			// Bearing
			{
				const auto bearingRad = toRadians(rc.bearingWaypointBearingDeg);
				const auto bearingVec = Vector2F(0, -bearingRadius).rotate(bearingRad - headingRad);
				const auto bearingVecNorm = bearingVec.normalize();
				const auto bearingVecNormPerp = bearingVecNorm.clockwisePerpendicular();

				// Line
				const auto lineFromVec = centerVec - bearingVec;
				const auto lineToVec = centerVec + bearingVec;

				// Line to
				renderer->renderLine(
					static_cast<Point>(lineToVec - bearingVecNorm * bearingLength),
					static_cast<Point>(lineToVec),
					&Theme::ocean,
					2
				);

				// Line from
				renderer->renderLine(
					static_cast<Point>(lineFromVec + bearingVecNorm * bearingLength),
					static_cast<Point>(lineFromVec),
					&Theme::ocean,
					2
				);

				// Arrow
				const auto arrowVec = lineToVec - bearingVecNorm * bearingArrowHeight;

				// Arrow left
				renderer->renderLine(
					static_cast<Point>(arrowVec - bearingVecNormPerp * bearingArrowWidth),
					static_cast<Point>(lineToVec),
					&Theme::ocean,
					2
				);

				// Arrow right
				renderer->renderLine(
					static_cast<Point>(arrowVec + bearingVecNormPerp * bearingArrowWidth),
					static_cast<Point>(lineToVec),
					&Theme::ocean,
					2
				);
			}

			// HSI
			{
				const auto HSICDIDeg = rc.settings.nav.navWaypointCourseDeg - rc.navWaypointBearingDeg;
				const auto HSIVec = Vector2F(0, -HSIRadius).rotate(toRadians(rc.settings.nav.navWaypointCourseDeg) - headingRad);
				const auto HSIVecNorm = HSIVec.normalize();
				const auto HSIVecNormPerp = HSIVecNorm.clockwisePerpendicular();

				// CDI ellipses
				for (int16_t stepIndex = -HSICDIAngleSteps; stepIndex <= HSICDIAngleSteps; stepIndex++) {
					if (stepIndex == 0)
						continue;

					renderer->renderCircle(
						static_cast<Point>(centerVec + HSIVecNormPerp * static_cast<float>(stepIndex * HSICDIAngleStepDeg * HSICDIAnglePixelsPerDeg)),
						HSICDIAngleRadius,
						&Theme::fg1
					);
				}

				// Line head
				const auto HSILineHeadToVec = centerVec + HSIVec;
				const auto HSILineHeadTo = static_cast<Point>(HSILineHeadToVec);

				const auto HSILineHeadFromVec = HSILineHeadToVec - HSIVecNorm * HSIFixedLengthHalf;

				renderer->renderLine(
					static_cast<Point>(HSILineHeadFromVec),
					HSILineHeadTo,
					&Theme::purple,
					2
				);

				// Line tail
				const auto HSILineTailFromVec = centerVec - HSIVec + HSIVecNorm * HSIFixedLengthHalf;

				renderer->renderLine(
					static_cast<Point>(HSILineTailFromVec),
					static_cast<Point>(centerVec - HSIVec),
					&Theme::purple,
					2
				);

				// Big arrow
				{
					const auto arrowFrom = HSILineHeadToVec - HSIVecNorm * HSIArrowBigHeight;

					renderer->renderFilledTriangle(
						static_cast<Point>(arrowFrom + HSIVecNormPerp * HSIArrowBigWidth),
						static_cast<Point>(arrowFrom - HSIVecNormPerp * HSIArrowBigWidth),
						HSILineHeadTo,
						&Theme::purple
					);
				}

				// Small arrow
				{
					if (std::abs(HSICDIDeg) < 180) {
						renderer->renderFilledTriangle(
							static_cast<Point>(HSILineHeadFromVec + HSIVecNormPerp * HSIArrowSmallWidth),
							static_cast<Point>(HSILineHeadFromVec - HSIVecNormPerp * HSIArrowSmallWidth),
							static_cast<Point>(HSILineHeadFromVec + HSIVecNorm * HSIArrowSmallHeight),
							&Theme::purple
						);
					}
					else {
						renderer->renderFilledTriangle(
							static_cast<Point>(HSILineTailFromVec + HSIVecNormPerp * HSIArrowSmallWidth),
							static_cast<Point>(HSILineTailFromVec - HSIVecNormPerp * HSIArrowSmallWidth),
							static_cast<Point>(HSILineTailFromVec - HSIVecNorm * HSIArrowSmallHeight),
							&Theme::purple
						);
					}
				}

				// CDI
				const auto HSICDIOffsetPixels = std::clamp(
					HSICDIDeg,
					-static_cast<float>(HSICDIAngleMaxDeg),
					static_cast<float>(HSICDIAngleMaxDeg)
				) * HSICDIAnglePixelsPerDeg;

				const auto HSICDIVec = centerVec + HSIVecNormPerp * HSICDIOffsetPixels;

				renderer->renderLine(
					static_cast<Point>(HSICDIVec + HSIVecNorm * HSICDILengthHalf),
					static_cast<Point>(HSICDIVec - HSIVecNorm * HSICDILengthHalf),
					&Theme::purple,
					2
				);
			}

			// Fields
			{
				const auto fieldsVec = static_cast<Point>(Vector2F(fieldsRadius, 0).rotate(toRadians(-45)));

				// WPT
				{
					const auto& waypoint = rc.settings.nav.waypoints[rc.settings.nav.navWaypointIndex];

					renderField(
					   renderer,
					   Point(
						   center.getX() - fieldsVec.getX(),
						   center.getY() + fieldsVec.getY()
					   ),
					   L"WPT",
					   waypoint.name
				   );
				}

				// CRS
				renderField(
					renderer,
					Point(
						center.getX() + fieldsVec.getX(),
						center.getY() + fieldsVec.getY()
					),
					L"CRS",
					std::format(L"{:03}", rc.settings.nav.navWaypointCourseDeg)
				);

				// DIS
				renderField(
					renderer,
					Point(
						center.getX() - fieldsVec.getX(),
						center.getY() - fieldsVec.getY()
					),
					L"DIS",
					std::format(L"{:.1f} nm", rc.navWaypointDistanceNm)
				);

				// ETE
				renderField(
					renderer,
					Point(
						center.getX() + fieldsVec.getX(),
						center.getY() - fieldsVec.getY()
					),
					L"ETE",
					std::format(L"{:02}:{:02}", rc.navWaypointETESec / 3600, rc.navWaypointETESec % 60)
				);
			}
		}
		// AHRS align
		else {
			renderer->renderFilledCircle(
			   center,
			   compassRadius,
			   &Theme::bgRed1
			);

			// 1
			{
				const auto text = L"AHRS ALIGN";

				renderer->renderString(
					Point(
						center.getX() - Theme::fontNormal.getWidth(text) / 2,
						center.getY() - Theme::fontNormal.getHeight()
					),
					&Theme::fontNormal,
					&Theme::yellow,
					text
				);
			}

			// 2
			{
				const auto text = std::format(L"{:.1f} / {:.1f}", toDegrees(rc.gnss.getLatitudeRad()), toDegrees(rc.gnss.getLongitudeRad()));

				renderer->renderString(
					Point(
						center.getX() - Theme::fontNormal.getWidth(text) / 2,
						center.getY() + 1
					),
					&Theme::fontNormal,
					&Theme::yellow,
					text
				);
			}
		}
	}

	void PFDPage::renderField(Renderer* renderer, const Point& point, const std::wstring_view text1, const std::wstring_view text2) {
		constexpr static uint8_t spacing = 1;

		renderer->renderString(
			Point(
				point.getX() - Theme::fontNormal.getWidth(text1) / 2,
				point.getY() - Theme::fontNormal.getHeight() - spacing / 2
			),
			&Theme::fontNormal,
			&Theme::fg5,
			text1
		);

		renderer->renderString(
			Point(
				point.getX() - Theme::fontNormal.getWidth(text2) / 2,
				point.getY() + spacing / 2
			),
			&Theme::fontNormal,
			&Theme::purple,
			text2
		);
	}

	void PFDPage::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		const auto sidebarBounds = Bounds(
			bounds.getX(),
			center.getY() - sidebarHeight / 2,
			bounds.getWidth(),
			sidebarHeight
		);

		// Background
		renderer->clear(&Theme::bg3);

		// Sidebar blackout
		renderer->renderFilledRectangle(
			sidebarBounds,
			&Theme::bg2
		);

		// Sidebar underlay
		renderer->renderFilledRectangle(
			Bounds(
				sidebarBounds.getX(),
				sidebarBounds.getY2() + 1,
				sidebarBounds.getWidth(),
				sidebarUnderlayHeight
			),
			&Theme::bg4
		);

		renderer->renderFilledRectangle(
			Bounds(
				sidebarBounds.getX(),
				sidebarBounds.getY() - sidebarUnderlayHeight,
				sidebarBounds.getWidth(),
				sidebarUnderlayHeight
			),
			&Theme::bg4
		);

		// Course
		{
			constexpr static uint8_t courseWidth = 42;
			constexpr static uint8_t courseHeight = 20;

			constexpr static uint8_t triangleWidth = 4;
			constexpr static uint8_t triangleHeight = 5;

			const auto courseBounds = Bounds(
				center.getX() - courseWidth / 2,
				bounds.getY(),
				courseWidth,
				courseHeight
			);

			renderer->renderFilledRectangle(
				courseBounds,
				4,
				&Theme::bg1
			);

			renderer->renderFilledTriangle(
				Point(
					center.getX() - triangleWidth,
					courseBounds.getY2() + 1
				),
				Point(
					center.getX() + triangleWidth,
					courseBounds.getY2() + 1
				),
				Point(
					center.getX(),
					courseBounds.getY2() + 1 + triangleHeight
				),
				&Theme::bg1
			);

			const auto text = std::format(L"{:03}", static_cast<int16_t>(rc.courseDeg));

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text) / 2,
					courseBounds.getYCenter() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Time
		{
			constexpr static uint8_t timeWidth = 42;
			constexpr static uint8_t timeHeight = 20;

			const auto timeBounds = Bounds(
				center.getX() - timeWidth / 2,
				bounds.getY2() - timeHeight + 1,
				timeWidth,
				timeHeight
			);

			renderer->renderFilledRectangle(
				timeBounds,
				4,
				&Theme::bg1
			);

			const auto text = rc.gnss.haveTime() ? std::format(L"{:02}:{:02}", rc.gnss.getTimeHours(), rc.gnss.getTimeMinutes()) : L"--:--";

			renderer->renderString(
				Point(
					timeBounds.getXCenter() - Theme::fontNormal.getWidth(text) / 2,
					timeBounds.getYCenter() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Compass
		renderCompass(renderer, bounds);

		// Speed
		renderSpeed(
			renderer,
			sidebarBounds,
			center
		);

		// Altitude
		renderAltitude(
			renderer,
			sidebarBounds,
			center
		);
	}

	void PFDPage::onEvent(Event* event) {
		if (event->getTypeID() != KorryEvent::typeID)
			return;

		const auto korryEvent = reinterpret_cast<KorryEvent*>(event);

		if (korryEvent->getButtonType() == KorryButtonType::up || korryEvent->getButtonType() == KorryButtonType::down) {
			if (korryEvent->getEventType() == KorryEventType::down || korryEvent->getEventType() == KorryEventType::tick) {
				auto& rc = RC::getInstance();

				const uint8_t incrementMagnitude = korryEvent->getEventType() == KorryEventType::tick ? 5 : 1;

				const auto incrementValue =
					korryEvent->getButtonType() == KorryButtonType::down
					? static_cast<int16_t>(incrementMagnitude)
					: static_cast<int16_t>(-incrementMagnitude);

				rc.settings.nav.navWaypointCourseDeg = static_cast<uint16_t>(normalizeAngle360(static_cast<int32_t>(rc.settings.nav.navWaypointCourseDeg) + incrementValue));
				rc.settings.nav.scheduleWrite();

				event->setHandled(true);
			}
		}
		else {
			if (korryEvent->getEventType() == KorryEventType::down) {
				RC::getInstance().setRoute(&Routes::settings);

				event->setHandled(true);
			}
		}
	}
}
