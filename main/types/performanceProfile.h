#pragma once

#include <YOBA/main.h>
#include "UI/theme.h"

namespace pizda {
	using namespace YOBA;

	class SpeedBar {
		public:
			float from;
			float to;
			const Color* color;
	};

	class SpeedBug {
		public:
			std::wstring name;
			float speed;
	};

	class PerformanceProfileSpeed {
		public:
			uint8_t step = 0;
			uint8_t stepBig = 0;
			float radPerUnit = 0;

			std::vector<SpeedBar> bars = std::vector<SpeedBar>(5);
			std::vector<SpeedBug> bugs = std::vector<SpeedBug>(5);
	};

	class PerformanceProfileAltitude {
		public:
			uint16_t step = 0;
			uint16_t stepBig = 0;
			float radPerUnit = 0;
	};

	class PerformanceProfileAHRS {
		public:
			uint16_t dataSamplingDistanceM = 0;
			uint32_t dataSamplingIntervalUs = 0;
	};

	class PerformanceProfile {
		public:
			PerformanceProfileSpeed speed {};
			PerformanceProfileAltitude altitude{};
			PerformanceProfileAHRS AHRS {};

			void setOnFoot() {
				// Speeed
				speed.step = 1;
				speed.stepBig = 2;
				speed.radPerUnit = toRadians(10.0f);

				speed.bars.clear();
				speed.bars.push_back(SpeedBar { 0, 9, &Theme::speedBarGreen });
				speed.bars.push_back(SpeedBar { 9, 13, &Theme::speedBarYellow });
				speed.bars.push_back(SpeedBar { 13, 40, &Theme::speedBarRed });

				speed.bugs.clear();
				speed.bugs.push_back(SpeedBug { L"Opt", 3 });

				// Altitude
				altitude.step = 10;
				altitude.stepBig = 50;
				altitude.radPerUnit = toRadians(0.5f);

				// AHRS
				AHRS.dataSamplingDistanceM = 8;
				AHRS.dataSamplingIntervalUs = 20'000'000;
			}

			void setCycling() {
				// Speed
				speed.step = 1;
				speed.stepBig = 5;
				speed.radPerUnit = toRadians(4.0f);

				speed.bars.clear();
				speed.bars.push_back(SpeedBar { 0, 5, &Theme::speedBarWhite });
				speed.bars.push_back(SpeedBar { 5, 10, &Theme::speedBarGreen });
				speed.bars.push_back(SpeedBar { 10, 18, &Theme::speedBarYellow });
				speed.bars.push_back(SpeedBar { 18, 40, &Theme::speedBarRed });

				speed.bugs.clear();
				speed.bugs.push_back(SpeedBug { L"Opt", 13 });

				// Altitude
				altitude.step = 10;
				altitude.stepBig = 50;
				altitude.radPerUnit = toRadians(0.5f);

				// AHRS
				AHRS.dataSamplingDistanceM = 10;
				AHRS.dataSamplingIntervalUs = 10'000'000;
			}

			void setDiamondDA40() {
				// Speed
				speed.step = 5;
				speed.stepBig = 10;
				speed.radPerUnit = toRadians(1.6f);

				speed.bars.clear();
				speed.bars.push_back(SpeedBar { 0, 49, &Theme::speedBarRed });
				speed.bars.push_back(SpeedBar { 49, 60, &Theme::speedBarWhite });
				speed.bars.push_back(SpeedBar { 60, 170, &Theme::speedBarGreen });
				speed.bars.push_back(SpeedBar { 170, 190, &Theme::speedBarYellow });
				speed.bars.push_back(SpeedBar { 190, 300, &Theme::speedBarRed });

				speed.bugs.clear();
				speed.bugs.push_back(SpeedBug { L"G", 76 });
				speed.bugs.push_back(SpeedBug { L"Y", 68 });
				speed.bugs.push_back(SpeedBug { L"R", 59 });

				// Altitude
				altitude.step = 50;
				altitude.stepBig = 100;
				altitude.radPerUnit = toRadians(0.15f);

				// AHRS
				AHRS.dataSamplingDistanceM = 20;
				AHRS.dataSamplingIntervalUs = 8'000'000;
			}
	};
}
