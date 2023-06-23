#pragma once

#include <cmath>

namespace geo {

    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }

        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;

        if (from == to) {
            return 0;
        }

        const int EARTH_RADIUS = 6371000;
        const double PI = 3.1415926535;
        const double HALF_SPHERE_DEGREES = 180.;

        static const double dr = PI / HALF_SPHERE_DEGREES;

        return acos(
            sin(from.lat * dr) * sin(to.lat * dr) +
            cos(from.lat * dr) * cos(to.lat * dr) *
            cos(abs(from.lng - to.lng) * dr)
        ) * EARTH_RADIUS;
    }

} // namespace geo