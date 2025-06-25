#include "eblo.h"

#include "rc.h"
#include "UI/theme.h"

namespace pizda {
	void Eblo::onTick() {
		invalidate();
	}

	void Eblo::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& rc = RC::getInstance();

		constexpr uint8_t spacing = 1;
		uint16_t x = 2;
		uint16_t y = 0;

		renderer->renderString(Point(x, y), &Theme::fontSmall, &Theme::w, std::format(L"Sat/HDOP: {}, {}", rc.gps.getSatellites(), rc.gps.getHDOP()));
		y += Theme::fontSmall.getHeight() + spacing;

		renderer->renderString(Point(x, y), &Theme::fontSmall, &Theme::w, std::format(L"Time: {:02}.{:02}.{:04} {:02}:{:02}:{:02}", rc.gps.getDateDay(), rc.gps.getDateMonth(), rc.gps.getDateYear(), rc.gps.getTimeHours(), rc.gps.getTimeMinutes(), rc.gps.getTimeSeconds()));
		y += Theme::fontSmall.getHeight() + spacing;

		renderer->renderString(Point(x, y), &Theme::fontSmall, &Theme::w, std::format(L"Lat/lon: {}, {}", rc.gps.getLatitude(), rc.gps.getLongitude()));
		y += Theme::fontSmall.getHeight() + spacing;

		renderer->renderString(Point(x, y), &Theme::fontSmall, &Theme::w, std::format(L"Alt: {} ft", rc.gps.getAltitudeFt()));
		y += Theme::fontSmall.getHeight() + spacing;

		renderer->renderString(Point(x, y), &Theme::fontSmall, &Theme::w, std::format(L"SPD/CRS: {}, {}", rc.gps.getSpeedKt(), rc.gps.getCourseDeg()));
		y += Theme::fontSmall.getHeight() + spacing;

	}
}
