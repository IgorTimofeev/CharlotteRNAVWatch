#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	enum class ChronoPageTimerState : uint8_t {
		zero,
		active,
		paused
	};

	class ChronoPage : public Control {
		public:

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;

		private:
			static ChronoPageTimerState timerState;
			static int64_t timerTime;
			static float timerDistance;
			static bool timerView;
	};
}
