#pragma once

#include "driver/uart.h"
#include "constants.h"
#include "esp_log.h"
#include "hardware/GNSS/TinyGPS++.h"
#include <inttypes.h>
#include <YOBA/main.h>
#include <YOBAUnits.h>

namespace pizda {
	using namespace YOBA;

	namespace GNSSSystem {
		enum : uint8_t {
			GPS =     0b000'0001,
			BDS =     0b000'0010,
			GLONASS = 0b000'0100
		};
	}

	class GNSS {
		public:
			explicit GNSS(const uart_port_t uartPort) : uartPort(uartPort) {

			}

			void setup() {
				ESP_ERROR_CHECK(uart_driver_install(uartPort, rxBufferSize, txBufferSize, queueSize, &uartQueue, 0));

				uart_config_t config {};
				config.baud_rate = 9600;
				config.data_bits = UART_DATA_8_BITS;
				config.parity  = UART_PARITY_DISABLE;
				config.stop_bits = UART_STOP_BITS_1;
				config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
				config.source_clk = UART_SCLK_DEFAULT;

				ESP_ERROR_CHECK(uart_param_config(uartPort, &config));
				ESP_ERROR_CHECK(uart_set_pin(uartPort, constants::uart::tx, constants::uart::rx, GPIO_NUM_NC, GPIO_NUM_NC));

				xTaskCreate(
					[](void* arg) {
						while (true) {
							static_cast<GNSS*>(arg)->tick();
							vTaskDelay(pdMS_TO_TICKS(tickInterval / 1000));
						}
					},
					"GPS task",
					4096,
					this,
					1,
					nullptr
				);
			}

			void setSystems(const uint8_t systems) const {
				const auto buffer = new char[11];
				snprintf(buffer, 11, "PCAS04,%d", systems);

				sendCommand(buffer);

				delete[] buffer;
			}

			void setUpdateInterval(const uint16_t intervalMs) const {
				const auto buffer = new char[13];
				snprintf(buffer, 13, "MTK220,%d", intervalMs);

				sendCommand(buffer);

				delete[] buffer;
			}

			bool haveLocation() const {
				return simulationMode || gps.location.isValid();
			}

			float getLatitudeRad() {
				return simulationMode ? simulationLatitude : gps.location.lat();
			}

			float getLongitudeRad() {
				return simulationMode ? simulationLongitude : gps.location.lng();
			}

			bool haveAltitude() const {
				return simulationMode || gps.altitude.isValid();
			}

			float getAltitudeM() {
				return simulationMode ? simulationAltitude : gps.altitude.meters();
			}

			float getAltitudeTrendM() const {
				return altitudeTrend;
			}

			bool haveSpeed() const {
				return simulationMode || gps.speed.isValid();
			}

			float getSpeedMps() {
				return simulationMode ? simulationSpeed : gps.speed.mps();
			}

			float getSpeedTrendKt() const {
				return speedTrend;
			}

			bool haveCourse() const {
				return simulationMode || gps.course.isValid();
			}

			float getCourseDeg() {
				return normalizeAngle360(gps.course.value());
			}

			float getComputedCourseDeg() const {
				return courseComputed;
			}

			bool haveTime() const {
				return simulationMode || gps.time.isValid();
			}

			uint8_t getTimeHours() {
				return simulationMode ? 12 : gps.time.hour();
			}

			uint8_t getTimeMinutes() {
				return simulationMode ? 0 : gps.time.minute();
			}

			uint8_t getTimeSeconds() {
				return simulationMode ? 0 : gps.time.second();
			}

			uint8_t getDateDay() {
				return simulationMode ? 18 : gps.date.day();
			}

			uint8_t getDateMonth() {
				return simulationMode ? 9 : gps.date.month();
			}

			uint16_t getDateYear() {
				return simulationMode ? 1994 : gps.date.year();
			}

			uint32_t getSatellites() {
				return simulationMode ? 19 : gps.satellites.value();
			}

			float getHDOP() {
				return simulationMode ? 10 : gps.hdop.hdop();
			}

			bool getSimulationMode() const {
				return simulationMode;
			}
			
			void setSimulationMode(const bool value) {
				simulationMode = value;
			}

		private:
			constexpr static uint16_t rxBufferSize = 2048;
			constexpr static uint16_t txBufferSize = 2048;
			constexpr static uint16_t queueSize = 10;
			constexpr static auto tickInterval = 1'000'000;
			constexpr static auto trendInterval = 5'000'000;
			constexpr static auto courseTickInterval = 3'000'000;

			uart_port_t uartPort;
			TinyGPSPlus gps {};
			QueueHandle_t uartQueue {};
			char rxBuffer[rxBufferSize] {};

