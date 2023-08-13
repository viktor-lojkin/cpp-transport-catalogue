#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace trans_cat {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::set<std::string> buses_by_stop;
};

struct Bus {
    std::string number;
    std::vector<const Stop*> stops;
    bool is_circle;
};

struct BusStat {
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double curvature;
};
    
struct StopnameIds{
    std::unordered_map<std::string_view, int> stopname_ids;
    size_t count = 0;
};

} // namespace transport