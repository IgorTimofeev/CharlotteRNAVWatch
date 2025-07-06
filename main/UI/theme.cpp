#include "theme.h"

namespace pizda {
	const RGB565Color Theme::bg1 = RGB888Color(0x000000).toRGB565();
	const RGB565Color Theme::bg2 = RGB888Color(0x0C0C0C).toRGB565();
	const RGB565Color Theme::bg3 = RGB888Color(0x181818).toRGB565();
	const RGB565Color Theme::bg4 = RGB888Color(0x242424).toRGB565();
	const RGB565Color Theme::bg5 = RGB888Color(0x484848).toRGB565();
	const RGB565Color Theme::bg6 = RGB888Color(0x545454).toRGB565();
	const RGB565Color Theme::bg7 = RGB888Color(0x606060).toRGB565();

	const RGB565Color Theme::fg1 = RGB888Color(0xF0F0F0).toRGB565();
	const RGB565Color Theme::fg2 = RGB888Color(0xCCCCCC).toRGB565();
	const RGB565Color Theme::fg3 = RGB888Color(0x999999).toRGB565();
	const RGB565Color Theme::fg4 = RGB888Color(0x888888).toRGB565();
	const RGB565Color Theme::fg5 = RGB888Color(0x777777).toRGB565();
	const RGB565Color Theme::fg6 = RGB888Color(0x666666).toRGB565();
	const RGB565Color Theme::fg7 = RGB888Color(0x555555).toRGB565();

	const RGB565Color Theme::accent1 = RGB888Color(0xffd200).toRGB565();
	const RGB565Color Theme::accent2 = RGB888Color(0x997e53).toRGB565();

	const RGB565Color Theme::good1 = RGB888Color(0x1db153).toRGB565();
	const RGB565Color Theme::good2 = RGB888Color(0x28DA69).toRGB565();

	const RGB565Color Theme::bad1 = RGB888Color(0x9f2d2d).toRGB565();
	const RGB565Color Theme::bad2 = RGB888Color(0xf24949).toRGB565();

	const RGB565Color Theme::red = RGB888Color(0xff0000).toRGB565();
	const RGB565Color Theme::purple = RGB888Color(0xff00ff).toRGB565();
	const RGB565Color Theme::green = RGB888Color(0x00FF00).toRGB565();
	const RGB565Color Theme::greenSpeed = RGB888Color(0x008100).toRGB565();
	const RGB565Color Theme::yellow = RGB888Color(0xffd200).toRGB565();
	const RGB565Color Theme::ocean = RGB888Color(0x00ffff).toRGB565();
	const RGB565Color Theme::ground = RGB888Color(0x343f15).toRGB565();
	const RGB565Color Theme::ground2 = RGB888Color(0xdef2a2).toRGB565();
	const RGB565Color Theme::sky = RGB888Color(0x1d2d3c).toRGB565();
	const RGB565Color Theme::sky2 = RGB888Color(0xaed1f2).toRGB565();

	const unscii16Font Theme::fontBig {};
	const PIXY10Font Theme::fontNormal {};
	const Vaticanus8Font Theme::fontSmall {};

	void Theme::apply(Text* text) {
		text->setFont(&Theme::fontSmall);
		text->setTextColor(&Theme::fg1);
	}
}
