#pragma once

namespace pizda {
	class LowPassFilter {
		public:
			static void apply(float& value, const float targetValue, const float factor) {
				value = value * (1.f - factor) + targetValue * factor;
			}
	};
}