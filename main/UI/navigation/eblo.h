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
			void renderField(Renderer* renderer, const Point& point, std::wstring_view text1,
									std::wstring_view text2);
			void testBlackBg(Renderer* renderer, const Bounds& bounds);
			void testHorizon(Renderer* renderer, const Bounds& bounds);
			void renderFields(Renderer* renderer, const Bounds& bounds);
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			// Generic
			constexpr static uint8_t displayDiameter = 240;
			constexpr static uint8_t displayRadius = displayDiameter / 2;

			constexpr static uint8_t sidebarWidth = 24;
			constexpr static uint8_t sidebarHeight = 130;

			// Compass
			constexpr static uint8_t compassDiameter = displayDiameter - sidebarWidth * 2;
			constexpr static float compassRadius = compassDiameter / 2.f;
			constexpr static uint8_t compassLineLength1 = 7;
			constexpr static uint8_t compassLineLength2 = 4;
			constexpr static uint8_t compassLineLength3 = 1;
			constexpr static uint8_t compassLineTexOffset = 12;

			// HSI
			constexpr static uint8_t HSIDiameter = compassDiameter;
			constexpr static float HSIRadius = HSIDiameter / 2.f;

			constexpr static uint8_t HSIArrowWidth = 6;
			constexpr static uint8_t HSIArrowHeight = 9;

			constexpr static uint8_t HSICDIAngleMaxDeg = 20;
			constexpr static uint8_t HSICDIAngleStepDeg = 10;
			constexpr static uint8_t HSICDIAnglePixelsPerDeg = 2;
			constexpr static uint8_t HSICDIAngleRadius = 2;

			constexpr static uint8_t HSICDILength = 55;
			constexpr static float HSICDILengthHalf = HSICDILength / 2.f;
			constexpr static uint8_t HSICDIOffset = 5;

			constexpr static float HSIFixedLengthHalf = HSIRadius - HSICDILengthHalf - HSICDIOffset;

			// Bearing
			constexpr static uint8_t bearingDiameter = compassDiameter - 20;
			constexpr static float bearingRadius = bearingDiameter / 2.f;

			constexpr static uint8_t bearingLength = 18;
			constexpr static uint8_t bearingArrowWidth = HSIArrowWidth;
			constexpr static uint8_t bearingArrowHeight = HSIArrowHeight;

			// Speed
			constexpr static uint8_t speedStepPixels = 10;
			constexpr static uint8_t speedStepUnits = 1;
			constexpr static uint8_t speedStepUnitsBig = 5;

			constexpr static uint8_t speedLineLength1 = 2;
			constexpr static uint8_t speedLineLength2 = 2;

			constexpr static uint8_t speedLineTextOffset = 5;

			constexpr static uint8_t speedValueHeight = 15;
			constexpr static uint8_t speedValueWidth = sidebarWidth + 10;
			constexpr static uint8_t speedValueMargin = 5;

			// Heading value
			constexpr static uint8_t headingValueWidth = 36;
			constexpr static uint8_t headingValueHeight = 18;

			// Fields
			constexpr static float fieldsRadius = compassRadius / 2.f;
	};
}