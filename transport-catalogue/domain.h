#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>


namespace trans_cat {

    struct Stop {
        std::string stop_name;
        geo::Coordinates coordinates;
        std::set<std::string> buses_on_this_stop;
    };

    struct Bus {
        std::string bus_name;
        std::vector<const Stop*> stops_of_this_bus;
        bool is_circle;
    };

    struct BusStat {
        size_t stops_count;
        size_t unique_stops_count;
        int route_length;
        double curvature;
    };

} // namespace trans_cat