#pragma once

#include <cstdint>
#include <esp_timer.h>
#include <esp_log.h>
#include <memory>
#include <cstring>
#include <esp_log.h>

#include "nvs_flash.h"
#include "nvs.h"

namespace pizda {
	class NVSStream {
		public:
			void openForWriting(const char* key) {
				ESP_ERROR_CHECK(nvs_open(key, NVS_READWRITE, &_handle));
			}

			void openForReading(const char* key) {
				const auto status = nvs_open(key, NVS_READONLY, &_handle);
				assert(status == ESP_OK || status == ESP_ERR_NVS_NOT_FOUND);
			}

			void commit() const {
				ESP_ERROR_CHECK(nvs_commit(_handle));
			}

			void close() const {
				nvs_close(_handle);
			}

			uint8_t getUint8(const char* key, uint8_t defaultValue = 0) const {
				return getValue<uint8_t, uint8_t, nvs_get_u8>(key, defaultValue);
			}

			void setUint8(const char* key, uint8_t value) const {
				setValue<uint8_t, nvs_set_u8>(key, value);
			}

			uint16_t getUint16(const char* key, uint16_t defaultValue = 0) const {
				return getValue<uint16_t, uint16_t, nvs_get_u16>(key, defaultValue);
			}

			void setUint16(const char* key, uint16_t value) const {
				setValue<uint16_t, nvs_set_u16>(key, value);
			}

			uint32_t getUint32(const char* key, uint32_t defaultValue = 0) const {
				return getValue<uint32_t, uint32_t, nvs_get_u32>(key, defaultValue);
			}

			void setUint32(const char* key, uint32_t value) const {
				setValue<uint32_t, nvs_set_u32>(key, value);
			}

			uint64_t getUint64(const char* key, uint64_t defaultValue = 0) const {
				return getValue<uint64_t, uint64_t, nvs_get_u64>(key, defaultValue);
			}

			void setUint64(const char* key, uint64_t value) const {
				setValue<uint64_t, nvs_set_u64>(key, value);
			}

			float getFloat(const char* key, float defaultValue = 0) const {
				const auto u32 = getUint32(key, defaultValue);

				float result;
				std::memcpy(&result, &u32, sizeof(float));
				return result;
			}

			void setFloat(const char* key, float value) const {
				uint32_t u32;
				std::memcpy(&u32, &value, sizeof(float));
				setUint32(key, u32);
			}

			bool getBool(const char* key, bool defaultValue = false) const {
				return getValue<bool, uint8_t, nvs_get_u8>(key, defaultValue);
			}

			void setBool(const char* key, bool value) const {
				setValue<bool, nvs_set_u8>(key, value);
			}

			std::string getString(const char* key, const std::string& defaultValue = std::string()) const {
				return getStringT<char>(key, defaultValue);
			}

			void setString(const char* key, const std::string& value) const {
				setStringT<char>(key, value);
			}

			void getBlob(const char* key, uint8_t* data, size_t length) const {
				size_t lengthCopy = length;
				ESP_ERROR_CHECK(nvs_get_blob(_handle, key, data, &lengthCopy));
			}

			void setBlob(const char* key, const uint8_t* data, size_t length) const {
				ESP_ERROR_CHECK(nvs_set_blob(_handle, key, data, length));
			}

			template<typename T>
			void getObject(const char* key, T* data, size_t length) const {
				getBlob(
					key,
					reinterpret_cast<uint8_t*>(data),
					sizeof(T) * length
				);
			}

			template<typename T>
			void setObject(const char* key, const T* data, size_t length) const {
				setBlob(
					key,
					reinterpret_cast<const uint8_t*>(data),
					sizeof(T) * length
				);
			}

			void testForBullshit() {
				ESP_LOGI("NVS test", "Writing");

				openForWriting("pizda");
				setUint8("uint8Test", 123);
				setUint16("uint16Test", 12345);
				setUint32("uint32Test", 12345);
				setFloat("floatTest", 123.456);
				setString("stringTest", "Pizda penisa");
				commit();
				close();

				ESP_LOGI("NVS test", "Reading");

				openForReading("pizda");
				ESP_LOGI("NVS test", "Value: %d", getUint8("uint8Test"));
				ESP_LOGI("NVS test", "Value: %d", getUint16("uint16Test"));
				ESP_LOGI("NVS test", "Value: %lu", getUint32("uint32Test"));
				ESP_LOGI("NVS test", "Value: %f", getFloat("floatTest"));
				ESP_LOGI("NVS test", "Value: %s", getString("stringTest").c_str());
				close();
			}

		private:
			nvs_handle_t _handle {};

			template<typename TResult, typename TGet, auto Function>
			TResult getValue(const char* key, TResult defaultValue = 0) const {
				TGet got;

				if (Function(_handle, key, &got) == ESP_OK)
					return static_cast<TResult>(got);

				return defaultValue;
			}

			template<typename TValue, auto Function>
			void setValue(const char* key, TValue value) const {
				ESP_ERROR_CHECK(Function(_handle, key, value));
			}

			template<typename TChar>
			std::string getStringT(const char* key, const std::basic_string<TChar>& defaultValue = std::basic_string<TChar>()) const {
				size_t bufferSize = 0;

				if (nvs_get_blob(_handle, key, nullptr, &bufferSize) != ESP_OK)
					return defaultValue;

				uint8_t buffer[bufferSize] {};

				if (nvs_get_blob(_handle, key, &buffer[0], &bufferSize) != ESP_OK)
					return defaultValue;

				return std::string(
					reinterpret_cast<TChar*>(buffer),
					(bufferSize / sizeof(TChar)) - 1
				);
			}

			template<typename TChar>
			void setStringT(const char* key, const std::basic_string<TChar>& value) const {
				ESP_ERROR_CHECK(nvs_set_blob(
					_handle,
					key,
					reinterpret_cast<const uint8_t*>(value.data()),
					(value.size() + 1) * sizeof(char)
				));
			}
	};
}