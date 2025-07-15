#pragma once

#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;

	enum class SettingsPFDScale : uint8_t {
		foot,
		bike,
		car,
		airplane
	};

	class SettingsPFD : public NVSSettings {
		public:
			SettingsPFDScale scale = SettingsPFDScale::bike;
			uint16_t speedKt = 0;
			uint16_t altitudeFt = 0;
			uint16_t pressureHPA = 0;
			uint16_t minimumsFt = 0;

		protected:
			const char* getNVSNamespace() override {
				return "PFD";
			}

			void onRead(const NVSStream& stream) override {
				scale = static_cast<SettingsPFDScale>(stream.getUint8(_scale, static_cast<uint8_t>(SettingsPFDScale::bike)));
				speedKt = stream.getUint16(_speedKt, 10);
				altitudeFt = stream.getUint16(_altitudeFt, 120);
				pressureHPA = stream.getUint16(_pressureHPA, 1013);
				minimumsFt = stream.getUint16(_minimumsFt, 150);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint8(_scale, static_cast<uint8_t>(scale));
				stream.setUint16(_speedKt, speedKt);
				stream.setUint16(_altitudeFt, altitudeFt);
				stream.setUint16(_pressureHPA, pressureHPA);
				stream.setUint16(_minimumsFt, minimumsFt);
			}

		private:
			constexpr static auto _scale = "sc";
			constexpr static auto _speedKt = "sp";
			constexpr static auto _altitudeFt = "al";
			constexpr static auto _pressureHPA = "pr";
			constexpr static auto _minimumsFt = "mn";

	};
}
