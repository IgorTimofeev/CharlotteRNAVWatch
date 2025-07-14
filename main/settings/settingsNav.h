#pragma once

#include <YOBA/main.h>
#include <YOBANVS/main.h>

namespace pizda {
	class SettingsNavWaypoint {
		public:
			SettingsNavWaypoint() = default;

			explicit SettingsNavWaypoint(
				std::wstring_view name,
				const GeographicCoordinates& geographicCoordinates
			) :
				geographicCoordinates(geographicCoordinates)
			{
				std::ranges::copy(name, this->name);
			}

			wchar_t name[16] {};
			GeographicCoordinates geographicCoordinates;
	};

	class SettingsNav : public NVSSettings {
		public:
			std::vector<SettingsNavWaypoint> waypoints {};

			uint16_t navWaypointIndex = 0;
			uint16_t navWaypointCourseDeg = 0;

			uint16_t bearingWaypointIndex = 0;

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
								waypoint.geographicCoordinates
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

				navWaypointIndex = stream.getUint16(_navWaypointIndex, 0);
				navWaypointCourseDeg = stream.getUint16(_navWaypointCourse, 0);

				bearingWaypointIndex = stream.getUint16(_bearingWaypointIndex, 1);
			}

			void onWrite(const NVSStream& stream) override {
				stream.setUint16(_navWaypointIndex, navWaypointIndex);
				stream.setUint16(_navWaypointCourse, navWaypointCourseDeg);

				stream.setUint16(_bearingWaypointIndex, bearingWaypointIndex);
			}

		private:
			constexpr static auto _navWaypointIndex = "nw";
			constexpr static auto _navWaypointCourse = "wc";

			constexpr static auto _bearingWaypointIndex = "bw";

			constexpr static auto _waypointsSize = "ws";
			constexpr static auto _waypointsList = "wl";
	};
}
