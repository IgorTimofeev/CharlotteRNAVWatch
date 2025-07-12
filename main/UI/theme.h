#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/resources/fonts/unscii16.h>
#include <YOBA/resources/fonts/PIXY10Font.h>
#include <YOBA/resources/fonts/Vaticanus8Font.h>

namespace pizda {
	using namespace YOBA;

	class Theme {
		public:
			static const RGB565Color bg1;
			static const RGB565Color bg2;
			static const RGB565Color bg3;
			static const RGB565Color bg4;
			static const RGB565Color bg5;
			static const RGB565Color bg6;
			static const RGB565Color bg7;

			static const RGB565Color bgRed1;
			static const RGB565Color bgRed2;
			static const RGB565Color bgRed3;

			static const RGB565Color fg1;
			static const RGB565Color fg2;
			static const RGB565Color fg3;
			static const RGB565Color fg4;
			static const RGB565Color fg5;
			static const RGB565Color fg6;
			static const RGB565Color fg7;

			static const RGB565Color accent1;
			static const RGB565Color accent2;

			static const RGB565Color good1;
			static const RGB565Color good2;

			static const RGB565Color bad1;
			static const RGB565Color bad2;

			static const RGB565Color red;
			static const RGB565Color purple;
			static const RGB565Color green;
			static const RGB565Color greenSpeed;
			static const RGB565Color yellow;
			static const RGB565Color ocean;
			static const RGB565Color ground;
			static const RGB565Color ground2;
			static const RGB565Color sky;
			static const RGB565Color sky2;

			static const unscii16Font fontBig;
			static const PIXY10Font fontNormal;
			static const Vaticanus8Font fontSmall;

			static void apply(Text* text);
	};
}