			bool simulationMode = false;

			float courseComputed = 0;
			int64_t courseTickTime = 0;
			float coursePrevLatitude = 0;
			float coursePrevLongitude = 0;

			float simulationAltitude = 0;
			float simulationSpeed = 0;
			float simulationLatitude = -1;
			float simulationLongitude = -1;

			float oldAltitude = 0;
			float altitudeTrend = 0;

			float oldSpeed = 0;
			float speedTrend = 0;

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

				ESP_LOGI("GNSS", "Sending command: %s", buffer);

				uart_write_bytes(uartPort, buffer, targetLen);

				delete[] buffer;
			}

			void tick() {
				const auto bytesRead = uart_read_bytes(uartPort, rxBuffer, rxBufferSize - 1, pdMS_TO_TICKS(100));

				if (bytesRead) {
					rxBuffer[bytesRead] = '\0';

					auto bufferPtr = rxBuffer;

					while (*bufferPtr)
						gps.encode(*bufferPtr++);

					if (simulationMode) {
						// Lat/lon
						constexpr static float simulationLatFrom = toRadians(59.804165104373745);
						constexpr static float simulationLonFrom = toRadians(30.58331776426169);

						constexpr static float simulationLatTo = toRadians(59.80960977788406);
						constexpr static float simulationLonTo = toRadians(30.59353048681725);

						constexpr static float simulationLatLonTime = 20'000'000.f;

						if (simulationLatitude < 0) {
							simulationLatitude = simulationLatFrom;
							simulationLongitude = simulationLonFrom;
						}

						// distance - 10 sec
						// x - deltaTime
						simulationLatitude += (simulationLatTo - simulationLatFrom) * static_cast<float>(tickInterval) / simulationLatLonTime;
						simulationLongitude += (simulationLonTo - simulationLonFrom) * static_cast<float>(tickInterval) / simulationLatLonTime;

						if (simulationLatitude > simulationLatTo) {
							simulationLatitude = simulationLatFrom;
							simulationLongitude = simulationLonFrom;
						}

						// Speed
						simulationSpeed += YOBA::random(
							static_cast<int32_t>(Units::convertSpeed(1, SpeedUnit::knot, SpeedUnit::meterPerSecond)),
							static_cast<int32_t>(Units::convertSpeed(2, SpeedUnit::knot, SpeedUnit::meterPerSecond))
						);

						if (simulationSpeed >= Units::convertSpeed(20, SpeedUnit::knot, SpeedUnit::meterPerSecond))
							simulationSpeed = 0;

						// Altitude
						simulationAltitude += YOBA::random(
							static_cast<int32_t>(Units::convertDistance(6, DistanceUnit::foot, DistanceUnit::meter)),
							static_cast<int32_t>(Units::convertDistance(12, DistanceUnit::foot, DistanceUnit::meter))
						);

						if (simulationAltitude >= Units::convertDistance(150, DistanceUnit::foot, DistanceUnit::meter))
							simulationAltitude = 0;
					}

					// Course
					if (esp_timer_get_time() > courseTickTime) {
						courseComputed = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
							coursePrevLatitude,
							coursePrevLongitude,
							getLatitudeRad(),
							getLongitudeRad()
						)));

						coursePrevLatitude = getLatitudeRad();
						coursePrevLongitude = getLongitudeRad();

						courseTickTime = esp_timer_get_time() + courseTickInterval;
					}

					// Trends
					if (haveSpeed()) {
						speedTrend = (getSpeedMps() - oldSpeed) * trendInterval / tickInterval;
						oldSpeed = getSpeedMps();
					}
					else {
						speedTrend = oldSpeed;
					}

					if (haveAltitude()) {
						altitudeTrend = (getAltitudeM() - oldAltitude) * trendInterval / tickInterval;
						oldAltitude = getAltitudeM();
					}
					else {
						altitudeTrend = oldAltitude;
					}

					// ESP_LOGI("GNSS", "---------------- Processed data ----------------");
					// ESP_LOGI("GNSS", "Sat / HDOP: %lu, %lf", getSatellites(), getHDOP());
					// ESP_LOGI("GNSS", "Date / time: %d.%d.%d %d:%d:%d", getDateDay(), getDateMonth(), getDateYear(), getTimeHours(), getTimeMinutes(), getTimeSeconds());
					// ESP_LOGI("GNSS", "Lat / lon / alt: %f deg, %f deg, %f m", toDegrees(getLatitudeRad()), toDegrees(getLongitudeRad()), getAltitudeM());
					// ESP_LOGI("GNSS", "Speed / Course: %f mps, %f deg", getSpeedMps(), getCourseDeg());
				}
			}
	};
}
