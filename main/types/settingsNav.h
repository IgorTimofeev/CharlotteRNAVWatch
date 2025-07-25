#pragma once

#include <YOBA/main.h>
#include <YOBANVS/main.h>

namespace pizda {
	using namespace YOBA;

	class SettingsNavWaypoint {
		public:
			explicit SettingsNavWaypoint(
				const std::wstring_view name,
				const GeographicCoordinates& coordinates
			) :
				coordinates(coordinates)
			{
				std::wcscpy(this->name, name.data());
			}

			SettingsNavWaypoint() : SettingsNavWaypoint(L"", {}) {

			}

			wchar_t name[16];
			GeographicCoordinates coordinates;
	};

	enum class SettingsNavPerformanceProfile : uint8_t {
		onFoot,
		cycling,
		diamondDA40
	};

	class SettingsNav : public NVSSettings {
		public:
			std::vector<SettingsNavWaypoint> waypoints {};

			uint16_t waypoint1Index = 0;
			uint16_t waypoint1CourseDeg = 0;
			uint16_t waypoint2Index = 0;

			SettingsNavPerformanceProfile performanceProfile = SettingsNavPerformanceProfile::cycling;
			uint16_t samplingDistanceM = 0;

		protected:
			const char* getNVSNamespace() override {
				return "nav";
			}

			void onRead(const NVSStream& stream) override {
				// Waypoints list
				{
					const auto waypointsSize = stream.getUint16(_waypointsSize, 0);

					// Previously saved data
					if (waypointsSize > 0) {
						const auto NVSWaypoints = std::make_unique<SettingsNavWaypoint[]>(waypointsSize);
						stream.getObject(_waypointsList, NVSWaypoints.get(), waypointsSize);

						waypoints.reserve(waypointsSize);

						for (uint16_t i = 0; i < waypointsSize; i++) {
							const auto& waypoint = NVSWaypoints[i];

							waypoints.push_back(SettingsNavWaypoint(
								waypoint.name,
								waypoint.coordinates
							));
						}
					}
					// Template data
					else {
						waypoints.push_back(SettingsNavWaypoint(
							L"HOME",
							GeographicCoordinates(toRadians(59.81146609852194f), toRadians(30.563501508879945f), 0)
						));

						waypoints.push_back(SettingsNavWaypoint(
							L"TSRKV",
							GeographicCoordinates(toRadians(59.8045049291447), toRadians(30.602001249170907), 0)
						));

						waypoints.push_back(SettingsNavWaypoint(
							L"ULLI",
							GeographicCoordinates(toRadians(59.800278f), toRadians(30.2625f), 0)
						));
					}
				}

				waypoint1Index = stream.getUint16(_navWaypointIndex, 0);
				waypoint1CourseDeg = stream.getUint16(_navWaypointCourse, 0);

				waypoint2Index = stream.getUint16(_bearingWaypointIndex, 1);

				performanceProfile = static_cast<SettingsNavPerformanceProfile>(stream.getUint8(_performanceProfile, static_cast<uint8_t>(SettingsNavPerformanceProfile::cycling)));
				samplingDistanceM = stream.getUint16(_samplingDistanceM, 10);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint16(_navWaypointIndex, waypoint1Index);
				stream.setUint16(_navWaypointCourse, waypoint1CourseDeg);

				stream.setUint16(_bearingWaypointIndex, waypoint2Index);

				stream.setUint8(_performanceProfile, static_cast<uint8_t>(performanceProfile));
				stream.setUint16(_samplingDistanceM, samplingDistanceM);
			}

		private:
			constexpr static auto _navWaypointIndex = "nw";
			constexpr static auto _navWaypointCourse = "wc";

			constexpr static auto _bearingWaypointIndex = "bw";

			constexpr static auto _waypointsSize = "ws";
			constexpr static auto _waypointsList = "wl";

			constexpr static auto _performanceProfile = "pp";
			constexpr static auto _samplingDistanceM = "sd";
	};
}
