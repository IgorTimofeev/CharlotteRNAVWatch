#pragma once

#include <cstdint>

#include <hardware/NVS/NVSStream.h>
#include <hardware/NVS/NVSSerializable.h>

namespace pizda {
	class SettingsInterface : public NVSSerializable {
		public:
			bool eblo = false;

		protected:
			const char* getNVSNamespace() override {
				return _namespace;
			}

			void onRead(const NVSStream& stream) override {
				eblo = stream.getBool(_eblo, true);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setBool(_eblo, eblo);
			}

		private:
			constexpr static auto _namespace = "if";
			constexpr static auto _eblo = "eblo";
	};
}
