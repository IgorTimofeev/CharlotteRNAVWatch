#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	enum class RawGNSSPageMode : uint8_t {
		raw,
		processed,

		first = raw,
		last = processed
	};

	class RawGNSSPage : public Control {
		public:

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;

		private:
			RawGNSSPageMode mode = RawGNSSPageMode::raw;
	};
}
