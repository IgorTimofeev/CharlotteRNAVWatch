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
		NVSSettings::setup();
		settings.readAll();

		// Buttons
		buttonUp.setup();
		buttonMiddle.setup();
		buttonDown.setup();

		// GNSS
		gnss.setup();
		gnss.setUpdateInterval(1'000'000);
		gnss.startReading();

		// UI
		updateThemeFromSettings();
		updatePerformanceProfileFromSettings();
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

	void RC::updateThemeFromSettings() const {
		switch (settings.interface.theme) {
			case SettingsInterfaceTheme::dark:
				Theme::setDark();
				break;
			default:
				Theme::setBright();
				break;
		}
	}

	void RC::updatePerformanceProfileFromSettings() {
		switch (settings.nav.performanceProfile) {
			case SettingsNavPerformanceProfile::onFoot:
				performanceProfile.setOnFoot();
				break;
			case SettingsNavPerformanceProfile::cycling:
				performanceProfile.setCycling();
				break;
			case SettingsNavPerformanceProfile::diamondDA40:
				performanceProfile.setDiamondDA40();
				break;
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

	void RC::computeStuff() {
		const auto deltaTime = static_cast<float>(esp_timer_get_time() - _computingPrimaryTickTime);

		if (deltaTime < computePrimaryTickInterval)
			return;


		// Low pass

		// factorPerSecond -> 1'000'000 us
		// factorPerTick -> deltaTime us
		//
		// factorPerTick = factorPerSecond * deltaTime / 1'000'000

		// Normal
		float LPFFactor = deltaTime / 1'000'000.f;

		LowPassFilter::apply(speedKt, gnss.isSatellitesCountEnough() ? Units::convertSpeed(gnss.getSpeedMps(), SpeedUnit::meterPerSecond, SpeedUnit::knot) : 0, LPFFactor);
		LowPassFilter::apply(altitudeFt, gnss.isSatellitesCountEnough() ? Units::convertDistance(gnss.getAltitudeM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		// Course
		if (speedKt > 2) {
			const auto targetCourseDeg = gnss.getComputedCourseDeg();
			const auto courseLPFFactor = deltaTime / 1'000'000.f;

			// Clockwise
			if (courseDeg > 270 && targetCourseDeg < 90) {
				courseDeg = courseDeg - 360;
				LowPassFilter::apply(courseDeg, targetCourseDeg, courseLPFFactor);
			}
			// Counter-clockwise
			else if (targetCourseDeg > 270 && courseDeg < 90) {
				LowPassFilter::apply(courseDeg, targetCourseDeg - 360, courseLPFFactor);
				courseDeg = normalizeAngle360(courseDeg);
			}
			else {
				LowPassFilter::apply(courseDeg, targetCourseDeg, courseLPFFactor);
			}
		}

		// Slow
		LPFFactor = deltaTime / 2'000'000.f;
		LowPassFilter::apply(speedTrendKt, gnss.isSatellitesCountEnough() ? Units::convertSpeed(gnss.getSpeedTrendMps(), SpeedUnit::meterPerSecond, SpeedUnit::knot) : 0, LPFFactor);
		LowPassFilter::apply(altitudeTrendFt, gnss.isSatellitesCountEnough() ? Units::convertDistance(gnss.getAltitudeTrendM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		LowPassFilter::apply(waypoint1BearingDeg, gnss.getWaypoint1BearingDeg(), LPFFactor);
		LowPassFilter::apply(waypoint2BearingDeg, gnss.getWaypoint2BearingDeg(), LPFFactor);

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
