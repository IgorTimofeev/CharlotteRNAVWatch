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
		auto lineY = lineSpeedHalfCount * speedStepPixels + lineYFract * speedStepPixels;

		for (int32_t lineSpeed = lineSpeedFrom; lineSpeed <= lineSpeedTo; lineSpeed += speedStepUnits) {
			if (lineSpeed >= 0) {
				const auto lineX = std::sqrtf(displayRadius * displayRadius - lineY * lineY);

				const auto lineFrom = Point(
					center.getX() - static_cast<int32_t>(lineX),
					center.getY() + lineY
				);

				const auto isBig = lineSpeed % speedStepUnitsBig == 0;
				const auto lineLength = isBig ? speedLineLength1 : speedLineLength2;

				renderer->renderHorizontalLine(
					lineFrom,
					lineLength,
					&Theme::fg1
				);

				if (isBig) {
					renderer->renderString(
						Point(
							lineFrom.getX() + lineLength + speedLineTextOffset,
							lineFrom.getY() - Theme::fontSmall.getHeight() / 2
						),
						&Theme::fontSmall,
						&Theme::fg1,
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

			{
				const Color* lineColor;

				if (lineLength == compassLineLength1 || lineLength == compassLineLength2) {
					lineColor = &Theme::fg1;
				}
				else {
					lineColor = &Theme::fg1;
				}

				renderer->renderLine(
				   static_cast<Point>(centerVec + lineVec),
				   static_cast<Point>(centerVec + lineVecNorm * static_cast<float>(compassRadius - lineLength)),
				   lineColor
			   );
			}

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

				const auto textVec = centerVec + lineVecNorm * static_cast<float>(compassRadius - lineLength - compassLineTexOffset);

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

		// HSI
		const auto HSICourseRad = toRadians(rc.HSICourseDeg);

		const auto HSICourseVec = Vector2F(0, -HSICourseLengthHalf).rotate(HSICourseRad - headingRad);
		const auto HSICourseTo = static_cast<Point>(centerVec + HSICourseVec);
		const auto HSICourseFrom = static_cast<Point>(centerVec - HSICourseVec);

		renderer->renderLine(
			HSICourseFrom,
			HSICourseTo,
			&Theme::purple
		);
	}

	void Eblo::onRender(Renderer* renderer, const Bounds& bounds) {
		const auto center = bounds.getCenter();

		// Background
		renderer->clear(&Theme::bg3);

		// Circle
		renderer->renderFilledCircle(
			center,
			compassRadius,
			&Theme::bg1
		);

		// Top bar
		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX(),
				bounds.getY(),
				bounds.getWidth(),
				verticalBarSize
			),
			&Theme::bg1
		);

		// Bottom bar
		renderer->renderFilledRectangle(
			Bounds(
				bounds.getX(),
				bounds.getY2() - verticalBarSize + 1,
				bounds.getWidth(),
				verticalBarSize
			),
			&Theme::bg1
		);

		// Compass
		renderCompass(renderer, bounds);

		// Speed
		renderSpeed(
			renderer,
			Bounds(
				bounds.getX(),
				bounds.getY() + verticalBarSize,
				bounds.getWidth(),
				bounds.getHeight() - verticalBarSize * 2
			)
		);
	}
}
