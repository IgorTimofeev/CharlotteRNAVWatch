#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "types/settingsInterface.h"
#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class FacePage : public MenuPage {
		public:
			FacePage();

			FunctionMenuItem PFDItem {};
			FunctionMenuItem chronoItem {};
			FunctionMenuItem rawGNSSItem {};
			RouteMenuItem backItem {};
	};
}