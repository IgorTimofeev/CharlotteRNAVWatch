#pragma once

#include "driver/uart.h"
#include "constants.h"
#include "esp_log.h"
#include "hardware/GNSS/TinyGPS++.h"
#include <inttypes.h>

namespace pizda {
	namespace GNSSSystem {
		enum : uint8_t {
			GPS = 0b00000001,
			GLONASS = 0b00000010,
			Galileo = 0b00000100,
			BeiDou = 0b00001000,
			QZSS = 0b00010000,
			SBAS = 0b00100000,
		};
	}

	class GNSS {
		public:
			explicit GNSS(const uart_port_t uartPort) : _uartPort(uartPort) {

			}
			
			void setup() {
				ESP_ERROR_CHECK(uart_driver_install(_uartPort, _rxBufferSize, _txBufferSize, _queueSize, &_uartQueue, 0));

				uart_config_t config {};
				config.baud_rate = 9600;
				config.data_bits = UART_DATA_8_BITS;
				config.parity  = UART_PARITY_DISABLE;
				config.stop_bits = UART_STOP_BITS_1;
				config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
				config.source_clk = UART_SCLK_DEFAULT;

				ESP_ERROR_CHECK(uart_param_config(_uartPort, &config));
				ESP_ERROR_CHECK(uart_set_pin(_uartPort, constants::uart::tx, constants::uart::rx, GPIO_NUM_NC, GPIO_NUM_NC));

				xTaskCreate(startTask, "GPS task", 4096, this, 1, nullptr);
			}

			void setGNSSSystem(const uint8_t flags) const {
				const auto buffer = new char[11];
				snprintf(buffer, 11, "PCAS04,%d", flags);

				sendCommand(buffer);

				delete[] buffer;
			}

			void setUpdateInterval(const uint16_t intervalMs) const {
				const auto buffer = new char[13];
				snprintf(buffer, 13, "MTK220,%d", intervalMs);

				sendCommand(buffer);

				delete[] buffer;
			}

			double getLatitude() {
				return _gps.location.lat();
			}

			double getLongitude() {
				return _gps.location.lng();
			}

			double getAltitudeFt() {
				return _gps.altitude.feet();
			}

			double getSpeedKt() {
				return _gps.speed.knots();
			}

			double getCourseDeg() {
				return _gps.course.deg();
			}

			uint8_t getTimeHours() {
				return _gps.time.hour();
			}

			uint8_t getTimeMinutes() {
				return _gps.time.minute();
			}

			uint8_t getTimeSeconds() {
				return _gps.time.second();
			}

			uint8_t getDateDay() {
				return _gps.date.day();
			}

			uint8_t getDateMonth() {
				return _gps.date.month();
			}

			uint16_t getDateYear() {
				return _gps.date.year();
			}

			uint32_t getSatellites() {
				return _gps.satellites.value();
			}

			double getHDOP() {
				return _gps.hdop.hdop();
			}

		private:
			uart_port_t _uartPort;
			TinyGPSPlus _gps {};
			QueueHandle_t _uartQueue {};

			constexpr static uint16_t _rxBufferSize = 2048;
			constexpr static uint16_t _txBufferSize = 2048;
			constexpr static uint16_t _queueSize = 10;

			static void startTask(void* args) {
				static_cast<GNSS*>(args)->tick();
			}

			static uint8_t computeCommandCRC(const char* data) {
				uint8_t crc = 0;

				while (*data != '\0') {
					crc ^= *data;
					data++;
				}

				return crc;
			}

			void sendCommand(const char* command) const {
				const auto crc = computeCommandCRC(command);
				const auto targetLen = 1 + strlen(command) + 1 + 2 + 2;

				const auto buffer = new char[targetLen + 1];
				snprintf(buffer, targetLen + 1, "$%s*%02X\r\n", command, crc);

				ESP_LOGI("GPS", "Sending command: %s", buffer);

				uart_write_bytes(_uartPort, buffer, targetLen);

				delete[] buffer;
			}

			[[noreturn]] void tick() {
				const auto data = new uint8_t[_rxBufferSize];

				while (true) {
					const auto bytesRead = uart_read_bytes(_uartPort, data, _rxBufferSize - 1, pdMS_TO_TICKS(100));

					if (bytesRead) {
						data[bytesRead] = '\0';

						ESP_LOGI("GPS", "Received:\n%s", (char *) data);

						auto ptr = data;

						while (*ptr) {
							_gps.encode(*ptr++);
						}
					}

					ESP_LOGI("GPS", "---------------- Processed data ----------------");
					ESP_LOGI("GPS", "Sat / HDOP: %lu, %lf", getSatellites(), getHDOP());
					ESP_LOGI("GPS", "Date / time: %d.%d.%d %d:%d:%d", getDateDay(), getDateMonth(), getDateYear(), getTimeHours(), getTimeMinutes(), getTimeSeconds());
					ESP_LOGI("GPS", "Lat / lon / alt: %f deg, %f deg, %f ft", getLatitude(), getLongitude(), getAltitudeFt());
					ESP_LOGI("GPS", "Speed / Course: %f kt, %f deg", getSpeedKt(), getCourseDeg());

					vTaskDelay(pdMS_TO_TICKS(1000));
				}
			}
	};
}
