#pragma once

#include <cstdint>

#include <YOBA/main.h>
#include <hardware/AHRS/AHRS.h>
#include <hardware/korryButton.h>
#include <constants.h>

#include <types/settings.h>
#include <types/performanceProfile.h>

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
			AHRS ahrs {};

			GC9A01Display display = GC9A01Display(
				constants::spi::mosi,
				constants::spi::miso,
				constants::spi::sck,

				constants::screen::slaveSelect,
				constants::screen::dataCommand,
				constants::screen::reset,
				constants::screen::frequency
			);

			RGB565PixelBufferRenderer renderer {};

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
			PerformanceProfile performanceProfile {};

			float courseDeg = 0;

			float altitudeFt = 0;
			float altitudeTrendFt = 0;
			float altitudeMinimumsFt = 80;

			float speedKt = 0;
			float speedTrendKt = 0;

			float waypoint1BearingDeg = 0;
			float waypoint2BearingDeg = 0;

			static RC& getInstance();

			void setup();
			void tick();

			const Route* getRoute() const;
			void setRoute(const Route* route);

			void updateThemeFromSettings() const;
			void updatePerformanceProfileFromSettings();
			void sleep();
			void setFaceRouteFromSettings();

		private:
			RC() = default;

			constexpr static uint32_t mainTickInterval = 1'000'000 / 30;
			constexpr static uint32_t computePrimaryTickInterval = 1'000'000 / 30;

			const Route* _selectedRoute = nullptr;
			Element* _selectedPage = nullptr;

			int64_t _computingPrimaryTickTime = 0;

			void SPIBusSetup() const;
			static void sleepSetup();
			void computeStuff();
	};
}
