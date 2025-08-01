#include "AHRS.h"

#include "rc.h"

namespace pizda {
	void AHRS::setup() {
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

		xTaskCreate(
			[](void* arg) {
				const auto instance = static_cast<AHRS*>(arg);

				// ReSharper disable once CppDFAEndlessLoop
				while (true) {
					instance->tick();

					vTaskDelay(pdMS_TO_TICKS(RC::getInstance().settings.AHRS.updatingIntervalMs));
				}
			},
			"AHRS",
			4096,
			this,
			1,
			nullptr
		);
	}

	void AHRS::setUpdateInterval(const uint16_t intervalMs) {
		const auto buffer = new char[15];
		snprintf(buffer, 15, "MTK220,%d", intervalMs);

		sendCommand(buffer);

		delete[] buffer;
	}

	void AHRS::updateSystemsFromSettings() const {
		const auto& settings = RC::getInstance().settings;

		uint8_t flags = 0;

		if (settings.AHRS.GPS)
			flags |= GNSSSystem::GPS;

		if (settings.AHRS.BeiDou)
			flags |= GNSSSystem::BDS;

		if (settings.AHRS.GLONASS)
			flags |= GNSSSystem::GLONASS;

		setSystems(flags);
	}

	void AHRS::setSystems(const uint8_t systems) const { // NOLINT(*-convert-member-functions-to-static)
		const auto buffer = new char[11];
		snprintf(buffer, 11, "PCAS04,%d", systems);

		sendCommand(buffer);

		delete[] buffer;
	}

	void AHRS::updateUpdatingIntervalFromSettings() {
		setUpdateInterval(RC::getInstance().settings.AHRS.updatingIntervalMs);
	}

	bool AHRS::haveLocation() const {
		return isSimulationMode() || gps.location.isValid();
	}

	float AHRS::getLatitudeRad() const {
		return isSimulationMode() ? simulationLatRad : latitudeRad;
	}

	float AHRS::getLongitudeRad() const {
		return isSimulationMode() ? simulationLonRad : longitudeRad;
	}

	bool AHRS::haveAltitude() const {
		return isSimulationMode() || gps.altitude.isValid();
	}

	float AHRS::getAltitudeM() {
		return isSimulationMode() ? simulationAltitude : static_cast<float>(gps.altitude.meters());
	}

	float AHRS::getAltitudeTrendM() const {
		return altitudeTrendM;
	}

	float AHRS::getSpeedMps() const {
		return speedMps;
	}

	float AHRS::getSpeedTrendMps() const {
		return speedTrendMps;
	}

	float AHRS::getCourseDeg() const {
		return course;
	}

	bool AHRS::haveTime() const {
		return isSimulationMode() || gps.time.isValid();
	}

