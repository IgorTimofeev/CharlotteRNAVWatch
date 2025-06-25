#pragma once

#include <ranges>
#include <locale>
#include <codecvt>
#include <cstdlib>

namespace pizda {
	class StringUtils {
		public:
			static bool containsIgnoreCase(std::wstring_view haystack, std::wstring_view needle) {
				return !std::ranges::search(
					haystack,
					needle,
					[](wchar_t a, wchar_t b) {
						return std::towlower(a) == std::towlower(b);
					}
				).empty();
			}

			static std::wstring toWString(const std::string& source) {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converterX;

				return converterX.from_bytes(source.data(), source.data() + source.size());
			}

			static std::string toString(const std::wstring_view& source) {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converterX;

				return converterX.to_bytes(source.data(), source.data() + source.size());
			}
	};
}