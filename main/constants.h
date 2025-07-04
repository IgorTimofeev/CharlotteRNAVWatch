#pragma once

#include "driver/ledc.h"
#include "cstdint"
#include "esp_adc/adc_oneshot.h"
#include "driver/uart.h"

namespace pizda {
	class constants {
		public:
			class spi {
				public:
					constexpr static gpio_num_t mosi = GPIO_NUM_6;
					constexpr static gpio_num_t miso = GPIO_NUM_5;
					constexpr static gpio_num_t sck = GPIO_NUM_4;
			};

			class uart {
				public:
					constexpr static uart_port_t port = UART_NUM_1;
					constexpr static gpio_num_t rx = GPIO_NUM_20;
					constexpr static gpio_num_t tx = GPIO_NUM_21;
			};

			class screen {
				public:
					constexpr static gpio_num_t slaveSelect = GPIO_NUM_1;
					constexpr static gpio_num_t dataCommand = GPIO_NUM_2;
					constexpr static gpio_num_t reset = GPIO_NUM_3;
					constexpr static uint32_t frequency = 40'000'000;
			};

			class application {
				public:
					constexpr static uint32_t mainTickInterval = 1'000'000 / 30;
					constexpr static uint32_t interpolationTickInterval = 1'000'000 / 30;
			};
	};
}
