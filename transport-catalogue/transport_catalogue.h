#pragma once

#include "geo.h"
#include "domain.h"

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <iostream>
#include <stdexcept>

namespace trans_cat {

class TransCatalogue {
public:
    void AddStop(std::string_view stop_name, const geo::Coordinates coordinates);
    void AddBus(std::string_view bus_number, const std::vector<const Stop*> stops, bool is_circle);
    
    const Stop* FindStop(std::string_view stop_name) const;
    const Bus* FindBus(std::string_view bus_number) const;
    
    void SetDistance(const Stop* from, const Stop* to, const int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
    
    size_t UniqueStopsCount(std::string_view bus_number) const;
    
    const std::map<std::string_view, const Stop*> GetSortedAllStops() const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;

private:
    struct StopDistancesHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
            size_t hash = std::hash<const void*>{}(points.first)
                        + std::hash<const void*>{}(points.second) * 37;
            return hash;
        }
    };
    
    struct StopHasher {
        size_t operator()(const Stop* stop) const {
            size_t hash = std::hash<const void*>{}(stop);
            return hash;
        }
    };
    
    std::deque<Stop> all_stops_;
    std::deque<Bus> all_buses_;
    
    trans_cat::StopnameIds stopname_to_id_; // domain.h
    
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopDistancesHasher> stop_distances_;
};

}  // namespace transport