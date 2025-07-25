#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "types/settingsInterface.h"
#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class FaceMenuItem : public ListMenuItem {
		public:
			explicit FaceMenuItem(SettingsInterfaceFace face);

		protected:
			bool isSelected() const override;
			void onSelectionRequested() override;

		private:
			SettingsInterfaceFace face;
	};

	class FacePage : public MenuPage {
		public:
			FacePage();

			FaceMenuItem PFDItem { SettingsInterfaceFace::PFD };
			FaceMenuItem analogItem { SettingsInterfaceFace::analog };
			FaceMenuItem rawItem { SettingsInterfaceFace::GNSSRaw };

			RouteMenuItem backItem {};
	};
}