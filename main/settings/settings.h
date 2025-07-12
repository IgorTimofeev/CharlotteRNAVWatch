#pragma once

#include <cstdint>
#include <esp_timer.h>
#include <esp_log.h>
#include <memory>
#include <cstring>

#include "settingsPFD.h"
#include "settingsNav.h"
#include "settingsGNSS.h"

namespace pizda {
	class Settings {
		public:
			SettingsPFD PFD {};
			SettingsNav nav {};
			SettingsGNSS GNSS {};

			void readAll() {
				PFD.read();
				nav.read();
				GNSS.read();
			}
	};
}
