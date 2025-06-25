#pragma once

#include <cstdint>
#include <esp_timer.h>
#include <esp_log.h>
#include <memory>
#include <cstring>

#include "settingsInterface.h"

namespace pizda {
	class Settings {
		public:
			SettingsInterface interface {};

			void readAll() {
				interface.read();
			}
	};
}
