#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	enum class GNSSRawPageMode : uint8_t {
		raw,
		processed,

		first = raw,
		last = processed
	};

	class GNSSRawPage : public Control {
		public:

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;

		private:
			GNSSRawPageMode mode = GNSSRawPageMode::raw;
	};
}
