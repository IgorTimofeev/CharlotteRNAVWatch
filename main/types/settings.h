#pragma once

#include "settingsPFD.h"
#include "settingsNav.h"
#include "settingsAHRS.h"
#include "settingsInterface.h"

namespace pizda {
	using namespace YOBA;

	class Settings {
		public:
			SettingsPFD PFD {};
			SettingsNav nav {};
			SettingsAHRS AHRS {};
			SettingsInterface interface {};

			void readAll() {
				PFD.read();
				nav.read();
				AHRS.read();
				interface.read();
			}
	};
}
