#pragma once

#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;

	enum class SettingsInterfaceTheme : uint8_t {
		dark,
		bright
	};

	enum class SettingsInterfaceFace : uint8_t {
		PFD,
		analog,
		GNSSRaw
	};

	class SettingsInterface : public NVSSettings {
		public:
			SettingsInterfaceFace face = SettingsInterfaceFace::analog;
			SettingsInterfaceTheme theme = SettingsInterfaceTheme::dark;

		protected:
			const char* getNVSNamespace() override {
				return "int";
			}

			void onRead(const NVSStream& stream) override {
				face = static_cast<SettingsInterfaceFace>(stream.getUint8(_face, static_cast<uint8_t>(SettingsInterfaceFace::PFD)));
				theme = static_cast<SettingsInterfaceTheme>(stream.getUint8(_theme, static_cast<uint8_t>(SettingsInterfaceTheme::dark)));
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint8(_face, static_cast<uint8_t>(face));
				stream.setUint8(_theme, static_cast<uint8_t>(theme));
			}

		private:
			constexpr static auto _face = "rt";
			constexpr static auto _theme = "th";

	};
}

