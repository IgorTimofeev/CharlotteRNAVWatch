#include <rc.h>
#include "utils/lowPassFilter.h"

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

		// GPS
		gps.setup();
		gps.setUpdateInterval(1000);
		gps.setGNSSSystem(GNSSSystem::GPS | GNSSSystem::GLONASS | GNSSSystem::Galileo | GNSSSystem::QZSS | GNSSSystem::SBAS);

		// UI
		Theme::setup(&renderer);
		application.setRenderer(&renderer);
		application.setBackgroundColor(&Theme::bg1);

		application += &eblo;

		// Main loop
		while (true) {
			application.tick();
			application.render();

			interpolationTick();

			// Skipping remaining tick time if any
			const auto deltaTime = esp_timer_get_time() - esp_timer_get_time();

			if (deltaTime < constants::application::mainTickInterval)
				vTaskDelay(pdMS_TO_TICKS((constants::application::mainTickInterval - deltaTime) / 1000));
		}
	}

	void RC::interpolationTick() {
		const uint32_t deltaTime = esp_timer_get_time() - _interpolationTickTime;

		if (deltaTime < constants::application::interpolationTickInterval)
			return;

		// Principle of calculating the interpolation factor:
		//
		// factorPerSecond -> 1'000'000 us
		// factorPerTick -> deltaTime us
		//
		// factorPerTick = factorPerSecond * deltaTime / 1'000'000

		// Slow
		float LPFFactor = 1.0f * static_cast<float>(deltaTime) / 1'000'000.f;
		LowPassFilter::apply(courseDeg, gps.getCourseDeg(), LPFFactor);
		LowPassFilter::apply(speedKt, gps.getSpeedKt(), LPFFactor);
		LowPassFilter::apply(altitudeFt, gps.getAltitudeFt(), LPFFactor);

		LowPassFilter::apply(courseDeviationDeg, courseDeviationDeg + 2, LPFFactor);

		if (courseDeviationDeg > 20)
			courseDeviationDeg = -20;

		_interpolationTickTime = esp_timer_get_time() + constants::application::interpolationTickInterval;
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
