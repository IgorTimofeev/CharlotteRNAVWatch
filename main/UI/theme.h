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
			static const Bit8PaletteColor bg1;
			static const Bit8PaletteColor bg2;
			static const Bit8PaletteColor bg3;
			static const Bit8PaletteColor bg4;
			static const Bit8PaletteColor bg5;
			static const Bit8PaletteColor bg6;
			static const Bit8PaletteColor bg7;

			static const Bit8PaletteColor fg1;
			static const Bit8PaletteColor fg2;
			static const Bit8PaletteColor fg3;
			static const Bit8PaletteColor fg4;
			static const Bit8PaletteColor fg5;
			static const Bit8PaletteColor fg6;
			static const Bit8PaletteColor fg7;

			static const Bit8PaletteColor accent1;
			static const Bit8PaletteColor accent2;

			static const Bit8PaletteColor good1;
			static const Bit8PaletteColor good2;

			static const Bit8PaletteColor bad1;
			static const Bit8PaletteColor bad2;

			static const Bit8PaletteColor red;
			static const Bit8PaletteColor purple;
			static const Bit8PaletteColor green;
			static const Bit8PaletteColor greenSpeed;
			static const Bit8PaletteColor yellow;
			static const Bit8PaletteColor ocean;
			static const Bit8PaletteColor ground;
			static const Bit8PaletteColor ground2;
			static const Bit8PaletteColor sky;
			static const Bit8PaletteColor sky2;

			static const unscii16Font fontBig;
			static const PIXY10Font fontNormal;
			static const Vaticanus8Font fontSmall;

			static void setup(Bit8PaletteRenderer* renderer);

			static void apply(Text* text);
	};
}