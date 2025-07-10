#pragma once

#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class Route {
		public:
			virtual ~Route() = default;
			virtual Element* buildElement() const = 0;
	};

	template<typename T>
	class ElementRoute : public Route {
		public:
			Element* buildElement() const override;
	};

	template<typename T>
	Element* ElementRoute<T>::buildElement() const {
		return new T();
	}
}