	uint8_t AHRS::getTimeHours() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 / 3600 : gps.time.hour();
	}

	uint8_t AHRS::getTimeMinutes() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 / 60 : gps.time.minute();
	}

	uint8_t AHRS::getTimeSeconds() {
		return isSimulationMode() ? esp_timer_get_time() / 1'000'000 : gps.time.second();
	}

	uint32_t AHRS::getSatellitesCount() {
		return isSimulationMode() ? 9 : gps.satellites.value();
	}

	float AHRS::getHDOP() {
		return isSimulationMode() ? 10 : static_cast<float>(gps.hdop.hdop());
	}

	std::span<char> AHRS::getRXData() {
		return { rxBuffer, static_cast<size_t>(rxDataLength) };
	}

	AHRSState AHRS::getState() const {
		return state;
	}

	float AHRS::getWaypoint1BearingDeg() const {
		return waypoint1BearingDeg;
	}

	float AHRS::getWaypoint1DistanceM() const {
		return waypoint1DistanceM;
	}

	uint32_t AHRS::getWaypoint1ETESec() const {
		return waypoint1ETESec;
	}

	float AHRS::getWaypoint2BearingDeg() const {
		return waypoint2BearingDeg;
	}

	float AHRS::getDistanceM() const {
		return distance;
	}

	uint8_t AHRS::computeCommandCRC(const char* data) {
		uint8_t crc = 0;

		while (*data != '\0') {
			crc ^= *data;
			data++;
		}

		return crc;
	}

	void AHRS::sendCommand(const char* command) {
		const auto crc = computeCommandCRC(command);
		const auto targetLen = 1 + strlen(command) + 1 + 2 + 2;

		const auto buffer = new char[targetLen + 1];
		snprintf(buffer, targetLen + 1, "$%s*%02X\r\n", command, crc);

		ESP_LOGI("AHRS", "Sending command: %s", buffer);

		uart_write_bytes(constants::gnss::port, buffer, targetLen);

		delete[] buffer;
	}

	void AHRS::tick() {
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

		// Satellite count is enough for data processing
		if (getSatellitesCount() >= 3) {
			// Location, copying raw values from GNSS module
			latitudeRad = toRadians(gps.location.lat());
			longitudeRad = toRadians(gps.location.lng());

			// Simulation
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

			// Awake from cold & dark state
			if (state == AHRSState::coldAndDark) {
				dataSamplingPrevLatRad = getLatitudeRad();
				dataSamplingPrevLonRad = getLongitudeRad();
			}

			// Definitely aligned
			state = AHRSState::aligned;

			// Too much time has passed since last update, data should be zeroed out
			if (esp_timer_get_time() >= dataSamplingZeroingTime) {
				// Speed
				speedMps = 0;

				// Speed trend
				speedTrendMps = 0;
				speedTrendOldSpeedMps = 0;

				// Time
				dataSamplingTime = esp_timer_get_time();
				dataSamplingZeroingTime = dataSamplingTime + rc.performanceProfile.AHRS.dataSamplingIntervalUs;
			}
			else {
				// Distance between 2 lat/lon samples
				const auto dataSamplingDistanceM = GeographicCoordinates::getDistance(
					dataSamplingPrevLatRad,
					dataSamplingPrevLonRad,
					getLatitudeRad(),
					getLongitudeRad()
				);

				// Distance between 2 samples is enough, data should be updated
				if (dataSamplingDistanceM >= static_cast<float>(rc.performanceProfile.AHRS.dataSamplingDistanceM)) {
					const auto dataUpdatingDeltaTimeUsF = static_cast<float>(esp_timer_get_time() - dataSamplingTime);

					// Speed
					// samplesDistanceM - interval us
					// x - 1'000'000 us
					speedMps = dataSamplingDistanceM * 1'000'000.f / dataUpdatingDeltaTimeUsF;

					// Speed trend
					// deltasPeed - interval us
					// x - trendInterval us
					speedTrendMps = (getSpeedMps() - speedTrendOldSpeedMps) * static_cast<float>(trendInterval) / dataUpdatingDeltaTimeUsF;
					speedTrendOldSpeedMps = getSpeedMps();

					// Course
					course = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
						dataSamplingPrevLatRad,
						dataSamplingPrevLonRad,
						getLatitudeRad(),
						getLongitudeRad()
					)));

					// Distance
					distance += dataSamplingDistanceM;

					// )))
					dataSamplingPrevLatRad = getLatitudeRad();
					dataSamplingPrevLonRad = getLongitudeRad();

					dataSamplingTime = esp_timer_get_time();
					dataSamplingZeroingTime = dataSamplingTime + rc.performanceProfile.AHRS.dataSamplingIntervalUs;
				}
			}

			// Altitude trend
			altitudeTrendM = (getAltitudeM() - oldAltitudeM) * static_cast<float>(trendInterval) / deltaTimeUsF;
			oldAltitudeM = getAltitudeM();

			// Waypoint 1
			waypoint1BearingDeg = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
				getLatitudeRad(),
				getLongitudeRad(),
				navWaypoint.coordinates.getLatitude(),
				navWaypoint.coordinates.getLongitude()
			)));

			waypoint1DistanceM = GeographicCoordinates::getDistance(
				getLatitudeRad(),
				getLongitudeRad(),
				navWaypoint.coordinates.getLatitude(),
				navWaypoint.coordinates.getLongitude()
			);

			waypoint1ETESec = speedMps > 0 ? static_cast<uint32_t>(waypoint1DistanceM / speedMps) : 0;

			// Waypoint 2
			waypoint2BearingDeg = normalizeAngle360(toDegrees(GeographicCoordinates::getBearing(
				getLatitudeRad(),
				getLongitudeRad(),
				bearingWaypoint.coordinates.getLatitude(),
				bearingWaypoint.coordinates.getLongitude()
			)));
		}
		else {
			// AHRS was previously aligned
			if (state == AHRSState::aligned) {
				state = AHRSState::lost;
			}
		}

		// ESP_LOGI("AHRS", "---------------- Processed data ----------------");
		// ESP_LOGI("AHRS", "Sat / HDOP: %lu, %lf", getSatellites(), getHDOP());
		// ESP_LOGI("AHRS", "Date / time: %d.%d.%d %d:%d:%d", getDateDay(), getDateMonth(), getDateYear(), getTimeHours(), getTimeMinutes(), getTimeSeconds());
		// ESP_LOGI("AHRS", "Lat / lon / alt: %f deg, %f deg, %f m", toDegrees(getLatitudeRad()), toDegrees(getLongitudeRad()), getAltitudeM());
		// ESP_LOGI("AHRS", "Speed / Course: %f mps, %f deg", getSpeedMps(), getCourseDeg());

		tickTime = esp_timer_get_time();
	}

	bool AHRS::isSimulationMode() {
		return RC::getInstance().settings.AHRS.simulation;
	}
}
