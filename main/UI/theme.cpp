#include "theme.h"

namespace pizda {
	const Bit8PaletteColor Theme::bg1 = Bit8PaletteColor(0);
	const Bit8PaletteColor Theme::bg2 = Bit8PaletteColor(1);
	const Bit8PaletteColor Theme::bg3 = Bit8PaletteColor(2);
	const Bit8PaletteColor Theme::bg4 = Bit8PaletteColor(3);
	const Bit8PaletteColor Theme::bg5 = Bit8PaletteColor(4);
	const Bit8PaletteColor Theme::bg6 = Bit8PaletteColor(5);
	const Bit8PaletteColor Theme::bg7 = Bit8PaletteColor(6);

	const Bit8PaletteColor Theme::fg1 = Bit8PaletteColor(7);
	const Bit8PaletteColor Theme::fg2 = Bit8PaletteColor(8);
	const Bit8PaletteColor Theme::fg3 = Bit8PaletteColor(9);
	const Bit8PaletteColor Theme::fg4 = Bit8PaletteColor(10);
	const Bit8PaletteColor Theme::fg5 = Bit8PaletteColor(11);
	const Bit8PaletteColor Theme::fg6 = Bit8PaletteColor(12);
	const Bit8PaletteColor Theme::fg7 = Bit8PaletteColor(13);

	const Bit8PaletteColor Theme::accent1 = Bit8PaletteColor(14);
	const Bit8PaletteColor Theme::accent2 = Bit8PaletteColor(15);

	const Bit8PaletteColor Theme::good1 = Bit8PaletteColor(16);
	const Bit8PaletteColor Theme::good2 = Bit8PaletteColor(17);

	const Bit8PaletteColor Theme::bad1 = Bit8PaletteColor(18);
	const Bit8PaletteColor Theme::bad2 = Bit8PaletteColor(19);

	const Bit8PaletteColor Theme::red = Bit8PaletteColor(20);
	const Bit8PaletteColor Theme::purple = Bit8PaletteColor(21);
	const Bit8PaletteColor Theme::green = Bit8PaletteColor(22);
	const Bit8PaletteColor Theme::greenSpeed = Bit8PaletteColor(23);
	const Bit8PaletteColor Theme::yellow = Bit8PaletteColor(24);
	const Bit8PaletteColor Theme::ocean = Bit8PaletteColor(25);
	const Bit8PaletteColor Theme::ground = Bit8PaletteColor(26);
	const Bit8PaletteColor Theme::ground2 = Bit8PaletteColor(27);
	const Bit8PaletteColor Theme::sky = Bit8PaletteColor(28);
	const Bit8PaletteColor Theme::sky2 = Bit8PaletteColor(29);

	const unscii16Font Theme::fontBig {};
	const PIXY10Font Theme::fontNormal {};
	const Vaticanus8Font Theme::fontSmall {};

	void Theme::setup(Bit8PaletteRenderer* renderer) {
		renderer->setPaletteColors({
			// Background
			RGB888Color(0x000000),
			RGB888Color(0x0C0C0C),
			RGB888Color(0x1C1C1C),
			RGB888Color(0x2C2C2C),
			RGB888Color(0x3C3C3C),
			RGB888Color(0x4C4C4C),
			RGB888Color(0x5C5C5C),

			// Foreground
			RGB888Color(0xF0F0F0),
			RGB888Color(0xCCCCCC),
			RGB888Color(0x999999),
			RGB888Color(0x888888),
			RGB888Color(0x777777),
			RGB888Color(0x666666),
			RGB888Color(0x555555),

			// Accent 1
			RGB888Color(0xffd200),
			// Accent 2
			RGB888Color(0x997e53),

			// Good 1
			RGB888Color(0x1db153),
			// Good 2
			RGB888Color(0x28DA69),

			// Bad 1
			RGB888Color(0x9f2d2d),
			// Bad 2
			RGB888Color(0xf24949),

			// Red
			RGB888Color(0xff0000),
			// Purple
			RGB888Color(0xff00ff),
			// Green
			RGB888Color(0x00FF00),
			// Green speed
			RGB888Color(0x008100),
			// Yellow
			RGB888Color(0xffd200),
			// Ocean
			RGB888Color(0x00ffff),
			// Ground
			RGB888Color(0x97b838),
			// Ground 2
			RGB888Color(0xdef2a2),
			// Sky
			RGB888Color(0x317fcb),
			// Sky 2
			RGB888Color(0xaed1f2),
		});
	}

	void Theme::apply(Text* text) {
		text->setFont(&Theme::fontSmall);
		text->setTextColor(&Theme::fg1);
	}
}
