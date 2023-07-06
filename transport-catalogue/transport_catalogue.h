#pragma once

#include "geo.h"
#include "domain.h"

#include <utility>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <iostream>
#include <stdexcept>
#include <functional>


namespace trans_cat {

class TransportCatalogue {
private:
    struct StopDistancesHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
            size_t hash_first = std::hash<const void*>{}(points.first);
            size_t hash_second = std::hash<const void*>{}(points.second);
            return hash_first + hash_second * 37;
        }
    };

    std::deque<Bus> all_buses_;
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopDistancesHasher> stop_distances_;


public:
    void AddBus(const std::string_view& bus_name, const std::vector<const Stop*>& stops, bool circular);
    void AddStop(const std::string_view& stop_name, geo::Coordinates& coordinates);

    const Bus* FindBus(const std::string_view& bus_name) const;
    const Stop* FindStop(const std::string_view& stop_name) const;

    size_t UniqueStopsCount(const std::string_view& bus_name) const;
    const std::map<std::string_view, const Bus*> GetAllBuses() const;

    void SetDistance(const Stop* from, const Stop* to, const int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
};

} // namespace trans_cat