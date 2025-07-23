#include <rc.h>

using namespace pizda;

extern "C" void app_main(void) {
	auto& rc = RC::getInstance();

	rc.setup();

	// ReSharper disable once CppDFAEndlessLoop
	while (true)
		rc.tick();
}
