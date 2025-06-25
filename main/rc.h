#pragma once

#include <cstdint>

#include <YOBA/main.h>
#include <hardware/GNSS/GNSS.h>
#include <settings/settings.h>
#include <constants.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/SH1106Display.h>

#include "UI/theme.h"
#include "UI/navigation/eblo.h"

#include "constants.h"

namespace pizda {
	using namespace YOBA;

	class RC {
		public:
			Settings settings {};

			GNSS gps {
				constants::uart::port
			};

			SH1106Display display = SH1106Display(
				constants::spi::mosi,
				constants::spi::miso,
				constants::spi::sck,

				constants::screen::slaveSelect,
				constants::screen::dataCommand,
				constants::screen::reset,
				constants::screen::frequency
			);

			MonochromeRenderer renderer {};

			Application application {};
			Eblo eblo {};

			static RC& getInstance();

			[[noreturn]] void run();

		private:
			RC() = default;

			void SPIBusSetup() const;
	};
}
