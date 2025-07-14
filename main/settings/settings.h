#pragma once

#include "settingsPFD.h"
#include "settingsNav.h"
#include "settingsGNSS.h"
#include "settingsInterface.h"

namespace pizda {
	using namespace YOBA;

	class Settings {
		public:
			SettingsPFD PFD {};
			SettingsNav nav {};
			SettingsGNSS GNSS {};
			SettingsInterface interface {};

			void readAll() {
				PFD.read();
				nav.read();
				GNSS.read();
				interface.read();
			}
	};
}
