#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class Eblo : public Control {
		public:


		protected:
			void onTick() override;
			void renderCompass(Renderer* renderer, const Bounds& bounds);
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			constexpr static uint8_t displayDiameterPx = 240;
			constexpr static uint8_t displayRadiusPx = displayDiameterPx / 2;

			constexpr static uint8_t compassLineLength1 = 10;
			constexpr static uint8_t compassLineLength2 = 5;
			constexpr static uint8_t compassLineLength3 = 2;
			constexpr static uint8_t compassLineTexOffset = 15;

			constexpr static uint8_t HSICourseLength = displayDiameterPx - compassLineLength1 * 2;
			constexpr static uint8_t HSICourseLengthHalf = HSICourseLength / 2;

	};
}