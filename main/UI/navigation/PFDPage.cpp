#include "PFDPage.h"

#include "rc.h"
#include "hardware/korryButton.h"
#include "resources/images.h"
#include "UI/theme.h"
#include "UI/navigation/routes.h"

namespace pizda {
	void PFDPage::onTick() {
		invalidate();
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

	void PFDPage::renderSpeedLines(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, const float value) const {
		const auto& rc = RC::getInstance();

		const auto snappedLineValue = static_cast<int32_t>(value / static_cast<float>(rc.performanceProfile.speedStep)) * rc.performanceProfile.speedStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto computeLineTo = [&lineValue, &lineTo, &center, this, &rc, value] {
			lineTo =
				center + static_cast<Point>(
					Vector2F(displayRadius - speedBarWidth, 0)
					.rotate(std::numbers::pi_v<float> - (value - static_cast<float>(lineValue)) * rc.performanceProfile.speedRadPerUnit)
				);
		};

		const auto renderLine = [&lineValue, &lineTo, renderer, this, &rc] {
			const auto isBig = lineValue % rc.performanceProfile.speedStepBig == 0;
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

			lineValue -= rc.performanceProfile.speedStep;

			if (lineValue < 0)
				break;
		}

		// Up
		lineValue = snappedLineValue + rc.performanceProfile.speedStep;

		while (true) {
			computeLineTo();

			// ESP_LOGI("pizda", "UP lineValue: %f, linetoY: %f", (float) lineValue, (float) lineTo.getY());

			if (lineTo.getY() < sidebarBounds.getY())
				break;

			renderLine();

			lineValue += rc.performanceProfile.speedStep;
		}
	}

	void PFDPage::renderSpeedBars(Renderer* renderer, const Point& center, const float value) {
		const auto& rc = RC::getInstance();

		// Bars
		constexpr static uint8_t diffAngleDeg = 50;
		constexpr static float minAngleRad = toRadians(180 - diffAngleDeg);
		constexpr static float maxAngleRad = toRadians(180 + diffAngleDeg);

		for (const auto& [from, to, color] : rc.performanceProfile.speedBars) {
			const auto radFrom = std::clamp(
				std::numbers::pi_v<float> - (from - value) * rc.performanceProfile.speedRadPerUnit,
				minAngleRad,
				maxAngleRad
			);

			const auto radTo = std::clamp(
				std::numbers::pi_v<float> - (to - value) * rc.performanceProfile.speedRadPerUnit,
				minAngleRad,
				maxAngleRad
			);

			// ESP_LOGI("SPD BARS", "from: %f, to: %f", toDegrees(radFrom), toDegrees(radTo));

			if (radFrom == radTo)
				continue;

			// Thicc bars
			for (uint8_t i = 0; i < speedBarWidth; i++) {
				// To always > from
				renderer->renderArc(
					center,
					displayRadius - i,
					radTo,
					radFrom,
					color
				);
			}
		}
	}

	void PFDPage::renderSpeedValue(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, const float value) {
		auto& rc = RC::getInstance();

		const auto bg = rc.gnss.isSatellitesCountEnough() ? &Theme::bg4 : &Theme::bgRed2;
		const auto fg = rc.gnss.isSatellitesCountEnough() ? &Theme::fg1 : &Theme::fgRed1;
		const auto text = rc.gnss.isSatellitesCountEnough() ? std::format(L"{:03}", static_cast<int16_t>(value)) : L"---";

		const auto valueBounds  = Bounds(
		   sidebarBounds.getX(),
		   center.getY() - sidebarValueHeight / 2,
		   sidebarValueTextMargin + Theme::fontBigThin.getWidth(value < 1000 ? L"888" : L"8888"),
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
				valueBounds.getX() + sidebarValueTextMargin,
				center.getY() - Theme::fontBigThin.getHeight() / 2 + 1
			),
			&Theme::fontBigThin,
			fg,
			text
		);
	}

	void PFDPage::renderSpeedUnderlayText(Renderer* renderer, const Bounds& sidebarBounds) {
		const auto& rc = RC::getInstance();

		const auto underlayX = sidebarBounds.getX() + sidebarUnderlayTextCenterMargin;
		renderUnderlayText(renderer, underlayX, sidebarBounds.getY() - sidebarUnderlayHeight, &Theme::ocean, std::to_wstring(rc.settings.PFD.speedKt));
		renderUnderlayText(renderer, underlayX, sidebarBounds.getY2() + 1, &Theme::purple, L"GS");
	}

	void PFDPage::renderSpeedTrendArrow(Renderer* renderer, const Point& center) {
		const auto& rc = RC::getInstance();

		renderTrendArrow(
			renderer,
			center,
			-rc.speedTrendKt,
			rc.performanceProfile.speedRadPerUnit,
			std::numbers::pi_v<float>
		);
	}

