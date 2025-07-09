#include "korryButton.h"
#include "rc.h"

namespace pizda {
	KorryButtonEvent::KorryButtonEvent(const KorryButtonType type, const uint32_t time): Event(typeID), _type(type), _time(time) {

	}

	KorryButtonType KorryButtonEvent::getType() const {
		return _type;
	}

	uint32_t KorryButtonEvent::getTime() const {
		return _time;
	}

	uint16_t KorryButtonEvent::typeID = registerTypeID();

	KorryButton::KorryButton(const KorryButtonType type, const gpio_num_t pin) : _type(type), _pin(pin) {

	}

	void KorryButton::setup() {
		// GPIO
		gpio_config_t config{};
		config.pin_bit_mask = 1ULL << _pin;
		config.mode = GPIO_MODE_INPUT;
		config.pull_up_en = GPIO_PULLUP_DISABLE;
		config.pull_down_en = GPIO_PULLDOWN_ENABLE;
		config.intr_type = GPIO_INTR_ANYEDGE;

		gpio_config(&config);

		// Interrupt
		gpio_install_isr_service(0);
		gpio_isr_handler_add(_pin, onInterrupt, this);
	}

	void KorryButton::tick() {
		if (_pressTime <= 0)
			return;

		auto& rc = RC::getInstance();

		KorryButtonEvent event(_type, _pressTime);

		rc.application.pushEvent(&event);

		_pressTime = 0;
	}

	void KorryButton::onInterrupt(void* arg) {
		const auto instance = static_cast<KorryButton*>(arg);
		const auto pressed = gpio_get_level(instance->_pin) > 0;

		// Was not pressed or press was not handled in tick()
		if (instance->_pressTime >= 0) {
			if (pressed) {
				instance->_pressTime = -esp_timer_get_time();
			}
		}
		// Was pressed
		else {
			if (!pressed) {
				instance->_pressTime = esp_timer_get_time() - (-instance->_pressTime);
			}
		}
	}
}
