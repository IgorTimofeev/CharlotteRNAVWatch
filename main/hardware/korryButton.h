#pragma once

#include <YOBA/main.h>
#include "YOBA/UI.h"

namespace pizda {
	using namespace YOBA;

	enum class KorryButtonType : uint8_t {
		up,
		middle,
		down
	};

	class KorryButtonEvent : public Event {
		public:
			KorryButtonEvent(KorryButtonType type, uint32_t time);

			static uint16_t typeID;

			KorryButtonType getType() const;

			uint32_t getTime() const;

		private:
			KorryButtonType _type;
			uint32_t _time;
	};

	class KorryButton : public HID {
		public:
			explicit KorryButton(KorryButtonType type, gpio_num_t pin);

			void setup();

			void tick() override;

		private:
			KorryButtonType _type;
			gpio_num_t _pin;

			// Negative value means press was registered, but not released
			// 0 means no press or release
			// Positive value means press deltaTime
			int64_t _pressTime = 0;

			static void onInterrupt(void* arg);
	};
}
