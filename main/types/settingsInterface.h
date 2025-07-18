#pragma once

#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;

	enum class SettingsInterfaceTheme : uint8_t {
		dark,
		bright
	};

	class SettingsInterface : public NVSSettings {
		public:
			SettingsInterfaceTheme theme = SettingsInterfaceTheme::dark;

		protected:
			const char* getNVSNamespace() override {
				return "int";
			}

			void onRead(const NVSStream& stream) override {
				theme = static_cast<SettingsInterfaceTheme>(stream.getUint8(_theme, static_cast<uint8_t>(SettingsInterfaceTheme::dark)));
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint8(_theme, static_cast<uint8_t>(theme));
			}

		private:
			constexpr static auto _theme = "th";

	};
}

