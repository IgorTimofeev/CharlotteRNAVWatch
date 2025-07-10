#pragma once

#include <cstdint>

#include <YOBA/main.h>
#include <hardware/GNSS/GNSS.h>
#include <hardware/korryButton.h>
#include <settings/settings.h>
#include <constants.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/GC9A01Display.h>

#include "UI/theme.h"
#include "UI/navigation/routes.h"

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

			RGB565PixelBufferRenderer renderer {  };

			KorryButton buttonUp {
				KorryButtonType::up,
				constants::buttons::up
			};

			KorryButton buttonMiddle {
				KorryButtonType::middle,
				constants::buttons::middle
			};

			KorryButton buttonDown {
				KorryButtonType::down,
				constants::buttons::down
			};

			Application application {};

			float courseDeg = 0;

			float altitudeFt = 0;
			float altitudeTrendFt = 0;
			float altitudeMinimumsFt = 80;

			float speedKt = 0;
			float speedTrendKt = 0;

			float WPFBearingDeg = 60;
			float WPTCourseDeviationDeg = 5;
			float WPTDistance = 0.75;
			uint32_t WPTETESec = 12 * 60;

			static RC& getInstance();

			[[noreturn]] void run();
			void NVSSetup();

			const Route* getRoute() const;
			const void* getRouteArg() const;

			void setRoute(const Route* route, void* arg = nullptr);

		private:
			RC() = default;

			const Route* _selectedRoute = nullptr;
			Element* _selectedPage = nullptr;
			void* _selectedRouteArg = nullptr;

			int64_t _interpolationTickTime = 0;

			void SPIBusSetup() const;
			void interpolationTick();
	};
}
