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

			GNSS gnss {
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

			float navWaypointBearingDeg = 0;
			float navWaypointDistanceNm = 0;
			uint32_t navWaypointETESec = 0;

			float bearingWaypointBearingDeg = 0;

			static RC& getInstance();

			[[noreturn]] void run();

			const Route* getRoute() const;
			void setRoute(const Route* route);

		private:
			RC() = default;

			constexpr static uint32_t mainTickInterval = 1'000'000 / 30;
			constexpr static uint32_t computePrimaryTickInterval = 1'000'000 / 30;
			constexpr static uint32_t computeDelayedTickInterval = 2'000'000;

			const Route* _selectedRoute = nullptr;
			Element* _selectedPage = nullptr;

			int64_t _computingPrimaryTickTime = 0;
			int64_t _computingDelayedTickTime = 0;

			void SPIBusSetup() const;

			Vector3F computeWaypointBearingVector(const SettingsNavWaypoint& waypoint);
			static float computeWaypointBearingAngleDeg(const Vector3F& bearingVector);
			static float computeWaypointBearingDistance(const Vector3F& bearingVector);
			void computeStuff();
	};
}
