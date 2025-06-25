#include <rc.h>


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
		gps.setGNSSSystem(GNSSSystem::GPS | GNSSSystem::GLONASS | GNSSSystem::Galileo);

		// UI
		application.setRenderer(&renderer);
		application.setBackgroundColor(&Theme::b);

		application += &eblo;

		// Main loop
		while (true) {
			application.tick();
			application.render();

			vTaskDelay(pdMS_TO_TICKS(1000 / 30));
		}
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
