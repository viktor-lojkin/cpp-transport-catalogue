#pragma once

#include "geo.h"

#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <stdexcept>
#include <functional>
#include <string_view>


namespace trans_cat {

    struct Stop {
        std::string stop_name;
        geo::Coordinates coordinates;
        std::set<std::string> buses_on_this_stop;
    };

    struct Bus {
        std::string bus_name;
        std::vector<const Stop*> stops_of_this_bus;
        bool circular;
    };

    struct BusStat {
        size_t stops_count;
        size_t unique_stops_count;
        int route_length;
        double curvature;
    };

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
        void AddBus(const std::string& bus_name, const std::vector<const Stop*> stops, bool circular);
        void AddStop(const std::string& stop_name, geo::Coordinates& coordinates);

        const Bus* FindBus(const std::string& bus_name) const;
        const Stop* FindStop(const std::string& stop_name) const;
        const BusStat BusInformation(const std::string& bus_name) const;

        const std::set<std::string> GetBusesOnStop(const std::string& stop_name) const;

        size_t UniqueStopsCount(const std::string& bus_name) const;

        void SetDistance(const Stop* from, const Stop* to, const int distance);
        int GetDistance(const Stop* from, const Stop* to) const;
    };

} // namespace trans_cat