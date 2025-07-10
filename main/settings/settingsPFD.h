#pragma once

#include <cstdint>

#include <hardware/NVS/NVSStream.h>
#include <hardware/NVS/NVSSerializable.h>

namespace pizda {
	enum class SettingsPFDScale : uint8_t {
		foot,
		bike,
		car,
		airplane
	};

	class SettingsPFD : public NVSSerializable {
		public:
			SettingsPFDScale scale = SettingsPFDScale::bike;
			uint16_t speedKt = 10;
			uint16_t altitudeFt = 10;
			uint16_t pressureHPA = 1013;

		protected:
			const char* getNVSNamespace() override {
				return _namespace;
			}

			void onRead(const NVSStream& stream) override {
				scale = static_cast<SettingsPFDScale>(stream.getUint8(_scale, static_cast<uint8_t>(SettingsPFDScale::bike)));
				speedKt = stream.getUint16(_speedKt, 10);
				altitudeFt = stream.getUint16(_altitudeFt, 120);
				pressureHPA = stream.getUint16(_pressureHPA, 1013);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint8(_scale, static_cast<uint8_t>(scale));
				stream.setUint16(_speedKt, speedKt);
				stream.setUint16(_altitudeFt, altitudeFt);
				stream.setUint16(_pressureHPA, pressureHPA);
			}

		private:
			constexpr static auto _namespace = "PFD";
			constexpr static auto _scale = "sc";
			constexpr static auto _speedKt = "sp";
			constexpr static auto _altitudeFt = "al";
			constexpr static auto _pressureHPA = "pr";

	};
}
