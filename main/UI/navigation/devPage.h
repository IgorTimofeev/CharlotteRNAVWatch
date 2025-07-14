#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class DevPage : public Control {
		public:

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
			void onEvent(Event* event) override;
	};
}
