#pragma once

#include "driver/uart.h"
#include "constants.h"
#include "esp_log.h"
#include "hardware/GNSS/TinyGPS++.h"
#include <inttypes.h>
#include <span>
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
			void setup();
			void startReading();
			void setUpdateInterval(uint16_t intervalMs);
			void updateSystemsFromSettings() const;

			void setSystems(uint8_t systems) const;
			void updateUpdatingIntervalFromSettings();

			bool haveLocation() const;
			float getLatitudeRad() const;
			float getLongitudeRad() const;

			bool haveAltitude() const;
			float getAltitudeM();
			float getAltitudeTrendM() const;

			float getSpeedMps() const;
			float getSpeedTrendMps() const;
			float getCourseDeg() const;

			bool haveTime() const;
			uint8_t getTimeHours();
			uint8_t getTimeMinutes();
			uint8_t getTimeSeconds();

			uint32_t getSatellitesCount();
			bool isSatellitesCountEnough();

			float getWaypoint1BearingDeg() const;
			float getWaypoint1DistanceM() const;
			uint32_t getWaypoint1ETESec() const;
			float getWaypoint2BearingDeg() const;

			float getHDOP();
			std::span<char> getRXData();

		private:
			constexpr static uint16_t rxBufferSize = 2048;
			constexpr static uint16_t txBufferSize = 2048;
			constexpr static uint16_t queueSize = 10;
			constexpr static auto trendInterval = 5'000'000;

			char rxBuffer[rxBufferSize] {};
			int32_t rxDataLength = 0;

			TinyGPSPlus gps {};
			QueueHandle_t uartQueue {};

			int64_t tickTime = 0;

			// Location
			float latitudeRad = 0;
			float longitudeRad = 0;

			// Altitude
			float oldAltitudeM = 0;
			float altitudeTrendM = 0;

			// Waypoints
			float waypoint1BearingDeg = 0;
			float waypoint1DistanceM = 0;
			uint32_t waypoint1ETESec = 0;

			float waypoint2BearingDeg = 0;

			// Data updating
			float dataUpdatingPrevLatRad = -1;
			float dataUpdatingPrevLonRad = -1;
			int64_t dataUpdatingTime = 0;
			int64_t dataUpdatingZeroingTime = 0;

			// Speed
			float speedMps = 0;
			float speedTrendOldSpeedMps = 0;
			float speedTrendMps = 0;

			// Course
			float course = 0;

			// Simulation
			// constexpr static float simulationLatRadFrom = toRadians(59.804165104373745);
			// constexpr static float simulationLonRadFrom = toRadians(30.58331776426169);
			//
			// constexpr static float simulationLatRadTo = toRadians(59.80960977788406);
			// constexpr static float simulationLonRadTo = toRadians(30.59353048681725);

			constexpr static float simulationLatRadFrom = toRadians(59.81065170166242);
			constexpr static float simulationLonRadFrom = toRadians(30.562827043564145);

			constexpr static float simulationLatRadTo = toRadians(59.81183576340701);
			constexpr static float simulationLonRadTo = toRadians(30.564879512058923);

			constexpr static float simulationLatLonInterval = 20'000'000.f;

			bool simulationLatLonRev = false;
			float simulationAltitude = 0;
			float simulationLatRad = simulationLatRadFrom;
			float simulationLonRad = simulationLonRadFrom;

			static uint8_t computeCommandCRC(const char* data);
			static void sendCommand(const char* command);
			void tick();
			static bool isSimulationMode();
	};
}
