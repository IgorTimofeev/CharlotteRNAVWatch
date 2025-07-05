#include "eblo.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	void Eblo::onTick() {
		invalidate();
	}

	void Eblo::renderSpeed(Renderer* renderer, const Bounds& bounds, const Point& center) {
		const auto& rc = RC::getInstance();

		constexpr static auto lineCountPerHalf = static_cast<uint8_t>(static_cast<float>(sidebarHeight) / 2.f / speedStepPixels);

		const auto value = rc.speedKt;
		const auto lineValueFrom = static_cast<int32_t>(value) - lineCountPerHalf;
		const auto lineValueTo = lineValueFrom + lineCountPerHalf * 2;

		float lineYInt;
		const auto lineYFract = std::modff(value / speedStepUnits, &lineYInt);
		auto lineY = static_cast<float>(lineCountPerHalf) * speedStepPixels + lineYFract * speedStepPixels;

		for (int32_t lineValue = lineValueFrom; lineValue <= lineValueTo; lineValue += speedStepUnits) {
			if (lineValue >= 0) {
				// Line
				const auto isBig = lineValue % altitudeStepUnitsBig == 0;
				const auto lineLength = isBig ? sidebarLineLength1 : sidebarLineLength2;
				const auto lineX = std::sqrtf(displayRadius * displayRadius - lineY * lineY);

				const auto lineFrom = Point(
					center.getX() - static_cast<int32_t>(lineX),
					center.getY() + static_cast<int32_t>(lineY)
				);

				renderer->renderHorizontalLine(
					lineFrom,
					lineLength,
					&Theme::fg6
				);

				// Text
				if (isBig) {
					renderer->renderString(
						Point(
							lineFrom.getX() + lineLength + sidebarLineTextOffset,
							lineFrom.getY() - Theme::fontSmall.getHeight() / 2
						),
						&Theme::fontSmall,
						&Theme::fg6,
						std::to_wstring(lineValue)
					);
				}
			}

			lineY -= speedStepPixels;
		}

		// Value
		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX(),
				center.getY() - sidebarValueHeight / 2,
				sidebarValueWidth,
				sidebarValueHeight
			),
			&Theme::bg4
		);

		renderer->renderString(
			Point(
				bounds.getX() + sidebarValueMargin,
				center.getY() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::fg1,
			std::format(L"{:03}", static_cast<int16_t>(value))
		);
	}

	void Eblo::renderAltitude(Renderer* renderer, const Bounds& bounds, const Point& center) {
		const auto& rc = RC::getInstance();

		static constexpr auto lineCountPerHalf = static_cast<uint8_t>(static_cast<float>(sidebarHeight) / 2.f / altitudeStepPixels);

		const auto value = rc.altitudeFt;

		float valueSnappedIntPart;
		const auto valueSnappedFractPart = std::modff(value / altitudeStepUnits, &valueSnappedIntPart);

		auto lineValue = std::max(
			static_cast<int32_t>(0),
			(static_cast<int32_t>(valueSnappedIntPart) - lineCountPerHalf) * altitudeStepUnits
		);

		auto lineY = (valueSnappedIntPart - static_cast<float>(lineValue) / altitudeStepUnits + valueSnappedFractPart) * altitudeStepPixels;

		// ESP_LOGI("pizda", "valueSnappedIntPart: %f, valueSnappedFractPart: %f", valueSnappedIntPart, valueSnappedFractPart);
		// ESP_LOGI("pizda", "lineValue: %lu, lineY: %f",lineValue, lineY);

		while (center.getY() + static_cast<int32_t>(lineY) > bounds.getY()) {
			// Line
			const auto isBig = lineValue % altitudeStepUnitsBig == 0;
			const auto lineLength = isBig ? sidebarLineLength1 : sidebarLineLength2;
			const auto lineX = std::sqrtf(displayRadius * displayRadius - lineY * lineY);

			const auto lineFrom = Point(
				center.getX() + static_cast<int32_t>(lineX) - lineLength,
				center.getY() + static_cast<int32_t>(lineY)
			);

			renderer->renderHorizontalLine(
				lineFrom,
				lineLength,
				&Theme::fg6
			);

			// Text
			if (isBig) {
				const auto text = std::to_wstring(lineValue);

				renderer->renderString(
					Point(
						lineFrom.getX() - sidebarLineTextOffset - Theme::fontSmall.getWidth(text),
						lineFrom.getY() - Theme::fontSmall.getHeight() / 2
					),
					&Theme::fontSmall,
					&Theme::fg6,
					text
				);
			}

			lineValue += altitudeStepUnits;
			lineY -= altitudeStepPixels;
		}

		// Value
		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX2() - sidebarValueWidth + 1,
				center.getY() - sidebarValueHeight / 2,
				sidebarValueWidth,
				sidebarValueHeight
			),
			&Theme::bg4
		);

		renderer->renderString(
			Point(
				bounds.getX2() - sidebarValueMargin - Theme::fontNormal.getWidth(L"8888"),
				center.getY() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::fg1,
			std::format(L"{:04}", static_cast<int16_t>(value))
		);

		// Pizdulka
		{
			renderer->renderFilledRectangle(
			   Bounds(
					bounds.getX2() - pizdulkaWidth + 1,
					bounds.getY() + bounds.getHeight(),
					pizdulkaWidth,
					pizdulkaHeight
			   ),
			   &Theme::bg3
		   );

			const auto text = L"1013";

			renderer->renderString(
				Point(
					bounds.getX2() - pizdulkaTextMargin - Theme::fontNormal.getWidth(text),
					bounds.getY() + bounds.getHeight() + pizdulkaHeight / 2 - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::ocean,
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

	void Eblo::renderFields(Renderer* renderer, const Bounds& bounds) {
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
			const auto selectedCourseBounds = Bounds(
				center.getX() - verticalBarWidth / 2,
				bounds.getY(),
				verticalBarWidth,
				verticalBarHeight
			);

			renderer->renderFilledRectangle(
				selectedCourseBounds,
				&Theme::bg3
			);

			const auto text = std::format(L"{:03}", static_cast<int16_t>(rc.courseDeg));

			renderer->renderString(
				Point(
					selectedCourseBounds.getXCenter() - Theme::fontNormal.getWidth(text) / 2,
					selectedCourseBounds.getY() + verticalBarTextMargin
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Time
		{
			const auto timeBounds = Bounds(
				center.getX() - verticalBarWidth / 2,
				bounds.getY2() - verticalBarHeight + 1,
				verticalBarWidth,
				verticalBarHeight
			);

			renderer->renderFilledRectangle(
				timeBounds,
				&Theme::bg3
			);

			const auto text = std::format(L"{:02}:{:02}", rc.gps.getTimeHours(), rc.gps.getTimeMinutes());

			renderer->renderString(
				Point(
					timeBounds.getXCenter() - Theme::fontNormal.getWidth(text) / 2,
					timeBounds.getY2() - verticalBarTextMargin + 1 - Theme::fontNormal.getHeight()
				),
				&Theme::fontNormal,
				&Theme::fg1,
				text
			);
		}

		// Speed
		renderSpeed(
			renderer,
			Bounds(
				bounds.getX(),
				center.getY() - sidebarHeight / 2,
				sidebarWidth,
				sidebarHeight
			),
			center
		);

		// Altitude
		renderAltitude(
			renderer,
			Bounds(
			   bounds.getX2() - sidebarWidth + 1,
			   center.getY() - sidebarHeight / 2,
			   sidebarWidth,
			   sidebarHeight
			),
			center
		);

		// testBlack(renderer, bounds);
		// testBlackBg(renderer, bounds);
		// testHorizon(renderer, bounds);

		// Compass
		renderCompass(renderer, bounds);

		// Fields
		renderFields(renderer, bounds);
	}
}
