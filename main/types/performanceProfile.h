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

	class PerformanceProfile {
		public:
			// Speed
			uint8_t speedStep = 0;
			uint8_t speedStepBig = 0;
			float speedRadPerUnit = 0;

			// Altitude
			uint16_t altitudeStep = 0;
			uint16_t altitudeStepBig = 0;
			float altitudeRadPerUnit = 0;

			constexpr static uint16_t speedFlapsMin = 40;
			constexpr static uint16_t speedFlapsMax = 85;

			constexpr static uint16_t speedSmoothMin = 47;
			constexpr static uint16_t speedSmoothMax = 128;

			constexpr static uint16_t speedTurbulentMin = speedSmoothMax;
			constexpr static uint16_t speedTurbulentMax = 163;

			constexpr static uint16_t speedStructuralMin = speedTurbulentMax;
			constexpr static uint16_t speedStructuralMax = speedStructuralMin * 4;

			std::vector<SpeedBar> speedBars = std::vector<SpeedBar>(5);
			std::vector<SpeedBug> speedBugs = std::vector<SpeedBug>(5);

			void setCycling() {
				speedStep = 1;
				speedStepBig = 5;
				speedRadPerUnit = toRadians(4.0f);

				altitudeStep = 10;
				altitudeStepBig = 50;
				altitudeRadPerUnit = toRadians(0.5f);

				speedBars.clear();
				speedBars.push_back(SpeedBar { 0, 5, &Theme::speedBarWhite });
				speedBars.push_back(SpeedBar { 5, 10, &Theme::speedBarGreen });
				speedBars.push_back(SpeedBar { 10, 18, &Theme::speedBarYellow });
				speedBars.push_back(SpeedBar { 18, 40, &Theme::speedBarRed });

				speedBugs.clear();
				speedBugs.push_back(SpeedBug { L"X", 13 });
			}

			void setDiamondDA40() {
				speedStep = 5;
				speedStepBig = 10;
				speedRadPerUnit = toRadians(0.8f);

				altitudeStep = 50;
				altitudeStepBig = 100;
				altitudeRadPerUnit = toRadians(0.1f);

				speedBars.clear();
				speedBars.push_back(SpeedBar { 0, 49, &Theme::speedBarRed });
				speedBars.push_back(SpeedBar { 49, 60, &Theme::speedBarWhite });
				speedBars.push_back(SpeedBar { 60, 170, &Theme::speedBarGreen });
				speedBars.push_back(SpeedBar { 170, 190, &Theme::speedBarYellow });
				speedBars.push_back(SpeedBar { 190, 300, &Theme::speedBarRed });

				speedBugs.clear();
				speedBugs.push_back(SpeedBug { L"G", 76 });
				speedBugs.push_back(SpeedBug { L"Y", 68 });
				speedBugs.push_back(SpeedBug { L"R", 59 });
			}
	};
}
