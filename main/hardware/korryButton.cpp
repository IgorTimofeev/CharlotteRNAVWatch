#include "korryButton.h"
#include "rc.h"

namespace pizda {
	KorryEvent::KorryEvent(const KorryButtonType buttonType, const KorryEventType eventType): Event(typeID), _buttonType(buttonType), _type(eventType) {

	}

	KorryButtonType KorryEvent::getButtonType() const {
		return _buttonType;
	}

	KorryEventType KorryEvent::getEventType() const {
		return _type;
	}

	uint16_t KorryEvent::typeID = registerTypeID();

	KorryButton::KorryButton(const KorryButtonType buttonType, const gpio_num_t pin) : _buttonType(buttonType), _pin(pin) {

	}

	void KorryButton::setup() const {
		// GPIO
		gpio_config_t config {};
		config.pin_bit_mask = 1ULL << _pin;
		config.mode = GPIO_MODE_INPUT;
		config.pull_up_en = GPIO_PULLUP_DISABLE;
		config.pull_down_en = GPIO_PULLDOWN_ENABLE;
		config.intr_type = GPIO_INTR_ANYEDGE;

		gpio_config(&config);
	}

	void KorryButton::tick() {
		const auto newState = isPressed();

		const auto pushEvent = [this](const KorryEventType eventType) {
			KorryEvent event(_buttonType, eventType);
			RC::getInstance().application.pushEvent(&event);;
		};

		if (newState) {
			if (_oldState) {
				if (esp_timer_get_time() > _tickTime) {
					pushEvent(KorryEventType::tick);

					_tickTime = esp_timer_get_time() + tickEventInterval;
				}
			}
			else {
				pushEvent(KorryEventType::down);

				_oldState = newState;
				_tickTime = esp_timer_get_time() + tickEventDelay;
			}
		}
		else {
			if (_oldState) {
				pushEvent(KorryEventType::up);

				_oldState = false;
				_tickTime = 0;
			}
		}

		_oldState = newState;
	}

	bool KorryButton::isPressed() const {
		return gpio_get_level(_pin) == 1;
	}
}
