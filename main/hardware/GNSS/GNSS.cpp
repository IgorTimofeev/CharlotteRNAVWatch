#include "GNSS.h"

#include "rc.h"

namespace pizda {
	void GNSS::setup() {
		ESP_ERROR_CHECK(uart_driver_install(constants::gnss::port, rxBufferSize, txBufferSize, queueSize, &uartQueue, 0));

		uart_config_t config {};
		config.baud_rate = 9600;
		config.data_bits = UART_DATA_8_BITS;
		config.parity  = UART_PARITY_DISABLE;
		config.stop_bits = UART_STOP_BITS_1;
		config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
		config.source_clk = UART_SCLK_DEFAULT;

		ESP_ERROR_CHECK(uart_param_config(constants::gnss::port, &config));

		ESP_ERROR_CHECK(uart_set_pin(
			constants::gnss::port,
			constants::gnss::tx,
			constants::gnss::rx,
			GPIO_NUM_NC,
			GPIO_NUM_NC
		));
	}

	void GNSS::startReading() {
		xTaskCreate(
			[](void* arg) {
				const auto instance = static_cast<GNSS*>(arg);

				// ReSharper disable once CppDFAEndlessLoop
				while (true) {
					instance->tick();

					vTaskDelay(pdMS_TO_TICKS(RC::getInstance().settings.GNSS.updatingIntervalMs));
				}
			},
			"GNSS",
			4096,
			this,
			1,
			nullptr
		);
	}

	void GNSS::setUpdateInterval(const uint16_t intervalMs) {
		const auto buffer = new char[15];
		snprintf(buffer, 15, "MTK220,%d", intervalMs);

		sendCommand(buffer);

		delete[] buffer;
	}

	void GNSS::updateSystemsFromSettings() const {
		const auto& settings = RC::getInstance().settings;

		uint8_t flags = 0;

		if (settings.GNSS.GPS)
			flags |= GNSSSystem::GPS;

		if (settings.GNSS.BeiDou)
			flags |= GNSSSystem::BDS;

		if (settings.GNSS.GLONASS)
			flags |= GNSSSystem::GLONASS;

		setSystems(flags);
	}

	void GNSS::setSystems(const uint8_t systems) const { // NOLINT(*-convert-member-functions-to-static)
		const auto buffer = new char[11];
		snprintf(buffer, 11, "PCAS04,%d", systems);

		sendCommand(buffer);

		delete[] buffer;
	}

	void GNSS::updateUpdatingIntervalFromSettings() {
		setUpdateInterval(RC::getInstance().settings.GNSS.updatingIntervalMs);
	}

	bool GNSS::haveLocation() const {
		return isSimulationMode() || gps.location.isValid();
	}

	float GNSS::getLatitudeRad() const {
		return isSimulationMode() ? simulationLatRad : latitudeRad;
	}

	float GNSS::getLongitudeRad() const {
		return isSimulationMode() ? simulationLonRad : longitudeRad;
	}

	bool GNSS::haveAltitude() const {
		return isSimulationMode() || gps.altitude.isValid();
	}

	float GNSS::getAltitudeM() {
		return isSimulationMode() ? simulationAltitude : static_cast<float>(gps.altitude.meters());
	}

	float GNSS::getAltitudeTrendM() const {
		return altitudeTrendM;
	}

	float GNSS::getSpeedMps() const {
		return speedMps;
	}

	float GNSS::getSpeedTrendMps() const {
		return speedTrendMps;
	}

	float GNSS::getCourseDeg() const {
		return course;
	}

	bool GNSS::haveTime() const {
		return isSimulationMode() || gps.time.isValid();
	}

