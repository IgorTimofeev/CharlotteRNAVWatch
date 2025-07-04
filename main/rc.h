#pragma once

#include <cstdint>

#include <YOBA/main.h>
#include <hardware/GNSS/GNSS.h>
#include <settings/settings.h>
#include <constants.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/GC9A01Display.h>

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

			GC9A01Display display = GC9A01Display(
				constants::spi::mosi,
				constants::spi::miso,
				constants::spi::sck,

				constants::screen::slaveSelect,
				constants::screen::dataCommand,
				constants::screen::reset,
				constants::screen::frequency
			);

			Bit8PaletteRenderer renderer { 32 };

			Application application {};
			Eblo eblo {};

			float courseDeg = 0;
			float HSICourseDeg = 32;

			static RC& getInstance();

			[[noreturn]] void run();

		private:
			RC() = default;

			uint64_t _interpolationTickTime = 0;

			void SPIBusSetup() const;
			void interpolationTick();
	};
}
