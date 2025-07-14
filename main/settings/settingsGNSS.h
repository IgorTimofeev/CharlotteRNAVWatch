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

		protected:
			const char* getNVSNamespace() override {
				return "GNS";
			}

			void onRead(const NVSStream& stream) override {
				GPS = stream.getBool(_GPS, true);
				BDS = stream.getBool(_BDS, true);
				GLONASS = stream.getBool(_GLONASS, true);
				simulation = stream.getBool(_simulation, false);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setBool(_GPS, GPS);
				stream.setBool(_BDS, BDS);
				stream.setBool(_GLONASS, GLONASS);
				stream.setBool(_simulation, simulation);
			}

		private:
			constexpr static auto _GPS = "gp";
			constexpr static auto _BDS = "bd";
			constexpr static auto _GLONASS = "go";
			constexpr static auto _simulation = "si";

	};
}
