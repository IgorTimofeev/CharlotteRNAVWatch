#pragma once

#include <YOBA/main.h>
#include "YOBA/UI.h"

namespace pizda {
	using namespace YOBA;

	enum class KorryButtonType : uint8_t {
		down,
		middle,
		up
	};

	enum class KorryEventType : uint8_t {
		down,
		tick,
		up
	};

	class KorryEvent : public Event {
		public:
			KorryEvent(KorryButtonType buttonType, KorryEventType eventType);

			static uint16_t typeID;

			KorryButtonType getButtonType() const;
			KorryEventType getEventType() const;

		private:
			KorryButtonType _buttonType;
			KorryEventType _type;
	};

	class KorryButton : public HID {
		public:
			KorryButton(KorryButtonType buttonType, gpio_num_t pin);

			void setup() const;
			void tick() override;
			bool isPressed() const;

		private:
			constexpr static uint32_t tickEventDelay = 500'000;
			constexpr static uint32_t tickEventInterval = 100'000;

			KorryButtonType _buttonType;
			gpio_num_t _pin;

			uint64_t _tickTime = 0;
			bool _oldState = false;
	};
}
