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
			GPS =     0b00000001,
			GLONASS = 0b00000010,
			Galileo = 0b00000100,
			BeiDou =  0b00001000,
			QZSS =    0b00010000,
			SBAS =    0b00100000,
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

			bool haveLocation() const {
				return gps.location.isValid();
			}

			float getLatitudeRad() {
				return simulationMode ? simulationLatitude : gps.location.lat();
			}

			float getLongitudeRad() {
				return simulationMode ? simulationLongitude : gps.location.lng();
			}

			bool haveAltitude() const {
				return gps.altitude.isValid();
			}

			float getAltitudeM() {
				return simulationMode ? simulationAltitude : gps.altitude.meters();
			}

			float getAltitudeTrendM() const {
				return altitudeTrend;
			}

			bool haveSpeed() const {
				return gps.speed.isValid();
			}

			float getSpeedMps() {
				return simulationMode ? simulationSpeed : gps.speed.mps();
			}

			float getSpeedTrendKt() const {
				return speedTrend;
			}

			bool haveCourse() const {
				return gps.course.isValid();
			}

			float getCourseDeg() {
				return simulationMode ? simulationCourse : gps.course.value();
			}

			bool haveTime() const {
				return gps.time.isValid();
			}

			uint8_t getTimeHours() {
				return gps.time.hour();
			}

			uint8_t getTimeMinutes() {
				return gps.time.minute();
			}

			uint8_t getTimeSeconds() {
				return gps.time.second();
			}

			uint8_t getDateDay() {
				return gps.date.day();
			}

			uint8_t getDateMonth() {
				return gps.date.month();
			}

			uint16_t getDateYear() {
				return gps.date.year();
			}

			uint32_t getSatellites() {
				return gps.satellites.value();
			}

			float getHDOP() {
				return gps.hdop.hdop();
			}

			bool isSimulationMode() const {
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

			uart_port_t uartPort;
			TinyGPSPlus gps {};
			QueueHandle_t uartQueue {};
			char rxBuffer[rxBufferSize];

			bool simulationMode = false;
			float simulationCourse = 0;
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

					ESP_LOGI("GNSS", "bytesRead: %d, data \n: %s", bytesRead, rxBuffer);

					const char* NMEAData;

					if (simulationMode) {
						NMEAData =
							"$GPRMC,112019.950,A,5950.225256,N,03035.104663,E,22.9,37.0,250625,,,*2C\r\n"
							"$IIVHW,37.0,T,37.0,M,22.9,N,42.3,K*59\r\n"
							"$GPVTG,37.0,T,37.0,M,22.9,N,42.3,K*42\r\n"
							"$IIHDT,37.0,T*16"
							"$GPGLL,5950.225256,N,03035.104663,E,112019.950,A*3C\r\n"
							"$GPGGA,112019.950,5950.225256,N,03035.104663,E,1,4,0.7,630.0,M,,,,*31\r\n"
							"$GPGSA,A,3,8,11,15,22,,,,,,,,,0.5,0.7,1.1*0C\r\n"
							"$GPZDA,112019.950,25,06,2025,03,00*57\r\n"
							"$IIVBW,22.1,22.5,A,22.0,22.8,A,22.4,A,22.6,A*4D\r\n"
							"!AIVDO,1,1,,A,17PaewhP3TR<0N<R?C;1LQ:V0000,0*27\r\n"
							"!AIVDM,1,1,,A,17PaewhP3TR<0N<R?C;1LQ:V0000,0*25\r\n"
							"$INWPL,5949.665751,N,03034.126540,E,wpt*2F\r\n"
							"!AIVDO,2,1,9,A,57Paewh00001<To7;?@plD5<Tl0000000000000U1@:551dcD2TnA3QF,0*4A\r\n"
							"!AIVDO,2,2,9,A,@00000000000002,2*5D\r\n"
							"!AIVDM,2,1,9,A,57Paewh00001<To7;?@plD5<Tl0000000000000U1@:551dcD2TnA3QF,0*48\r\n"
							"!AIVDM,2,2,9,A,@00000000000002,2*5F\r\n"
							"$IIRPM,E,1,3450.0,10.5,A*50\r\n"
							"$IIRPM,E,2,0,10.5,A*7F\r\n"
							"$IIAPB,V,V,,R,N,,,,T,,,T,,T,N*79\r\n"
							"$GPRMB,V,,R,,,,,,,,,,,N*00\r\n";
					}
					else {
						NMEAData = rxBuffer;
					}

					while (*NMEAData)
						gps.encode(*NMEAData++);

					if (simulationMode) {
						// Lat/lon
						constexpr static float simulationLatFrom = toRadians(59.79363656610644);
						constexpr static float simulationLonFrom = toRadians(30.56466522806966);

						constexpr static float simulationLatTo = toRadians(59.82238390534588);
						constexpr static float simulationLonTo = simulationLonFrom;

						constexpr static float simulationLatLonTime = 10'000'000.f;

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

						// Course
						// simulationCourse += YOBA::random(4, 8);
						//
						// if (simulationCourse >= 360)
						// 	simulationCourse = 0;

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

					ESP_LOGI("GNSS", "---------------- Processed data ----------------");
					ESP_LOGI("GNSS", "Sat / HDOP: %lu, %lf", getSatellites(), getHDOP());
					ESP_LOGI("GNSS", "Date / time: %d.%d.%d %d:%d:%d", getDateDay(), getDateMonth(), getDateYear(), getTimeHours(), getTimeMinutes(), getTimeSeconds());
					ESP_LOGI("GNSS", "Lat / lon / alt: %f deg, %f deg, %f m", toDegrees(getLatitudeRad()), toDegrees(getLongitudeRad()), getAltitudeM());
					ESP_LOGI("GNSS", "Speed / Course: %f mps, %f deg", getSpeedMps(), getCourseDeg());
				}
			}
	};
}
