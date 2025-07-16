#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class PFDPage : public Control {
		public:

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;

		private:
			// Generic
			constexpr static uint8_t displayDiameter = 240;
			constexpr static uint8_t displayRadius = displayDiameter / 2;

			// Sidebar
			constexpr static uint8_t sidebarWidth = 28;
			constexpr static uint8_t sidebarHeight = 128;

			constexpr static uint8_t sidebarLineLength1 = 2;
			constexpr static uint8_t sidebarLineLength2 = 1;
			constexpr static uint8_t sidebarLineTextOffset = 4;

			constexpr static uint8_t sidebarValueHeight = 23;
			constexpr static uint8_t sidebarValueTriangleWidth = 8;
			constexpr static uint8_t sidebarValueTextMargin = 2;

			// Sidebar underlay
			constexpr static uint8_t sidebarUnderlayHeight = 18;
			constexpr static uint8_t sidebarUnderlayTextCenterMargin = 38;

			// Compass
			constexpr static uint8_t compassDiameter = displayDiameter - sidebarWidth * 2;
			constexpr static float compassRadius = compassDiameter / 2.f;
			constexpr static uint8_t compassLineLength1 = 6;
			constexpr static uint8_t compassLineLength2 = 4;
			constexpr static uint8_t compassLineLength3 = 1;
			constexpr static uint8_t compassLineTexOffset = 11;

			// HSI
			constexpr static uint8_t HSIDiameter = compassDiameter;
			constexpr static float HSIRadius = HSIDiameter / 2.f;

			constexpr static uint8_t HSIArrowBigWidth = 7;
			constexpr static uint8_t HSIArrowBigHeight = 9;

			constexpr static uint8_t HSIArrowSmallWidth = 5;
			constexpr static uint8_t HSIArrowSmallHeight = 8;

			constexpr static uint8_t HSICDIAngleStepDeg = 10;
			constexpr static uint8_t HSICDIAngleSteps = 2;
			constexpr static uint8_t HSICDIAngleMaxDeg = HSICDIAngleStepDeg * HSICDIAngleSteps;
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
			constexpr static uint8_t bearingArrowWidth = HSIArrowBigWidth;
			constexpr static uint8_t bearingArrowHeight = HSIArrowBigHeight;

			// Minimums
			constexpr static uint8_t minimumsLineWidth = sidebarWidth + 2;
			constexpr static uint8_t minimumsTriangleWidth = 2;
			constexpr static uint8_t minimumsTriangleHeight = 3;

			// Fields
			constexpr static float fieldsRadius = compassRadius / 2.f;

			// Speed bars
			constexpr static uint8_t speedBarWidth = 2;

			// Speed bugs
			constexpr static uint8_t speedBugLineWidth = 4;
			constexpr static uint8_t speedBugTextMargin = 2;

			void renderSpeedLines(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, float value) const;
			void renderAltitudeLines(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, float value) const;

			static void renderTrendArrow(Renderer* renderer, const Point& center, float trend, float radPerTrendUnit, float offsetRad);
			static void renderUnderlayText(Renderer* renderer, int32_t x, int32_t y, const Color* color, std::wstring_view text);
			static void renderSpeedBars(Renderer* renderer, const Point& center, float value);
			static void renderSpeedValue(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, float value);
			static void renderSpeedUnderlayText(Renderer* renderer, const Bounds& sidebarBounds);
			static void renderSpeedTrendArrow(Renderer* renderer, const Point& center);
			static void renderSpeedBugs(Renderer* renderer, const Point& center, float value);
			static void renderAltitudeMinimums(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, float value);
			static void renderAltitudeValue(Renderer* renderer, const Bounds& sidebarBounds, const Point& center, float value);
			static void renderAltitudeUnderlayText(Renderer* renderer, const Bounds& sidebarBounds);
			static void renderAltitudeTrendArrow(Renderer* renderer, const Point& center);
			static void renderCompass(Renderer* renderer, const Bounds& bounds);
			static void renderField(Renderer* renderer, const Point& point, std::wstring_view text1, std::wstring_view text2);
	};
}