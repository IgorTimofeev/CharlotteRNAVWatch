#include <rc.h>

#include "resources/images.h"
#include "utils/lowPassFilter.h"
#include <YOBAUnits.h>
#include <esp_sleep.h>

namespace pizda {
	using namespace YOBA;

	RC& RC::getInstance() {
		static auto instance = RC();

		return instance;
	}

	void RC::setup() {
		// SPI comes first to render splash screen on boot
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

		// Sleep
		sleepSetup();

		// GNSS
		ahrs.setup();
		ahrs.updateUpdatingIntervalFromSettings();

		// UI
		updateThemeFromSettings();
		updatePerformanceProfileFromSettings();
		application.setRenderer(&renderer);
		application.setBackgroundColor(&Theme::bg1);

		application.addHID(&buttonUp);
		application.addHID(&buttonMiddle);
		application.addHID(&buttonDown);

		setFaceRouteFromSettings();

		// This shit is blazingly 🔥 fast 🚀, so letting user enjoy logo for a few moments
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	void RC::tick() {
		const auto tickTime = esp_timer_get_time();

		computeStuff();

		application.tick();
		application.render();

		// Skipping remaining tick time if any
		const auto deltaTime = esp_timer_get_time() - tickTime;

		if (deltaTime < mainTickInterval)
			vTaskDelay(pdMS_TO_TICKS((mainTickInterval - deltaTime) / 1000));
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
		return selectedRoute;
	}

	void RC::setRoute(const Route* route) {
		if (selectedPage) {
			application -= selectedPage;
			delete selectedPage;
		}

		selectedRoute = route;
		selectedPage = selectedRoute->buildElement();
		application += selectedPage;
	}

	void RC::computeStuff() {
		const auto deltaTime = static_cast<float>(esp_timer_get_time() - computingPrimaryTickTime);

		if (deltaTime < computePrimaryTickInterval)
			return;


		// Low pass

		// factorPerSecond -> 1'000'000 us
		// factorPerTick -> deltaTime us
		//
		// factorPerTick = factorPerSecond * deltaTime / 1'000'000

		// Normal
		float LPFFactor = deltaTime / 1'000'000.f;

		const auto isAHRSAligned = ahrs.getState() == AHRSState::aligned;

		LowPassFilter::apply(speedKt, isAHRSAligned ? Units::convertSpeed(ahrs.getSpeedMps(), SpeedUnit::meterPerSecond, SpeedUnit::knot) : 0, LPFFactor);
		LowPassFilter::apply(altitudeFt, isAHRSAligned ? Units::convertDistance(ahrs.getAltitudeM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		// Course
		if (speedKt > 2) {
			const auto targetCourseDeg = ahrs.getCourseDeg();
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
		LowPassFilter::apply(speedTrendKt, isAHRSAligned ? Units::convertSpeed(ahrs.getSpeedTrendMps(), SpeedUnit::meterPerSecond, SpeedUnit::knot) : 0, LPFFactor);
		LowPassFilter::apply(altitudeTrendFt, isAHRSAligned ? Units::convertDistance(ahrs.getAltitudeTrendM(), DistanceUnit::meter, DistanceUnit::foot) : 0, LPFFactor);

		LowPassFilter::apply(waypoint1BearingDeg, ahrs.getWaypoint1BearingDeg(), LPFFactor);
		LowPassFilter::apply(waypoint2BearingDeg, ahrs.getWaypoint2BearingDeg(), LPFFactor);

		// ESP_LOGI("CDI", "gps lat: %f, lon: %f", toDegrees(gps.getLatitudeRad()), toDegrees(gps.getLongitudeRad()));
		// ESP_LOGI("CDI", "Bearing deg: %f", WPTBearingDeg);
		// ESP_LOGI("CDI", "dist: %f", WPTDistanceM);
		// ESP_LOGI("CDI", "WPTCourseDeviationDeg deg: %f", WPTCourseDeviationDeg);

		computingPrimaryTickTime = esp_timer_get_time() + computePrimaryTickInterval;
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

	void RC::sleepSetup() {
		ESP_ERROR_CHECK(gpio_wakeup_enable(constants::buttons::middle, GPIO_INTR_LOW_LEVEL));
		ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());
	}

	void RC::sleep() {
		ESP_LOGI("RC", "Light sleep started");

		// Waiting for button release
		while (buttonMiddle.isPressed())
			vTaskDelay(pdMS_TO_TICKS(10));

		// Turning off peripherals for power saving
		display.turnOff();

		// Zzzzz....
		ESP_ERROR_CHECK(esp_light_sleep_start());

		// Moving to active face
		setFaceRouteFromSettings();
		application.render();

		// Turning peripherals on again
		display.turnOn();

		ESP_LOGI("RC", "Light sleep finished");
	}

	void RC::setFaceRouteFromSettings() {
		switch (settings.interface.face) {
			case SettingsInterfaceFace::PFD:
				setRoute(&Routes::PFD);
				break;
			case SettingsInterfaceFace::chrono:
				setRoute(&Routes::chrono);
				break;
			case SettingsInterfaceFace::rawGNSS:
				setRoute(&Routes::rawGNSS);
				break;
		}
	}
}
