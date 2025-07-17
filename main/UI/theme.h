#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/resources/fonts/unscii16.h>
#include <YOBA/resources/fonts/PIXY10Font.h>
#include <YOBA/resources/fonts/Vaticanus8Font.h>
#include <YOBA/resources/fonts/unscii8Font.h>
#include <YOBA/resources/fonts/unscii8ThinFont.h>

namespace pizda {
	using namespace YOBA;

	class Theme {
		public:
			static RGB565Color bg1;
			static RGB565Color bg2;
			static RGB565Color bg3;
			static RGB565Color bg4;
			static RGB565Color bg5;
			static RGB565Color bg6;
			static RGB565Color bg7;

			static RGB565Color fg1;
			static RGB565Color fg2;
			static RGB565Color fg3;
			static RGB565Color fg4;
			static RGB565Color fg5;
			static RGB565Color fg6;
			static RGB565Color fg7;

			static RGB565Color bgRed1;
			static RGB565Color bgRed2;
			static RGB565Color bgRed3;

			static RGB565Color fgRed1;

			static RGB565Color good;

			static RGB565Color bad1;
			static RGB565Color bad2;

			static RGB565Color red;
			static RGB565Color redBright;
			static RGB565Color purple;
			static RGB565Color green;
			static RGB565Color yellow;
			static RGB565Color ocean;

			static RGB565Color speedBarWhite;
			static RGB565Color speedBarGreen;
			static RGB565Color speedBarYellow;
			static RGB565Color speedBarRed;

			static const unscii8Font fontBig;
			static const unscii8ThinFont fontBigThin;
			static const PIXY10Font fontNormal;
			static const Vaticanus8Font fontSmall;

			static void setDark();
			static void setBright();

		private:
			static void setCommon();
	};
}