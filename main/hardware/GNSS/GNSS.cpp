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
		ESP_ERROR_CHECK(uart_set_pin(constants::gnss::port, constants::gnss::tx, constants::gnss::rx, GPIO_NUM_NC, GPIO_NUM_NC));
	}

	void GNSS::startReading() {
		xTaskCreate(
			[](void* arg) {
				while (true) {
					static_cast<GNSS*>(arg)->readAndParse();
					vTaskDelay(pdMS_TO_TICKS(RC::getInstance().settings.GNSS.tickIntervalUs / 1000));
				}
			},
			"GPS task",
			4096,
			this,
			1,
			nullptr
		);
	}

	void GNSS::updateSystemsFromSettings() const {
		const auto& settings = RC::getInstance().settings;

		uint8_t flags = 0;

		if (settings.GNSS.GPS)
			flags |= GNSSSystem::GPS;

		if (settings.GNSS.BDS)
			flags |= GNSSSystem::BDS;

		if (settings.GNSS.GLONASS)
			flags |= GNSSSystem::GLONASS;

		setSystems(flags);
	}

	uint16_t GNSS::getDataUpdatingDistanceM() const {
		return dataUpdatingDistanceM;
	}

	void GNSS::setDataUpdatingDistanceM(const uint16_t value) {
		this->dataUpdatingDistanceM = value;
	}

	void GNSS::setSystems(const uint8_t systems) const {
		const auto buffer = new char[11];
		snprintf(buffer, 11, "PCAS04,%d", systems);

		sendCommand(buffer);

		delete[] buffer;
	}

	void GNSS::setUpdateInterval(const uint16_t intervalMs) const {
		const auto buffer = new char[13];
		snprintf(buffer, 13, "MTK220,%d", intervalMs);

		sendCommand(buffer);

		delete[] buffer;
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
		return isSimulationMode() ? simulationAltitude : gps.altitude.meters();
	}

	float GNSS::getAltitudeTrendM() const {
		return altitudeTrendM;
	}

	bool GNSS::haveSpeed() const {
		return isSimulationMode() || gps.speed.isValid();
	}

	float GNSS::getSpeedMps() const {
		return speedMps;
	}

	float GNSS::getSpeedTrendMps() const {
		return speedTrendMps;
	}

	bool GNSS::haveCourse() const {
		return isSimulationMode() || gps.course.isValid();
	}

	float GNSS::getCourseDeg() {
		return normalizeAngle360(gps.course.value());
	}

	float GNSS::getComputedCourseDeg() const {
		return course;
	}

	bool GNSS::haveTime() const {
		return isSimulationMode() || gps.time.isValid();
	}

	uint8_t GNSS::getTimeHours() {
		return isSimulationMode() ? 12 : gps.time.hour();
	}

	uint8_t GNSS::getTimeMinutes() {
		return isSimulationMode() ? 0 : gps.time.minute();
	}

	uint8_t GNSS::getTimeSeconds() {
		return isSimulationMode() ? 0 : gps.time.second();
	}

	uint32_t GNSS::getSatellitesCount() {
		return isSimulationMode() ? 19 : gps.satellites.value();
	}

	bool GNSS::isSatellitesCountEnough() {
		return getSatellitesCount() >= 3;
	}

	float GNSS::getHDOP() {
		return isSimulationMode() ? 10 : gps.hdop.hdop();
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

	void GNSS::readAndParse() {
		const auto bytesRead = uart_read_bytes(constants::gnss::port, rxBuffer, rxBufferSize - 1, pdMS_TO_TICKS(100));

		if (!bytesRead)
			return;

		rxBuffer[bytesRead] = '\0';

		auto bufferPtr = rxBuffer;

		while (*bufferPtr)
			gps.encode(*bufferPtr++);

		const auto& rc = RC::getInstance();
		const auto& navWaypoint = rc.settings.nav.waypoints[rc.settings.nav.waypoint1Index];
		const auto& bearingWaypoint = rc.settings.nav.waypoints[rc.settings.nav.waypoint2Index];

		// Location
		latitudeRad = toRadians(gps.location.lat());
		longitudeRad = toRadians(gps.location.lng());

		// First ever location update
		if (dataUpdatingPrevLatRad < 0) {
			dataUpdatingPrevLatRad = latitudeRad;
			dataUpdatingPrevLonRad = longitudeRad;
		}

		if (isSimulationMode()) {
			// Lat/lon
			// distance - 10 sec
			// x - deltaTime
			const auto factor = static_cast<float>(rc.settings.GNSS.tickIntervalUs) / simulationLatLonInterval * random(80, 100) / 100.f;
			simulationLatRad += (simulationLatLonRev ? simulationLatRadFrom - simulationLatRadTo : simulationLatRadTo - simulationLatRadFrom) * factor;
			simulationLonRad += (simulationLatLonRev ? simulationLonRadFrom - simulationLonRadTo : simulationLonRadTo - simulationLonRadFrom) * factor;

			if ((!simulationLatLonRev && simulationLatRad > simulationLatRadTo) || (simulationLatLonRev && simulationLatRad < simulationLatRadFrom)) {
				simulationLatLonRev = !simulationLatLonRev;
			}

			// Altitude
			simulationAltitude += YOBA::random(
				static_cast<int32_t>(Units::convertDistance(6, DistanceUnit::foot, DistanceUnit::meter)),
				static_cast<int32_t>(Units::convertDistance(12, DistanceUnit::foot, DistanceUnit::meter))
			);

			if (simulationAltitude >= Units::convertDistance(150, DistanceUnit::foot, DistanceUnit::meter))
				simulationAltitude = 0;
		}

		// Distance
		const auto samplesDistanceM = GeographicCoordinates::getDistance(
			dataUpdatingPrevLatRad,
			dataUpdatingPrevLonRad,
			getLatitudeRad(),
			getLongitudeRad()
		);

		// Distance between 2 samples is enough, data should be updated
		if (samplesDistanceM > dataUpdatingDistanceM) {
			// Speed
			// samplesDistanceM - tickInterval us
			// x - 1'000'000 us
			speedMps = samplesDistanceM * 1'000'000 / rc.settings.GNSS.tickIntervalUs;

			// Course
			course = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
				dataUpdatingPrevLatRad,
				dataUpdatingPrevLonRad,
				getLatitudeRad(),
				getLongitudeRad()
			)));

			// Speed trend
			if (haveSpeed()) {
				speedTrendMps = (getSpeedMps() - oldSpeedMps) * trendInterval / rc.settings.GNSS.tickIntervalUs;
				oldSpeedMps = getSpeedMps();
			}
			else {
				speedTrendMps = oldSpeedMps;
			}

			dataUpdatingPrevLatRad = getLatitudeRad();
			dataUpdatingPrevLonRad = getLongitudeRad();
		}

		// Altitude trend
		if (haveAltitude()) {
			altitudeTrendM = (getAltitudeM() - oldAltitudeM) * trendInterval / rc.settings.GNSS.tickIntervalUs;
			oldAltitudeM = getAltitudeM();
		}
		else {
			altitudeTrendM = oldAltitudeM;
		}

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
	}

	bool GNSS::isSimulationMode() {
		return RC::getInstance().settings.GNSS.simulation;
	}
}
