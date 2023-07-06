#include "transport_catalogue.h"

#include <algorithm>

namespace trans_cat {

void TransportCatalogue::AddStop(const std::string_view& stop_name, geo::Coordinates& coordinates) {
    all_stops_.push_back({ std::string(stop_name), coordinates, {} });
    stopname_to_stop_[all_stops_.back().stop_name] = &all_stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view& bus_name, const std::vector<const Stop*>& stops, bool circular) {
    all_buses_.push_back({ std::string(bus_name), stops, circular });
    busname_to_bus_[all_buses_.back().bus_name] = &all_buses_.back();

    for (const auto& stop_ptr : stops) {
        std::for_each(
            all_stops_.begin(), all_stops_.end(),
            [&bus_name, &stop_ptr](auto& stop_) {
                if (stop_.stop_name == stop_ptr->stop_name) {
                    stop_.buses_on_this_stop.insert(std::string(bus_name));
                }
            }
        );
    }
}

const Bus* TransportCatalogue::FindBus(const std::string_view& bus_name) const {
    return busname_to_bus_.count(bus_name) ? busname_to_bus_.at(bus_name) : nullptr;
}

const Stop* TransportCatalogue::FindStop(const std::string_view& stop_name) const {
    return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
}

size_t TransportCatalogue::UniqueStopsCount(const std::string_view& bus_name) const {
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : busname_to_bus_.at(bus_name)->stops_of_this_bus) {
        unique_stops.insert(stop->stop_name);
    }
    return unique_stops.size();
}

const std::map<std::string_view, const Bus*> TransportCatalogue::GetAllBuses() const {
    std::map<std::string_view, const Bus*> result;
    for (const auto& bus : busname_to_bus_) {
        result.emplace(bus);
    }
    return result;
}

void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, const int distance) {
    stop_distances_[{from, to}] = distance;
}

int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    if (stop_distances_.count({ from, to })) {
        return stop_distances_.at({ from, to });
    }
    else if (stop_distances_.count({ to, from })) {
        return stop_distances_.at({ to, from });
    }
    else {
        return 0;
    }
}

} // namespace trans_cat