#include "eblo.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	void Eblo::onTick() {
		invalidate();
	}

	void Eblo::renderCompass(Renderer* renderer, const Bounds& bounds) {
		const auto& rc = RC::getInstance();

		const auto centerVec = static_cast<Vector2F>(bounds.getCenter());
		const auto headingRad = toRadians(rc.courseDeg);

		for (uint16_t lineDeg = 0; lineDeg < 360; lineDeg += 5) {
			const auto lineVec = Vector2F(0, -displayRadiusPx).rotate(toRadians(lineDeg) - headingRad);
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
				static_cast<Point>(centerVec + lineVecNorm * static_cast<float>(displayRadiusPx - lineLength)),
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

				const auto textVec = centerVec + lineVecNorm * static_cast<float>(displayRadiusPx - lineLength - compassLineTexOffset);

				renderer->renderString(
					Point(
						static_cast<int32_t>(textVec.getX()) - Theme::fontBig.getWidth(text) / 2,
						static_cast<int32_t>(textVec.getY()) - Theme::fontBig.getHeight() / 2
					),
					&Theme::fontBig,
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
		renderCompass(renderer, bounds);

	}
}
