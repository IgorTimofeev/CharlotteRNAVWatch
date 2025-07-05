#include "eblo.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	void Eblo::onTick() {
		invalidate();
	}

	void Eblo::renderSpeedUnderlay(Renderer* renderer, const Bounds& speedBounds) {
		const auto pizdulkaBounds =  Bounds(
			speedBounds.getX(),
			speedBounds.getY() + speedBounds.getHeight(),
			pizdulkaWidth,
			pizdulkaHeight
	   );

		renderer->renderFilledRectangle(
			pizdulkaBounds,
			&Theme::bg3
		);

		const auto text = L"GS";

		renderer->renderString(
			Point(
				pizdulkaBounds.getX() + pizdulkaTextCenterMargin - Theme::fontNormal.getWidth(text) / 2,
				pizdulkaBounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::purple,
			text
		);
	}

	void Eblo::renderSpeed(Renderer* renderer, const Bounds& bounds, const Point& center) {
		const auto& rc = RC::getInstance();

		const auto value = rc.speedKt;
		const auto snappedLineValue = static_cast<int32_t>(value / speedStep) * speedStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto vaginoz = [&lineValue, &lineTo, renderer, &center, value] {
			const auto lineAngleRad = toRadians(180) - (value - static_cast<float>(lineValue)) * speedStepRadPerKt;
			lineTo = center + static_cast<Point>(Vector2F(displayRadius, 0).rotate(lineAngleRad));

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
		};

		// Down
		while (lineValue >= 0) {
			vaginoz();

			if (lineTo.getY() > bounds.getY2())
				break;

			lineValue -= speedStep;
		}

		// Up
		lineValue = snappedLineValue + speedStep;

		while (true) {
			vaginoz();

			if (lineTo.getY() < bounds.getY())
				break;

			lineValue += speedStep;
		}

		// Value
		{
			const auto text = std::format(L"{:03}", static_cast<int16_t>(value));

			const auto valueBounds  = Bounds(
			   bounds.getX(),
			   center.getY() - sidebarValueHeight / 2,
			   sidebarValueWidth,
			   sidebarValueHeight
		   );

			renderer->renderFilledRectangle(
				valueBounds,
				&Theme::bg4
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
				&Theme::bg4
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
	}

	Vector2F Eblo::getAltitudeDeg(const float altitude) {
		const auto& rc = RC::getInstance();
		const auto angleRad = (rc.altitudeFt - altitude) * altitudeStepRadPerFt;

		return Vector2F(displayRadius, 0).rotate(angleRad);
	}

	void Eblo::renderAltitudeUnderlay(Renderer* renderer, const Bounds& altitudeBounds) {
		renderer->renderFilledRectangle(
		   Bounds(
				altitudeBounds.getX2() - pizdulkaWidth + 1,
				altitudeBounds.getY() + altitudeBounds.getHeight(),
				pizdulkaWidth,
				pizdulkaHeight
		   ),
		   &Theme::bg3
	   );

		const auto text = L"1013";

		renderer->renderString(
			Point(
				altitudeBounds.getX2() - pizdulkaTextCenterMargin + 1 - Theme::fontNormal.getWidth(text) / 2,
				altitudeBounds.getY() + altitudeBounds.getHeight() + pizdulkaHeight / 2 - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::ocean,
			text
		);
	}

	void Eblo::renderAltitude(Renderer* renderer, const Bounds& bounds, const Point& center) {
		const auto& rc = RC::getInstance();

		const auto value = rc.altitudeFt;
		const auto snappedLineValue = static_cast<int32_t>(value / altitudeStep) * altitudeStep;

		auto lineValue = snappedLineValue;
		Point lineTo;

		const auto vaginoz = [&lineValue, &lineTo, renderer, &center, this] {
			lineTo = center + static_cast<Point>(getAltitudeDeg(static_cast<float>(lineValue)));

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
		while (lineValue >= 0) {
			vaginoz();

			if (lineTo.getY() > bounds.getY2())
				break;

			lineValue -= altitudeStep;
		}

		// Up
		lineValue = snappedLineValue + altitudeStep;

		while (true) {
			vaginoz();

			if (lineTo.getY() < bounds.getY())
				break;

			lineValue += altitudeStep;
		}

		// Minimums
		{
			const auto minimumsPt = center + static_cast<Point>(getAltitudeDeg(rc.altitudeMinimumsFt));

			if (minimumsPt.getY() >= bounds.getY() && minimumsPt.getY() <= bounds.getY2()) {
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
			const auto text = std::format(L"{:04}", static_cast<int16_t>(value));

			const auto valueBounds  = Bounds(
			   bounds.getX2() - sidebarValueWidth + 1,
			   center.getY() - sidebarValueHeight / 2,
			   sidebarValueWidth,
			   sidebarValueHeight
		   );

			renderer->renderFilledRectangle(
				valueBounds,
				&Theme::bg4
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
				&Theme::bg4
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
	}

	void Eblo::renderCompass(Renderer* renderer, const Bounds& bounds) {
		const auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

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
			   &Theme::fg1
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
			const auto bearingRad = toRadians(rc.WPTCourseDeg);
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
			const auto HSIRad = toRadians(rc.selectedCourseDeg);
			const auto HSIVec = Vector2F(0, -HSIRadius).rotate(HSIRad - headingRad);
			const auto HSIVecNorm = HSIVec.normalize();
			const auto HSIVecNormPerp = HSIVecNorm.clockwisePerpendicular();

			// CDI ellipses
			for (int16_t angle = -HSICDIAngleMaxDeg; angle <= HSICDIAngleMaxDeg; angle += HSICDIAngleStepDeg) {
				if (angle == 0)
					continue;

				renderer->renderCircle(
					static_cast<Point>(centerVec + HSIVecNormPerp * static_cast<float>(angle * HSICDIAnglePixelsPerDeg)),
					HSICDIAngleRadius,
					&Theme::fg1
				);
			}

			// Line from
			renderer->renderLine(
				static_cast<Point>(centerVec - HSIVec),
				static_cast<Point>(centerVec - HSIVec + HSIVecNorm * HSIFixedLengthHalf),
				&Theme::purple,
				2
			);

			// Line to
			const auto HSILineTo = static_cast<Point>(centerVec + HSIVec);

			renderer->renderLine(
				static_cast<Point>(centerVec + HSIVec - HSIVecNorm * HSIFixedLengthHalf),
				HSILineTo,
				&Theme::purple,
				2
			);

			// Arrow to
			const auto HSIArrowVec = centerVec + HSIVecNorm * (HSIRadius - HSIArrowHeight);

			renderer->renderFilledTriangle(
				static_cast<Point>(HSIArrowVec + HSIVecNormPerp * HSIArrowWidth),
				static_cast<Point>(HSIArrowVec - HSIVecNormPerp * HSIArrowWidth),
				HSILineTo,
				&Theme::purple
			);

			// CDI
			const auto HSICDIOffsetPixels = std::min(rc.courseDeviationDeg, static_cast<float>(HSICDIAngleMaxDeg)) * HSICDIAnglePixelsPerDeg;
			const auto HSICDIVec = centerVec + HSIVecNormPerp * HSICDIOffsetPixels;

			renderer->renderLine(
				static_cast<Point>(HSICDIVec + HSIVecNorm * HSICDILengthHalf),
				static_cast<Point>(HSICDIVec - HSIVecNorm * HSICDILengthHalf),
				&Theme::purple,
				2
			);
		}
	}

	void Eblo::renderField(Renderer* renderer, const Point& point, const std::wstring_view text1, const std::wstring_view text2) {
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

	void Eblo::testBlackBg(Renderer* renderer, const Bounds& bounds) {
		const auto center = bounds.getCenter();

		// Circle
		renderer->renderFilledCircle(
			center,
			compassRadius,
			&Theme::bg3
		);
	}

	void Eblo::testHorizon(Renderer* renderer, const Bounds& bounds) {
		const auto center = bounds.getCenter();

		// Sky
		renderer->renderFilledCircle(
			center,
			compassRadius,
			&Theme::sky
		);

		// Ground
		const auto oldViewport = renderer->pushViewport(Bounds(
			bounds.getX(),
			bounds.getY() + bounds.getHeight() / 2,
			bounds.getWidth(),
			bounds.getHeight() / 2
		));

		renderer->renderFilledCircle(
			center,
			compassRadius,
			&Theme::ground
		);

		renderer->popViewport(oldViewport);
	}

	void Eblo::renderCompassFields(Renderer* renderer, const Bounds& bounds) {
		const auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		const auto fieldsVec = static_cast<Point>(Vector2F(fieldsRadius, 0).rotate(toRadians(-45)));

		// WPT
		renderField(
			renderer,
			Point(
				center.getX() - fieldsVec.getX(),
				center.getY() + fieldsVec.getY()
			),
			L"WPT",
			L"ULLI"
		);

		// CRS
		renderField(
			renderer,
			Point(
				center.getX() + fieldsVec.getX(),
				center.getY() + fieldsVec.getY()
			),
			L"CRS",
			std::format(L"{:03}", rc.selectedCourseDeg)
		);

		// DIS
		renderField(
			renderer,
			Point(
				center.getX() - fieldsVec.getX(),
				center.getY() - fieldsVec.getY()
			),
			L"DIS",
			std::format(L"{} nm", rc.distance)
		);

		// ETE
		renderField(
			renderer,
			Point(
				center.getX() + fieldsVec.getX(),
				center.getY() - fieldsVec.getY()
			),
			L"ETE",
			std::format(L"{:02}:{:02}", rc.ETESec / 60, rc.ETESec % 60)
		);
	}

	void Eblo::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		// Background
		renderer->clear(&Theme::bg1);

		// Course
		{
			constexpr static uint8_t triangleWidth = 4;
			constexpr static uint8_t triangleHeight = 5;
			constexpr static uint8_t triangleCompassMargin = 3;
			constexpr static uint8_t textMargin = 2;

			const auto triangleBottom = center.getY() - static_cast<int32_t>(compassRadius) - triangleCompassMargin;

			renderer->renderFilledTriangle(
				Point(
					center.getX() - triangleWidth,
					triangleBottom - triangleHeight
				),
				Point(
					center.getX() + triangleWidth,
					triangleBottom - triangleHeight
				),
				Point(
					center.getX(),
					triangleBottom
				),
				&Theme::fg1
			);

			const auto text = std::format(L"{:03}", static_cast<int16_t>(rc.courseDeg));

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text) / 2,
					triangleBottom - triangleHeight - textMargin - Theme::fontNormal.getHeight()
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Time
		{
			const auto text = std::format(L"{:02}:{:02}", rc.gps.getTimeHours(), rc.gps.getTimeMinutes());

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(text) / 2,
					center.getY() + static_cast<int32_t>(compassRadius) + 5
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		const auto speedBounds = Bounds(
			bounds.getX(),
			center.getY() - sidebarHeight / 2,
			sidebarWidth,
			sidebarHeight
		);

		const auto altitudeBounds = Bounds(
		   bounds.getX2() - sidebarWidth + 1,
		   center.getY() - sidebarHeight / 2,
		   sidebarWidth,
		   sidebarHeight
		);

		// Underlays
		renderSpeedUnderlay(renderer, speedBounds);
		renderAltitudeUnderlay(renderer, altitudeBounds);

		// Compass

		testBlackBg(renderer, bounds);
		// testHorizon(renderer, bounds);

		renderCompass(renderer, bounds);
		renderCompassFields(renderer, bounds);

		// Speed
		renderSpeed(
			renderer,
			speedBounds,
			center
		);

		// Altitude
		renderAltitude(
			renderer,
			altitudeBounds,
			center
		);
	}
}
