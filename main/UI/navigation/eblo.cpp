#include "eblo.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	void Eblo::onTick() {
		invalidate();
	}

	void Eblo::renderSpeed(Renderer* renderer, const Bounds& bounds) {
		const auto& rc = RC::getInstance();

		const auto speed = rc.speedKt;
		const auto lineSpeedHalfCount = static_cast<uint8_t>(static_cast<float>(bounds.getHeight()) / 2.f / speedStepPixels);
		const auto lineSpeedFrom = static_cast<int32_t>(speed) - lineSpeedHalfCount;
		const auto lineSpeedTo = lineSpeedFrom + lineSpeedHalfCount * 2;

		const auto center = bounds.getCenter();

		float lineYInt;
		const auto lineYFract = std::modff(speed / speedStepUnits, &lineYInt);
		auto lineY = static_cast<float>(lineSpeedHalfCount) * speedStepPixels + lineYFract * speedStepPixels;

		for (int32_t lineSpeed = lineSpeedFrom; lineSpeed <= lineSpeedTo; lineSpeed += speedStepUnits) {
			if (lineSpeed >= 0) {
				// Line
				const auto lineX = std::sqrtf(displayRadius * displayRadius - lineY * lineY);

				const auto lineFrom = Point(
					center.getX() - static_cast<int32_t>(lineX),
					center.getY() + static_cast<int32_t>(lineY)
				);

				const auto isBig = lineSpeed % speedStepUnitsBig == 0;
				const auto lineLength = isBig ? speedLineLength1 : speedLineLength2;

				renderer->renderHorizontalLine(
					lineFrom,
					lineLength,
					&Theme::fg6
				);

				// Text
				if (isBig) {
					renderer->renderString(
						Point(
							lineFrom.getX() + lineLength + speedLineTextOffset,
							lineFrom.getY() - Theme::fontSmall.getHeight() / 2
						),
						&Theme::fontSmall,
						&Theme::fg6,
						std::to_wstring(lineSpeed)
					);
				}
			}

			lineY -= speedStepPixels;
		}

		// Value
		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX(),
				center.getY() - speedValueHeight / 2,
				speedValueWidth,
				speedValueHeight
			),
			&Theme::bg4
		);

		renderer->renderString(
			Point(
				bounds.getX() + speedValueMargin,
				center.getY() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			&Theme::fg1,
			std::to_wstring(static_cast<uint16_t>(speed))
		);
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

		// Heading value
		{
			renderer->renderHorizontalLine(
				Point(
					center.getX() - headingValueWidth / 2,
					bounds.getY() + headingValueHeight - 1
				),
				headingValueWidth,
				&Theme::fg1
			);

			constexpr static uint8_t triangleSize = 4;

			renderer->renderFilledTriangle(
				Point(
					center.getX() - triangleSize / 2,
					bounds.getY() + headingValueHeight - 1
				),
				Point(
					center.getX() + triangleSize / 2,
					bounds.getY() + headingValueHeight - 1
				),
				Point(
					center.getX(),
					bounds.getY() + headingValueHeight - 1 + triangleSize
				),
				&Theme::fg1
			);

			const auto headingValueText = std::to_wstring(static_cast<int16_t>(rc.courseDeg));

			renderer->renderString(
				Point(
					center.getX() - Theme::fontNormal.getWidth(headingValueText) / 2,
					bounds.getY() + headingValueHeight / 2 - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::fg1,
				headingValueText
			);
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
			const auto HSIRad = toRadians(rc.HSICourseDeg);
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
			&Theme::purple,
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

		// Background
		renderer->clear(&Theme::bg1);

		// Circle
		renderer->renderFilledCircle(
			center,
			compassRadius,
			&Theme::bg3
		);
	}

	void Eblo::testHorizon(Renderer* renderer, const Bounds& bounds) {
		const auto center = bounds.getCenter();

		// Background
		renderer->clear(&Theme::bg1);

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

		// Distance
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
		const auto& rc = RC::getInstance();
		const auto center = bounds.getCenter();

		// testBlack(renderer, bounds);
		// testBlackBg(renderer, bounds);
		testHorizon(renderer, bounds);

		// Compass
		renderCompass(renderer, bounds);

		// Speed
		renderSpeed(
			renderer,
			Bounds(
				bounds.getX(),
				center.getY() - sidebarHeight / 2,
				bounds.getWidth(),
				sidebarHeight
			)
		);

		// Fields
		renderFields(renderer, bounds);
	}
}
