#pragma once

#include "geo.h"

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace domain {

struct Stop {
    Stop(const std::string& name, const geo::Coordinates& coordinates)
        : name(name)
        , coordinates(coordinates) {
    }
    
    int GetDistance(Stop* to) {
        if (stop_distances.count(to->name))
            return stop_distances.at(to->name);
        else if (to->stop_distances.count(this->name))
            return to->stop_distances.at(this->name);
        else
            return 0;
    }

    std::string name;
    geo::Coordinates coordinates;
    std::unordered_map<std::string_view, int> stop_distances;
};

struct Bus {
    Bus(const std::string& name, std::vector<Stop*> stops, bool is_circle)
        : name(name)
        , stops(stops)
        , is_circle(is_circle) {
    }

    std::string name;
    std::vector<Stop*> stops;
    bool is_circle;
    Stop* final_stop = nullptr;
};  

} //namespace domain