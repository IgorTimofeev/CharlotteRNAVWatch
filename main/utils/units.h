#pragma once

#include <cstdint>

namespace pizda {
	enum class AltimeterMode : uint8_t {
		QNH,
		QNE
	};

	enum class SpeedUnit : uint8_t {
		meterPerSecond,
		knot
	};

	enum class DistanceUnit : uint8_t {
		meter,
		foot,
		nauticalMile
	};

	enum class PressureUnit : uint8_t {
		pascal,
		hectopascal,
		inHg
	};

	class Units {
		public:
			constexpr static float convertSpeed(float value, SpeedUnit fromUnit, SpeedUnit toUnit) {
				switch (fromUnit) {
					case SpeedUnit::meterPerSecond: {
						switch (toUnit) {
							case SpeedUnit::meterPerSecond: return value;
							default: return value / 0.5144444444f;
						}
					}
					default: {
						switch (toUnit) {
							case SpeedUnit::meterPerSecond: return value * 0.5144444444f;
							default: return value;
						}
					}
				}
			}

			constexpr static float convertDistance(float value, DistanceUnit fromUnit, DistanceUnit toUnit) {
				switch (fromUnit) {
					case DistanceUnit::meter: {
						switch (toUnit) {
							case DistanceUnit::meter: return value;
							case DistanceUnit::foot: return value / 0.3048f;
							default: return value / 1852.f;
						}
					}
					case DistanceUnit::foot: {
						switch (toUnit) {
							case DistanceUnit::meter: return value * 0.3048f;
							case DistanceUnit::foot: return value;
							default: return value / 6076.f;
						}
					}
					default: {
						switch (toUnit) {
							case DistanceUnit::meter: return value * 1852.f;
							case DistanceUnit::foot: return value * 6076.f;
							default: return value;
						}
					}
				}
			}

			constexpr static float convertPressure(float value, PressureUnit fromUnit, PressureUnit toUnit) {
				switch (fromUnit) {
					case PressureUnit::pascal: {
						switch (toUnit) {
							case PressureUnit::pascal: return value;
							case PressureUnit::hectopascal: return value / 100.f;
							default: return value / 3376.85f;
						}
					}
					case PressureUnit::hectopascal: {
						switch (toUnit) {
							case PressureUnit::pascal: return value * 100.f;
							case PressureUnit::hectopascal: return value;
							default: return value * 100.f / 3376.85f;
						}
					}
					default: {
						switch (toUnit) {
							case PressureUnit::pascal: return value * 3376.85f;
							case PressureUnit::hectopascal: return value * 3376.85f / 100.f;
							default: return value;
						}
					}
				}
			}
	};
}