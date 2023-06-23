#include "transport_catalogue.h"

#include <algorithm>

namespace trans_cat {

    void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates& coordinates) {
        all_stops_.push_back({ stop_name, coordinates, {} });
        stopname_to_stop_[all_stops_.back().stop_name] = &all_stops_.back();
    }

    void TransportCatalogue::AddBus(const std::string& bus_name, const std::vector<const Stop*> stops, bool circular) {
        all_buses_.push_back({ bus_name, stops, circular });
        busname_to_bus_[all_buses_.back().bus_name] = &all_buses_.back();

        for (const auto& stop_ptr : stops) {
            std::for_each(
                all_stops_.begin(), all_stops_.end(),
                [&bus_name, &stop_ptr](auto& stop_) {
                    if (stop_.stop_name == stop_ptr->stop_name) {
                        stop_.buses_on_this_stop.insert(bus_name);
                    }
                }
            );
        }
    }

    const Bus* TransportCatalogue::FindBus(const std::string& bus_name) const {
        return busname_to_bus_.count(bus_name) ? busname_to_bus_.at(bus_name) : nullptr;
    }

    const Stop* TransportCatalogue::FindStop(const std::string& stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }

    const BusStat TransportCatalogue::BusInformation(const std::string& bus_name) const {
        BusStat bus_stat{};
        const Bus* bus = FindBus(bus_name);

        if (!bus) {
            throw std::invalid_argument("Bus not found");
        }

        if (bus->circular) {
            bus_stat.stops_count = bus->stops_of_this_bus.size();
        } else {
            bus_stat.stops_count = bus->stops_of_this_bus.size() * 2 - 1;
        }

        double geo_dist = 0.0;
        int route_length = 0;
        for (size_t i = 0; i < bus->stops_of_this_bus.size() - 1; ++i) {
            auto from = bus->stops_of_this_bus[i];
            auto to = bus->stops_of_this_bus[i + 1];
            if (bus->circular) {
                route_length += GetDistance(from, to);
                geo_dist += ComputeDistance(from->coordinates, to->coordinates);
            } else {
                route_length += GetDistance(from, to) + GetDistance(to, from);
                geo_dist += ComputeDistance(from->coordinates, to->coordinates) * 2;
            }
        }

        bus_stat.unique_stops_count = UniqueStopsCount(bus_name);
        bus_stat.route_length = route_length;
        bus_stat.curvature = route_length / geo_dist;

        return bus_stat;
    }

    const std::set<std::string> TransportCatalogue::GetBusesOnStop(const std::string& stop_name) const {
        return stopname_to_stop_.at(stop_name)->buses_on_this_stop;
    }

    size_t TransportCatalogue::UniqueStopsCount(const std::string& bus_name) const {
        std::unordered_set<std::string> unique_stops;
        for (const auto& stop : busname_to_bus_.at(bus_name)->stops_of_this_bus) {
            unique_stops.insert(stop->stop_name);
        }
        return unique_stops.size();
    }

    void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, const int distance) {
        stop_distances_[{from, to}] = distance;
    }

    int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
        if (stop_distances_.count({ from, to })) {
            return stop_distances_.at({ from, to });
        } else if (stop_distances_.count({ to, from })) {
            return stop_distances_.at({ to, from });
        } else {
            return 0;
        }
    }

} // namespace trans_cat