	uint8_t GNSS::getTimeHours() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 / 3600 : gps.time.hour();
	}

	uint8_t GNSS::getTimeMinutes() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 / 60 : gps.time.minute();
	}

	uint8_t GNSS::getTimeSeconds() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 : gps.time.second();
	}

	uint32_t GNSS::getSatellitesCount() {
		return isSimulationMode() ? 9 : gps.satellites.value();
	}

	bool GNSS::isSatellitesCountEnough() {
		return getSatellitesCount() >= 3;
	}

	float GNSS::getHDOP() {
		return isSimulationMode() ? 10 : static_cast<float>(gps.hdop.hdop());
	}

	std::span<char> GNSS::getRXData() {
		return std::span(rxBuffer, rxDataLength);
	}

	float GNSS::getWaypoint1BearingDeg() const {
		return waypoint1BearingDeg;
	}

	float GNSS::getWaypoint1DistanceM() const {
		return waypoint1DistanceM;
	}

	uint32_t GNSS::getWaypoint1ETESec() const {
		return waypoint1ETESec;
	}

	float GNSS::getWaypoint2BearingDeg() const {
		return waypoint2BearingDeg;
	}

	uint8_t GNSS::computeCommandCRC(const char* data) {
		uint8_t crc = 0;

		while (*data != '\0') {
			crc ^= *data;
			data++;
		}

		return crc;
	}

	void GNSS::sendCommand(const char* command) {
		const auto crc = computeCommandCRC(command);
		const auto targetLen = 1 + strlen(command) + 1 + 2 + 2;

		const auto buffer = new char[targetLen + 1];
		snprintf(buffer, targetLen + 1, "$%s*%02X\r\n", command, crc);

		ESP_LOGI("GNSS", "Sending command: %s", buffer);

		uart_write_bytes(constants::gnss::port, buffer, targetLen);

		delete[] buffer;
	}

	void GNSS::tick() {
		rxDataLength = uart_read_bytes(constants::gnss::port, rxBuffer, rxBufferSize - 1, pdMS_TO_TICKS(100));

		if (!rxDataLength)
			return;

		rxBuffer[rxDataLength] = '\0';

		auto bufferPtr = rxBuffer;

		while (*bufferPtr)
			gps.encode(*bufferPtr++);

		const auto& rc = RC::getInstance();
		const auto& navWaypoint = rc.settings.nav.waypoints[rc.settings.nav.waypoint1Index];
		const auto& bearingWaypoint = rc.settings.nav.waypoints[rc.settings.nav.waypoint2Index];
		const auto deltaTimeUsF = static_cast<float>(esp_timer_get_time() - tickTime);

		// Location
		latitudeRad = toRadians(gps.location.lat());
		longitudeRad = toRadians(gps.location.lng());

		if (isSimulationMode()) {
			// Lat/lon
			// distance - 10 sec
			// x - deltaTime
			const auto factor = deltaTimeUsF / simulationLatLonInterval * static_cast<float>(random(80, 100)) / 100.f;
			simulationLatRad += (simulationLatLonRev ? simulationLatRadFrom - simulationLatRadTo : simulationLatRadTo - simulationLatRadFrom) * factor;
			simulationLonRad += (simulationLatLonRev ? simulationLonRadFrom - simulationLonRadTo : simulationLonRadTo - simulationLonRadFrom) * factor;

			if ((!simulationLatLonRev && simulationLatRad > simulationLatRadTo) || (simulationLatLonRev && simulationLatRad < simulationLatRadFrom)) {
				simulationLatLonRev = !simulationLatLonRev;
			}

			// Altitude
			simulationAltitude += static_cast<float>(random(
				static_cast<int32_t>(Units::convertDistance(6, DistanceUnit::foot, DistanceUnit::meter)),
				static_cast<int32_t>(Units::convertDistance(12, DistanceUnit::foot, DistanceUnit::meter))
			));

			if (simulationAltitude >= Units::convertDistance(150, DistanceUnit::foot, DistanceUnit::meter))
				simulationAltitude = 0;
		}

		// First ever location update
		if (dataUpdatingPrevLatRad < 0) {
			dataUpdatingPrevLatRad = getLatitudeRad();
			dataUpdatingPrevLonRad = getLongitudeRad();
		}

		// Distance
		const auto samplesDistanceM = GeographicCoordinates::getDistance(
			dataUpdatingPrevLatRad,
			dataUpdatingPrevLonRad,
			getLatitudeRad(),
			getLongitudeRad()
		);

		// Distance between 2 samples is enough, data should be updated
		if (samplesDistanceM >= static_cast<float>(rc.settings.GNSS.dataSamplingDistanceM)) {
			const auto dataUpdatingDeltaTimeUsF = static_cast<float>(esp_timer_get_time() - dataUpdatingTime);

			// Speed
			// samplesDistanceM - interval us
			// x - 1'000'000 us
			speedMps = samplesDistanceM * 1'000'000.f / dataUpdatingDeltaTimeUsF;

			// Speed trend
			// deltasPeed - interval us
			// x - trendInterval us
			speedTrendMps = (getSpeedMps() - speedTrendOldSpeedMps) * static_cast<float>(trendInterval) / dataUpdatingDeltaTimeUsF;
			speedTrendOldSpeedMps = getSpeedMps();

			// Course
			course = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
				dataUpdatingPrevLatRad,
				dataUpdatingPrevLonRad,
				getLatitudeRad(),
				getLongitudeRad()
			)));

			dataUpdatingPrevLatRad = getLatitudeRad();
			dataUpdatingPrevLonRad = getLongitudeRad();

			dataUpdatingTime = esp_timer_get_time();
			dataUpdatingZeroingTime = dataUpdatingTime + rc.settings.GNSS.dataSamplingZeroingIntervalMs * 1'000;
		}
		else {
			// If too much time has passed since last update, data should be zeroed out
			if (esp_timer_get_time() >= dataUpdatingZeroingTime) {
				// Speed
				speedMps = 0;

				// Speed trend
				speedTrendMps = 0;
				speedTrendOldSpeedMps = 0;

				dataUpdatingTime = esp_timer_get_time();
				dataUpdatingZeroingTime = dataUpdatingTime + rc.settings.GNSS.dataSamplingZeroingIntervalMs * 1'000;
			}
		}

		// Altitude trend
		altitudeTrendM = (getAltitudeM() - oldAltitudeM) * static_cast<float>(trendInterval) / deltaTimeUsF;
		oldAltitudeM = getAltitudeM();

		// Waypoint 1
		waypoint1BearingDeg = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
			getLatitudeRad(),
			getLongitudeRad(),
			navWaypoint.geographicCoordinates.getLatitude(),
			navWaypoint.geographicCoordinates.getLongitude()
		)));

		waypoint1DistanceM = GeographicCoordinates::getDistance(
			getLatitudeRad(),
			getLongitudeRad(),
			navWaypoint.geographicCoordinates.getLatitude(),
			navWaypoint.geographicCoordinates.getLongitude()
		);

		waypoint1ETESec = speedMps > 0 ? static_cast<uint32_t>(waypoint1DistanceM / speedMps * 3600) : 0;

		// Waypoint 2
		waypoint2BearingDeg = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
			getLatitudeRad(),
			getLongitudeRad(),
			bearingWaypoint.geographicCoordinates.getLatitude(),
			bearingWaypoint.geographicCoordinates.getLongitude()
		)));

		// ESP_LOGI("GNSS", "---------------- Processed data ----------------");
		// ESP_LOGI("GNSS", "Sat / HDOP: %lu, %lf", getSatellites(), getHDOP());
		// ESP_LOGI("GNSS", "Date / time: %d.%d.%d %d:%d:%d", getDateDay(), getDateMonth(), getDateYear(), getTimeHours(), getTimeMinutes(), getTimeSeconds());
		// ESP_LOGI("GNSS", "Lat / lon / alt: %f deg, %f deg, %f m", toDegrees(getLatitudeRad()), toDegrees(getLongitudeRad()), getAltitudeM());
		// ESP_LOGI("GNSS", "Speed / Course: %f mps, %f deg", getSpeedMps(), getCourseDeg());

		tickTime = esp_timer_get_time();
	}

	bool GNSS::isSimulationMode() {
		return RC::getInstance().settings.GNSS.simulation;
	}
}
