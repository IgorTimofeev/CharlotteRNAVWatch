#pragma once

#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;
	
	class SettingsGNSS : public NVSSettings {
		public:
			bool GPS = true;
			bool BeiDou = true;
			bool GLONASS = true;
			uint16_t updatingIntervalMs = 0;
			uint16_t dataSamplingDistanceM = 0;
			uint16_t dataSamplingZeroingIntervalMs = 0;
			bool simulation = false;

		protected:
			const char* getNVSNamespace() override {
				return "gns";
			}

			void onRead(const NVSStream& stream) override {
				updatingIntervalMs = stream.getUint16(_updatingIntervalMs, 1'000);
				dataSamplingDistanceM = stream.getUint16(_dataSamplingDistanceM, 20);
				dataSamplingZeroingIntervalMs = stream.getUint16(_dataSamplingZeroingIntervalMs, 10'000);

				GPS = stream.getBool(_GPS, true);
				BeiDou = stream.getBool(_BDS, true);
				GLONASS = stream.getBool(_GLONASS, true);

				simulation = stream.getBool(_simulation, false);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint16(_updatingIntervalMs, updatingIntervalMs);
				stream.setUint16(_dataSamplingDistanceM, dataSamplingDistanceM);
				stream.setUint16(_dataSamplingZeroingIntervalMs, dataSamplingZeroingIntervalMs);

				stream.setBool(_GPS, GPS);
				stream.setBool(_BDS, BeiDou);
				stream.setBool(_GLONASS, GLONASS);

				stream.setBool(_simulation, simulation);
			}

		private:
			constexpr static auto _updatingIntervalMs = "ui";
			constexpr static auto _dataSamplingDistanceM = "dd";
			constexpr static auto _dataSamplingZeroingIntervalMs = "dz";

			constexpr static auto _GPS = "gp";
			constexpr static auto _BDS = "bd";
			constexpr static auto _GLONASS = "go";

			constexpr static auto _simulation = "si";

	};
}
