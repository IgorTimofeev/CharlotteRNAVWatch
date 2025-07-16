#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/navigation/page.h"

namespace pizda {
	using namespace YOBA;

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			BoolMenuItem testItem1 { L"testItem1" };
			BoolMenuItem testItem2 { L"testItem2" };
			BoolMenuItem testItem3 { L"testItem3" };
			BoolMenuItem testItem4 { L"testItem4" };
			BoolMenuItem testItem5 { L"testItem5" };
			BoolMenuItem testItem6 { L"testItem6" };
			BoolMenuItem testItem7 { L"testItem7" };
			BoolMenuItem testItem8 { L"testItem8" };

			RouteMenuItem waypointsItem;
			RouteMenuItem devItem;
			RouteMenuItem settingsItem;
			RouteMenuItem backItem;
	};
}