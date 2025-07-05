#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class Eblo : public Control {
		public:


		protected:
			void onTick() override;
			void renderSpeed(Renderer* renderer, const Bounds& bounds);
			void renderCompass(Renderer* renderer, const Bounds& bounds);
			void testBlackBg(Renderer* renderer, const Bounds& bounds);
			void testHorizon(Renderer* renderer, const Bounds& bounds);
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			// Generic
			constexpr static uint8_t displayDiameter = 240;
			constexpr static uint8_t displayRadius = displayDiameter / 2;

			constexpr static uint8_t sidebarWidth = 24;
			constexpr static uint8_t sidebarHeight = 130;

			// Compass
			constexpr static uint8_t compassDiameter = displayDiameter - sidebarWidth * 2;
			constexpr static uint8_t compassRadius = compassDiameter / 2;
			constexpr static uint8_t compassLineLength1 = 7;
			constexpr static uint8_t compassLineLength2 = 4;
			constexpr static uint8_t compassLineLength3 = 1;
			constexpr static uint8_t compassLineTexOffset = 12;

			// HSI
			constexpr static uint8_t HSICourseLength = compassDiameter - compassLineLength1 * 2;
			constexpr static uint8_t HSICourseLengthHalf = HSICourseLength / 2;

			// Speed
			constexpr static uint8_t speedStepPixels = 10;
			constexpr static uint8_t speedStepUnits = 1;
			constexpr static uint8_t speedStepUnitsBig = 2;

			constexpr static uint8_t speedLineLength1 = 2;
			constexpr static uint8_t speedLineLength2 = 2;

			constexpr static uint8_t speedLineTextOffset = 5;

			constexpr static uint8_t speedValueHeight = 15;
			constexpr static uint8_t speedValueWidth = sidebarWidth + 10;
			constexpr static uint8_t speedValueMargin = 5;

			// Heading value
			constexpr static uint8_t headingValueWidth = 36;
			constexpr static uint8_t headingValueHeight = 18;

	};
}