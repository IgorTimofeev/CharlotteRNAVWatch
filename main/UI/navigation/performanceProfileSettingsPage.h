#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "types/settingsNav.h"
#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class PerformanceProfileMenuItem : public ListMenuItem {
		public:
			explicit PerformanceProfileMenuItem(SettingsNavPerformanceProfile performanceProfile);

		protected:
			bool isSelected() const override;
			void onSelectionRequested() override;

		private:
			SettingsNavPerformanceProfile performanceProfile;
	};

	class PerformanceProfileSettingsPage : public MenuPage {
		public:
			PerformanceProfileSettingsPage();

			PerformanceProfileMenuItem footItem { SettingsNavPerformanceProfile::onFoot };
			PerformanceProfileMenuItem cyclingItem { SettingsNavPerformanceProfile::cycling };
			PerformanceProfileMenuItem diamondDA40Item { SettingsNavPerformanceProfile::diamondDA40 };
			RouteMenuItem backItem {};
	};
}