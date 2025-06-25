#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/resources/fonts/PIXY10Font.h>
#include <YOBA/resources/fonts/Vaticanus8Font.h>

namespace pizda {
	using namespace YOBA;

	class Theme {
		public:
			static const MonochromeColor b;
			static const MonochromeColor w;

			static const Vaticanus8Font fontSmall;

			static void setup(Bit8PaletteRenderer* renderer);

			static void apply(Text* text);
	};
}