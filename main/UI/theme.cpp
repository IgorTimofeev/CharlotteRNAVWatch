#include "theme.h"

namespace pizda {
	const MonochromeColor Theme::b { false };
	const MonochromeColor Theme::w { true };

	const Vaticanus8Font Theme::fontSmall {};

	void Theme::setup(Bit8PaletteRenderer* renderer) {

	}

	void Theme::apply(Text* text) {
		text->setFont(&Theme::fontSmall);
		text->setTextColor(&Theme::w);
	}
}