	void PFDPage::renderSpeedBugs(Renderer* renderer, const Point& center, const float value) {
		const auto& rc = RC::getInstance();

		for (const auto& [name, speed] : rc.performanceProfile.speedBugs) {
			const auto angleRad = (value - static_cast<float>(speed)) * rc.performanceProfile.speedRadPerUnit;

			if (std::abs(angleRad) > toRadians(50))
				continue;

			const auto pos = center + static_cast<Point>(Vector2F(displayRadius - speedBarWidth, 0).rotate(std::numbers::pi_v<float> - angleRad));

			renderer->renderHorizontalLine(
				pos,
				speedBugLineWidth,
				&Theme::green
			);

			renderer->renderString(
				Point(
					pos.getX() + speedBugLineWidth + speedBugTextMargin,
					pos.getY() - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::green,
				name
			);
		}
	}

	void PFDPage::renderAltitudeLines(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, const float value) const {
		const auto& rc = RC::getInstance();

		const auto snappedLineValue = static_cast<int32_t>(value / static_cast<float>(rc.performanceProfile.altitudeStep)) * rc.performanceProfile.altitudeStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto computeLineTo = [&lineValue, &lineTo, &center, this, &rc, value] {
			lineTo =
				center +
				static_cast<Point>(Vector2F(displayRadius, 0)
				.rotate((value - static_cast<float>(lineValue)) * rc.performanceProfile.altitudeRadPerUnit));
		};

		const auto renderLine = [&lineValue, &lineTo, renderer, this, &rc] {
			const auto isBig = lineValue % rc.performanceProfile.altitudeStepBig == 0;
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

			lineValue -= rc.performanceProfile.altitudeStep;
		}

		// Up
		lineValue = snappedLineValue + rc.performanceProfile.altitudeStep;

		while (true) {
			computeLineTo();

			if (lineTo.getY() < sidebarBounds.getY())
				break;

			renderLine();

			lineValue += rc.performanceProfile.altitudeStep;
		}
	}

	void PFDPage::renderAltitudeMinimums(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, const float value) {
		const auto& rc = RC::getInstance();

		const auto minimumsPt = center + static_cast<Point>(Vector2F(displayRadius, 0).rotate((value - static_cast<float>(rc.settings.PFD.altitudeFt)) * rc.performanceProfile.altitudeRadPerUnit));

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

	void PFDPage::renderAltitudeValue(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, const float value) {
		auto& rc = RC::getInstance();

		const auto bg = rc.gnss.isSatellitesCountEnough() ? &Theme::bg4 : &Theme::bgRed2;
		const auto fg = rc.gnss.isSatellitesCountEnough() ? &Theme::fg1 : &Theme::fgRed1;
		const auto text = rc.gnss.isSatellitesCountEnough() ? std::format(L"{:03}", static_cast<int16_t>(value)) : L"---";

		auto valueBounds  = Bounds(
		   0,
		   center.getY() - sidebarValueHeight / 2,
		   sidebarValueTextMargin + Theme::fontBigThin.getWidth(value < 1'000 ? L"888" : (value < 10'000 ? L"8888" : L"88888")),
		   sidebarValueHeight
	   );

		valueBounds.setX(sidebarBounds.getX2() - valueBounds.getWidth() + 1);

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
				valueBounds.getX2() - sidebarValueTextMargin + 2 - Theme::fontBigThin.getWidth(text),
				center.getY() - Theme::fontBigThin.getHeight() / 2 + 1
			),
			&Theme::fontBigThin,
			fg,
			text
		);
	}

	void PFDPage::renderAltitudeUnderlayText(Renderer* renderer, const Bounds& sidebarBounds) {
		const auto& rc = RC::getInstance();

		const auto underlayX = sidebarBounds.getX2() - sidebarUnderlayTextCenterMargin + 1;
		renderUnderlayText(renderer, underlayX, sidebarBounds.getY() - sidebarUnderlayHeight, &Theme::ocean, std::to_wstring(rc.settings.PFD.altitudeFt));
		renderUnderlayText(renderer, underlayX, sidebarBounds.getY2() + 1, &Theme::yellow, std::to_wstring(rc.settings.PFD.pressureHPA));
	}

	void PFDPage::renderAltitudeTrendArrow(Renderer* renderer, const Point& center) {
		const auto& rc = RC::getInstance();

		renderTrendArrow(
			renderer,
			center,
			rc.altitudeTrendFt,
			rc.performanceProfile.altitudeRadPerUnit,
			0
		);
	}

	void PFDPage::renderCompass(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		// Normal mode
		if (rc.gnss.isSatellitesCountEnough()) {
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
				   lineDeg == 0 ? &Theme::speedBarRed : &Theme::fg2
			   );

				// Text for big
				if (lineLength == compassLineLength1) {
					std::wstring text;
					const Color* color;

					switch (lineDeg) {
						case 0:
							text = L"N";
							color = &Theme::speedBarRed;
							break;
						case 90:
							text = L"E";
							color = &Theme::fg1;
							break;
						case 180:
							text = L"S";
							color = &Theme::fg1;
							break;
						case 270:
							text = L"W";
							color = &Theme::fg1;
							break;
						default:
							text = std::to_wstring(lineDeg / 10);
							color = &Theme::fg1;
							break;
					}

					const auto textVec = centerVec + lineVec - lineVecNorm * (static_cast<float>(lineLength) + compassLineTexOffset);

					renderer->renderString(
						Point(
							static_cast<int32_t>(textVec.getX()) - Theme::fontNormal.getWidth(text) / 2,
							static_cast<int32_t>(textVec.getY()) - Theme::fontNormal.getHeight() / 2
						),
						&Theme::fontNormal,
						color,
						text
					);
				}
			}

			// Bearing
			{
				const auto bearingRad = toRadians(rc.waypoint2BearingDeg);
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
				const auto HSICDIDeg = normalizeAngle180(static_cast<float>(rc.settings.nav.waypoint1CourseDeg) - rc.waypoint1BearingDeg);
				const auto HSIVec = Vector2F(0, -HSIRadius).rotate(toRadians(rc.settings.nav.waypoint1CourseDeg) - headingRad);
				const auto HSIVecNorm = HSIVec.normalize();
				const auto HSIVecNormPerp = HSIVecNorm.clockwisePerpendicular();

				// ESP_LOGI("HSI", "HSICDIDeg: %f", HSICDIDeg);

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
					if (std::abs(HSICDIDeg) < 90) {
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
					std::abs(HSICDIDeg) > 90 ? (HSICDIDeg > 0 ? 180.f : -180.f) - HSICDIDeg : HSICDIDeg,
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
					const auto& waypoint = rc.settings.nav.waypoints[rc.settings.nav.waypoint1Index];

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
					std::format(L"{:03}", rc.settings.nav.waypoint1CourseDeg)
				);

				// DIS
				renderField(
					renderer,
					Point(
						center.getX() - fieldsVec.getX(),
						center.getY() - fieldsVec.getY()
					),
					L"DIS",
					std::format(L"{:.1f} nm", Units::convertDistance(rc.gnss.getWaypoint1DistanceM(), DistanceUnit::meter, DistanceUnit::nauticalMile))
				);

				// ETE
				renderField(
					renderer,
					Point(
						center.getX() + fieldsVec.getX(),
						center.getY() - fieldsVec.getY()
					),
					L"ETE",
					rc.gnss.getWaypoint1ETESec() > 0
					? std::format(L"{:02}:{:02}", rc.gnss.getWaypoint1ETESec() / 3600, rc.gnss.getWaypoint1ETESec() % 60)
					: L"--:--"
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
				const auto text = L"KEEP WINGS LEVEL";

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
		const auto speed = rc.speedKt;
		const auto altitude = rc.altitudeFt;

		const auto sidebarBounds = Bounds(
			bounds.getX(),
			center.getY() - sidebarHeight / 2,
			bounds.getWidth(),
			sidebarHeight
		);

		// Background
		renderer->clear(&Theme::bg1);

		// Sidebars
		renderSpeedLines(renderer, sidebarBounds, center, speed);
		renderSpeedBars(renderer, center, speed);
		renderSpeedBugs(renderer, center, speed);

		renderAltitudeLines(renderer, sidebarBounds, center, altitude);

		// Vertical panels
		{
			// Top
			renderer->renderFilledRectangle(
				Bounds(
					bounds.getX(),
					bounds.getY(),
					bounds.getWidth(),
					sidebarBounds.getY() - sidebarUnderlayHeight
				),
				&Theme::bg3
			);

			// Bottom
			renderer->renderFilledRectangle(
				Bounds(
					bounds.getX(),
					sidebarBounds.getY2() + 1 + sidebarUnderlayHeight,
					bounds.getWidth(),
					bounds.getHeight() - sidebarBounds.getY2() + 1
				),
				&Theme::bg3
			);
		}

		// Underlays
		{
			// Top
			renderer->renderFilledRectangle(
				Bounds(
					sidebarBounds.getX(),
					sidebarBounds.getY() - sidebarUnderlayHeight,
					sidebarBounds.getWidth(),
					sidebarUnderlayHeight
				),
				&Theme::bg4
			);

			// Bottom
			renderer->renderFilledRectangle(
				Bounds(
					sidebarBounds.getX(),
					sidebarBounds.getY2() + 1,
					sidebarBounds.getWidth(),
					sidebarUnderlayHeight
				),
				&Theme::bg4
			);

			// Texts
			renderSpeedUnderlayText(renderer, sidebarBounds);

			renderAltitudeUnderlayText(renderer, sidebarBounds);
		}

		// Course
		{
			constexpr static uint8_t courseWidth = 42;
			constexpr static uint8_t courseHeight = sidebarWidth + 5;

			const auto bg = rc.gnss.isSatellitesCountEnough() ? &Theme::bg1 : &Theme::bgRed2;
			const auto fg = rc.gnss.isSatellitesCountEnough() ? &Theme::fg1 : &Theme::fgRed1;
			const auto text = rc.gnss.isSatellitesCountEnough() ? std::format(L"{:03}", static_cast<int16_t>(rc.courseDeg)) : L"---";

			renderer->renderFilledRectangle(
				Bounds(
					center.getX() - courseWidth / 2,
					bounds.getY(),
					courseWidth,
					courseHeight
				),
				4,
				bg
			);

			renderer->renderString(
				Point(
					center.getX() - Theme::fontBigThin.getWidth(text) / 2,
					courseHeight / 2 - Theme::fontBigThin.getHeight() / 2 - 2
				),
				&Theme::fontBigThin,
				fg,
				text
			);
		}

		// Time
		{
			constexpr static uint8_t timeWidth = 62;
			constexpr static uint8_t timeHeight = sidebarWidth + 5;

			const auto bg = rc.gnss.haveTime() ? &Theme::bg1 : &Theme::bgRed2;
			const auto fg = rc.gnss.haveTime() ? &Theme::fg1 : &Theme::fgRed1;
			const auto text = rc.gnss.haveTime() ? std::format(L"{:02}{:02}Z", rc.gnss.getTimeHours(), rc.gnss.getTimeMinutes()) : L"----Z";

			renderer->renderFilledRectangle(
				Bounds(
					center.getX() - timeWidth / 2,
					bounds.getY2() - sidebarWidth - 5 + 1,
					timeWidth,
					timeHeight
				),
				bg
			);

			renderer->renderString(
				Point(
					center.getX() - Theme::fontBigThin.getWidth(text) / 2,
					bounds.getY2() - timeHeight / 2 - Theme::fontBigThin.getHeight() / 2 + 3
				),
				&Theme::fontBigThin,
				fg,
				text
			);
		}

		// Compass
		renderCompass(renderer, bounds);

		// Over compass
		renderSpeedTrendArrow(renderer, center);
		renderSpeedValue(renderer, sidebarBounds, center, speed);

		renderAltitudeTrendArrow(renderer, center);
		renderAltitudeMinimums(renderer, sidebarBounds, center, altitude);
		renderAltitudeValue(renderer, sidebarBounds, center, altitude);

		// Satellites
		{
			const auto position = Point(
				center.getX() - 59,
				bounds.getY() + 16
			);

			renderer->renderImage(
				position,
				&resources::Images::satellite
			);

			renderer->renderString(
				Point(
					position.getX() + resources::Images::satellite.getSize().getWidth() + 5,
					position.getY() + resources::Images::satellite.getSize().getHeight() / 2 - Theme::fontNormal.getHeight() / 2 + 5
				),
				&Theme::fontNormal,
				&Theme::fg3,
				std::to_wstring(rc.gnss.getSatellitesCount())
			);
		}

		// Battery
		{
			const auto batteryBounds = Bounds(
				center.getX() + 40,
				bounds.getY() + 19,
				15,
				8
			);

			renderer->renderRectangle(
				batteryBounds,
				&Theme::bg6
			);

			renderer->renderFilledRectangle(
				Bounds(
					batteryBounds.getX() + 1,
					batteryBounds.getY() + 1,
					(batteryBounds.getWidth() - 2) * 8 / 10,
					batteryBounds.getHeight() - 2
				),
				&Theme::good
			);

			renderer->renderRectangle(
				Bounds(
					batteryBounds.getX2() + 1,
					batteryBounds.getYCenter() - 1,
					3,
					3
				),
				&Theme::bg5
			);
		}
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

				rc.settings.nav.waypoint1CourseDeg = static_cast<uint16_t>(normalizeAngle360(static_cast<int32_t>(rc.settings.nav.waypoint1CourseDeg) + incrementValue));
				rc.settings.nav.scheduleWrite();

				event->setHandled(true);
			}
		}
		else {
			if (korryEvent->getEventType() == KorryEventType::down) {
				RC::getInstance().setRoute(&Routes::mainMenu);

				event->setHandled(true);
			}
		}
	}
}
