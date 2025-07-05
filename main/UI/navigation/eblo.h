#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class Eblo : public Control {
		public:


		protected:
			void onTick() override;
			void renderSpeedUnderlay(Renderer* renderer, const Bounds& speedBounds);
			void renderSpeed(Renderer* renderer, const Bounds& bounds, const Point& center);
			static Vector2F getAltitudeDeg(float altitude);
			void renderAltitudeUnderlay(Renderer* renderer, const Bounds& altitudeBounds);
			void renderAltitude(Renderer* renderer, const Bounds& bounds, const Point& center);
			void renderCompass(Renderer* renderer, const Bounds& bounds);
			void renderField(Renderer* renderer, const Point& point, std::wstring_view text1,
							std::wstring_view text2);
			void testBlackBg(Renderer* renderer, const Bounds& bounds);
			void testHorizon(Renderer* renderer, const Bounds& bounds);
			void renderCompassFields(Renderer* renderer, const Bounds& bounds);
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			// Generic
			constexpr static uint8_t displayDiameter = 240;
			constexpr static uint8_t displayRadius = displayDiameter / 2;

			// Sidebar
			constexpr static uint8_t sidebarWidth = 28;
			constexpr static uint8_t sidebarHeight = 120;

			constexpr static uint8_t sidebarLineLength1 = 2;
			constexpr static uint8_t sidebarLineLength2 = 1;
			constexpr static uint8_t sidebarLineTextOffset = 4;

			constexpr static uint8_t sidebarValueWidth = sidebarWidth;
			constexpr static uint8_t sidebarValueHeight = 24;
			constexpr static uint8_t sidebarValueMargin = 3;
			constexpr static uint8_t sidebarValueTriangleWidth = 10;

			// Pizdulka
			constexpr static uint8_t pizdulkaWidth = sidebarWidth + 50;
			constexpr static uint8_t pizdulkaHeight = 18;
			constexpr static uint8_t pizdulkaTextCenterMargin = 35;

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
			constexpr static uint8_t speedStep = 1;
			constexpr static uint8_t speedStepBig = 5;
			constexpr static float speedStepRadPerKt = toRadians(4.0f);

			// Altitude
			constexpr static uint16_t altitudeStep = 10;
			constexpr static uint16_t altitudeStepBig = 50;
			constexpr static float altitudeStepRadPerFt = toRadians(0.5f);

			// Minimums
			constexpr static uint8_t minimumsLineWidth = sidebarWidth + 2;
			constexpr static uint8_t minimumsTriangleWidth = 2;
			constexpr static uint8_t minimumsTriangleHeight = 3;

			// Fields
			constexpr static float fieldsRadius = compassRadius / 2.f;
	};
}