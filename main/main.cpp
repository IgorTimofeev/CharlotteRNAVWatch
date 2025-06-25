#include <rc.h>

extern "C" void app_main(void) {
	pizda::RC::getInstance().run();
}