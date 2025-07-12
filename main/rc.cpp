#include <rc.h>

#include "resources/images.h"
#include "utils/lowPassFilter.h"
#include <YOBAUnits.h>

namespace pizda {
	using namespace YOBA;

	RC& RC::getInstance() {
		static auto instance = RC();

		return instance;
	}

	[[noreturn]] void RC::run() {
		SPIBusSetup();

		// Display
		display.setup();
		renderer.setTarget(&display);

		// Rendering splash screen
		renderer.clear(&Theme::bg1);
		renderer.renderImage(Point(), &resources::Images::splashScreen);
		renderer.flush();

		// Turning display on
		display.turnOn();

		// Settings
		NVSSetup();
		settings.readAll();

		// Buttons
		buttonUp.setup();
		buttonMiddle.setup();
		buttonDown.setup();

		// GNSS
		gnss.setup();
		gnss.setUpdateInterval(1000);
		gnss.setGNSSSystem(GNSSSystem::GPS | GNSSSystem::GLONASS | GNSSSystem::Galileo | GNSSSystem::QZSS | GNSSSystem::SBAS);

		// UI
		// Theme::setup(&renderer);
		application.setRenderer(&renderer);
		application.setBackgroundColor(&Theme::bg1);

		application.addHID(&buttonUp);
		application.addHID(&buttonMiddle);
		application.addHID(&buttonDown);

		setRoute(&Routes::PFD);

		// This shit is blazingly 🔥 fast 🚀, so letting user enjoy logo for a few moments
		vTaskDelay(pdMS_TO_TICKS(1000));

		// Main loop
		while (true) {
			const auto tickTime = esp_timer_get_time();

			computeStuff();

			application.tick();
			application.render();

			// Skipping remaining tick time if any
			const auto deltaTime = esp_timer_get_time() - tickTime;

			if (deltaTime < mainTickInterval)
				vTaskDelay(pdMS_TO_TICKS((mainTickInterval - deltaTime) / 1000));
		}
	}

	void RC::NVSSetup() {
		const auto status = nvs_flash_init();

		if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			// NVS partition was truncated and needs to be erased
			ESP_ERROR_CHECK(nvs_flash_erase());
			// Retry init
			ESP_ERROR_CHECK(nvs_flash_init());
		}
		else {
			ESP_ERROR_CHECK(status);
		}
	}

	const Route* RC::getRoute() const {
		return _selectedRoute;
	}

	void RC::setRoute(const Route* route) {
		if (_selectedPage) {
			application -= _selectedPage;
			delete _selectedPage;
		}

		_selectedRoute = route;
		_selectedPage = _selectedRoute->buildElement();
		application += _selectedPage;
	}

	Vector3F RC::computeWaypointBearingVector(const SettingsNavWaypoint& waypoint) {
		const auto aircraftCoordinates = GeographicCoordinates(
			gnss.getLatitudeRad(),
			gnss.getLongitudeRad(),
			gnss.getAltitudeM()
		).toCartesian();

		auto coordinatesDelta = waypoint.geographicCoordinates.toCartesian() - aircraftCoordinates;
		coordinatesDelta = coordinatesDelta.rotateAroundZAxis(-gnss.getLongitudeRad());
		coordinatesDelta = coordinatesDelta.rotateAroundYAxis(gnss.getLatitudeRad());

		return coordinatesDelta;
	}

	float RC::computeWaypointBearingAngleDeg(const Vector3F& bearingVector) {
		return normalizeAngle360(toDegrees(std::atan2f(bearingVector.getY(), bearingVector.getZ())));
	}

	float RC::computeWaypointBearingDistance(const Vector3F& bearingVector) {
		return bearingVector.getLength();
	}

	void RC::computeStuff() {
		const auto deltaTime = static_cast<float>(esp_timer_get_time() - _computingPrimaryTickTime);

		if (deltaTime < computePrimaryTickInterval)
			return;

		const auto& navWaypoint = settings.nav.waypoints[settings.nav.navWaypointIndex];
		const auto& bearingWaypoint = settings.nav.waypoints[settings.nav.bearingWaypointIndex];

		const auto navWaypointBearingVector = computeWaypointBearingVector(navWaypoint);
		const auto navWaypointBearingDegTarget = computeWaypointBearingAngleDeg(navWaypointBearingVector);

		const auto bearingWaypointBearingVector = computeWaypointBearingVector(bearingWaypoint);
		const auto bearingWaypointBearingDegTarget = computeWaypointBearingAngleDeg(bearingWaypointBearingVector);

		// Low pass

		// factorPerSecond -> 1'000'000 us
		// factorPerTick -> deltaTime us
		//
		// factorPerTick = factorPerSecond * deltaTime / 1'000'000

		// Fast
		float LPFFactor = 2.0f * deltaTime / 1'000'000.f;
		LowPassFilter::apply(navWaypointBearingDeg, navWaypointBearingDegTarget, LPFFactor);
		LowPassFilter::apply(bearingWaypointBearingDeg, bearingWaypointBearingDegTarget, LPFFactor);

		// Normal
		LPFFactor = 1.0f * deltaTime / 1'000'000.f;
		LowPassFilter::apply(courseDeg, gnss.haveCourse() ? gnss.getCourseDeg() : 0, LPFFactor);
		LowPassFilter::apply(speedKt, gnss.haveSpeed() ? Units::convertSpeed(gnss.getSpeedMps(), SpeedUnit::meterPerSecond, SpeedUnit::knot) : 0, LPFFactor);
		LowPassFilter::apply(altitudeFt, gnss.haveAltitude() ? Units::convertDistance(gnss.getAltitudeM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		// Slow
		LPFFactor = 0.5f * deltaTime / 1'000'000.f;
		LowPassFilter::apply(speedTrendKt, gnss.haveSpeed() ? gnss.getSpeedTrendKt() : 0, LPFFactor);
		LowPassFilter::apply(altitudeTrendFt, gnss.haveAltitude() ? Units::convertDistance(gnss.getAltitudeTrendM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		// Delayed shit
		if (esp_timer_get_time() >= _computingDelayedTickTime) {
			navWaypointDistanceNm = Units::convertDistance(computeWaypointBearingDistance(bearingWaypointBearingVector), DistanceUnit::meter, DistanceUnit::nauticalMile);
			navWaypointETESec = static_cast<uint32_t>(navWaypointDistanceNm / speedKt * 3600);

			_computingDelayedTickTime = esp_timer_get_time() + computeDelayedTickInterval;
		}

		// ESP_LOGI("CDI", "gps lat: %f, lon: %f", toDegrees(gps.getLatitudeRad()), toDegrees(gps.getLongitudeRad()));
		// ESP_LOGI("CDI", "Bearing deg: %f", WPTBearingDeg);
		// ESP_LOGI("CDI", "dist: %f", WPTDistanceM);
		// ESP_LOGI("CDI", "WPTCourseDeviationDeg deg: %f", WPTCourseDeviationDeg);

		_computingPrimaryTickTime = esp_timer_get_time() + computePrimaryTickInterval;
	}

	void RC::SPIBusSetup() const {
		spi_bus_config_t busConfig {};
		busConfig.mosi_io_num = constants::spi::mosi;
		busConfig.miso_io_num = constants::spi::miso;
		busConfig.sclk_io_num = constants::spi::sck;
		busConfig.quadwp_io_num = -1;
		busConfig.quadhd_io_num = -1;
		busConfig.max_transfer_sz = static_cast<int32_t>(display.getSize().getSquare()) * 2;

		ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO));
	}
}
