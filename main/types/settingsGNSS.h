#pragma once

#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;
	
	class SettingsGNSS : public NVSSettings {
		public:
			bool GPS = true;
			bool BDS = true;
			bool GLONASS = true;
			bool simulation = false;
			uint32_t tickIntervalUs = 0;

		protected:
			const char* getNVSNamespace() override {
				return "GNS";
			}

			void onRead(const NVSStream& stream) override {
				GPS = stream.getBool(_GPS, true);
				BDS = stream.getBool(_BDS, true);
				GLONASS = stream.getBool(_GLONASS, true);
				simulation = stream.getBool(_simulation, false);
				tickIntervalUs = stream.getUint32(_tickIntervalUs, 1'000'000);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setBool(_GPS, GPS);
				stream.setBool(_BDS, BDS);
				stream.setBool(_GLONASS, GLONASS);
				stream.setBool(_simulation, simulation);
				stream.setUint32(_tickIntervalUs, tickIntervalUs);
			}

		private:
			constexpr static auto _GPS = "gp";
			constexpr static auto _BDS = "bd";
			constexpr static auto _GLONASS = "go";
			constexpr static auto _simulation = "si";
			constexpr static auto _tickIntervalUs = "ti";

	};
}
