#include "theme.h"

namespace pizda {
	RGB565Color Theme::bg1 { 0 };
	RGB565Color Theme::bg2 { 0 };
	RGB565Color Theme::bg3 { 0 };
	RGB565Color Theme::bg4 { 0 };
	RGB565Color Theme::bg5 { 0 };
	RGB565Color Theme::bg6 { 0 };
	RGB565Color Theme::bg7 { 0 };

	RGB565Color Theme::fg1 { 0 };
	RGB565Color Theme::fg2 { 0 };
	RGB565Color Theme::fg3 { 0 };
	RGB565Color Theme::fg4 { 0 };
	RGB565Color Theme::fg5 { 0 };
	RGB565Color Theme::fg6 { 0 };
	RGB565Color Theme::fg7 { 0 };

	RGB565Color Theme::bgRed1 { 0 };
	RGB565Color Theme::bgRed2 { 0 };
	RGB565Color Theme::bgRed3 { 0 };

	RGB565Color Theme::fgRed1 { 0 };

	RGB565Color Theme::good { 0 };

	RGB565Color Theme::bad1 { 0 };
	RGB565Color Theme::bad2 { 0 };

	RGB565Color Theme::red { 0 };
	RGB565Color Theme::redBright { 0 };

	RGB565Color Theme::purple { 0 };
	RGB565Color Theme::green { 0 };
	RGB565Color Theme::yellow { 0 };
	RGB565Color Theme::ocean { 0 };

	RGB565Color Theme::speedBarWhite { 0 };
	RGB565Color Theme::speedBarGreen { 0 };
	RGB565Color Theme::speedBarYellow { 0 };
	RGB565Color Theme::speedBarRed { 0 };

	const unscii16Font Theme::fontBig {};
	const PIXY10Font Theme::fontNormal {};
	const Vaticanus8Font Theme::fontSmall {};

	void Theme::setCommon() {
		good = RGB888Color(0x28DA69).toRGB565();

		bad1 = RGB888Color(0x9f2d2d).toRGB565();
		bad2 = RGB888Color(0xf24949).toRGB565();

		red = RGB888Color(0xff0000).toRGB565();
		purple = RGB888Color(0xff00ff).toRGB565();
		green = RGB888Color(0x00FF00).toRGB565();
		yellow = RGB888Color(0xffd200).toRGB565();
		ocean = RGB888Color(0x00ffff).toRGB565();

		speedBarWhite = RGB888Color(0xFFFFFF).toRGB565();
		speedBarGreen = RGB888Color(0x008100).toRGB565();
		speedBarYellow = RGB888Color(0xffd200).toRGB565();
		speedBarRed = RGB888Color(0xff3333).toRGB565();
	}

	void Theme::setDark() {
		bg1 = RGB888Color(0x000000).toRGB565();
		bg2 = RGB888Color(0x0C0C0C).toRGB565();
		bg3 = RGB888Color(0x181818).toRGB565();
		bg4 = RGB888Color(0x242424).toRGB565();
		bg5 = RGB888Color(0x484848).toRGB565();
		bg6 = RGB888Color(0x545454).toRGB565();
		bg7 = RGB888Color(0x606060).toRGB565();

		fg1 = RGB888Color(0xF0F0F0).toRGB565();
		fg2 = RGB888Color(0xCCCCCC).toRGB565();
		fg3 = RGB888Color(0x999999).toRGB565();
		fg4 = RGB888Color(0x888888).toRGB565();
		fg5 = RGB888Color(0x777777).toRGB565();
		fg6 = RGB888Color(0x666666).toRGB565();
		fg7 = RGB888Color(0x555555).toRGB565();

		bgRed1 = RGB888Color(0x230000).toRGB565();
		bgRed2 = RGB888Color(0x340000).toRGB565();
		bgRed3 = RGB888Color(0x490000).toRGB565();

		fgRed1 = RGB888Color(0xffff00).toRGB565();

		setCommon();
	}

	void Theme::setBright() {
		bg1 = RGB888Color(0xFFFFFF).toRGB565();
		bg2 = RGB888Color(0xEEEEEE).toRGB565();
		bg3 = RGB888Color(0xDDDDDD).toRGB565();
		bg4 = RGB888Color(0xCCCCCC).toRGB565();
		bg5 = RGB888Color(0x999999).toRGB565();
		bg6 = RGB888Color(0x888888).toRGB565();
		bg7 = RGB888Color(0x777777).toRGB565();

		fg1 = RGB888Color(0x000000).toRGB565();
		fg2 = RGB888Color(0x0C0C0C).toRGB565();
		fg3 = RGB888Color(0x181818).toRGB565();
		fg4 = RGB888Color(0x242424).toRGB565();
		fg5 = RGB888Color(0x484848).toRGB565();
		fg6 = RGB888Color(0x545454).toRGB565();
		fg7 = RGB888Color(0x606060).toRGB565();

		bgRed1 = RGB888Color(0x230000).toRGB565();
		bgRed2 = RGB888Color(0x340000).toRGB565();
		bgRed3 = RGB888Color(0x490000).toRGB565();

		fgRed1 = RGB888Color(0xffff00).toRGB565();

		setCommon();
	}
}
