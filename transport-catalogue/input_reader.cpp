#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

namespace trans_cat {

    void SetTransportCatalogue(std::istream& in, TransportCatalogue& catalogue) {
        std::vector<std::string> buses_to_set;
        std::vector<std::string> stops_to_set;
        std::vector<std::string> stop_distances_to_set;

        size_t requests_count;
        in >> requests_count;

        for (size_t i = 0; i < requests_count; ++i) {
            std::string keyword, line;
            in >> keyword;
            std::getline(in, line);
            if (keyword == "Stop") {
                stops_to_set.push_back(line);
            } else if (keyword == "Bus") {
                buses_to_set.push_back(line);
            }
        }

        stop_distances_to_set = stops_to_set;

        for (auto& stop : stops_to_set) {
            auto [stop_name, coordinates] = SetStop(stop);
            catalogue.AddStop(stop_name, coordinates);
        }

        for (auto& stop : stop_distances_to_set) {
            SetStopDistances(stop, catalogue);
        }

        for (auto& bus : buses_to_set) {
            auto [bus_number, stops, is_circle] = SetBus(bus, catalogue);
            catalogue.AddBus(bus_number, stops, is_circle);
            bus = {};
        }
    }

    std::pair<std::string, geo::Coordinates> SetStop(std::string& line) {
        std::string stop_name = line.substr(1, line.find_first_of(':') - line.find_first_of(' ') - 1);
        double lat = std::stod(line.substr(line.find_first_of(':') + 2, line.find_first_of(',') - 1));
        double lng;

        line.erase(0, line.find_first_of(',') + 2);

        if (line.find_last_of(',') == line.npos) {
            lng = std::stod(line.substr(0, line.npos - 1));
            line.clear();
        } else {
            lng = std::stod(line.substr(0, line.find_first_of(',')));
            line.erase(0, line.find_first_of(',') + 2);
        }

        geo::Coordinates stop_coordinates = { lat, lng };

        return { stop_name, stop_coordinates };
    }

    void SetStopDistances(std::string& line, TransportCatalogue& catalogue) {
        if (!line.empty()) {
            std::string stop_name_from = SetStop(line).first;
            const Stop* from = catalogue.FindStop(stop_name_from);

            while (!line.empty()) {
                int distanse = 0;
                std::string stop_to_name;
                distanse = std::stoi(line.substr(0, line.find_first_of("m to ")));
                line.erase(0, line.find_first_of("m to ") + 5);
                if (line.find("m to ") == line.npos) {
                    stop_to_name = line.substr(0, line.npos - 1);
                    const Stop* to = catalogue.FindStop(stop_to_name);
                    catalogue.SetDistance(from, to, distanse);
                    if (!catalogue.GetDistance(from, to)) {
                        catalogue.SetDistance(to, from, distanse);
                    }
                    line.clear();
                } else {
                    stop_to_name = line.substr(0, line.find_first_of(','));
                    const Stop* to = catalogue.FindStop(stop_to_name);
                    catalogue.SetDistance(from, to, distanse);
                    if (!catalogue.GetDistance(from, to)) {
                        catalogue.SetDistance(to, from, distanse);
                    }
                    line.erase(0, line.find_first_of(',') + 2);
                }
            }
        }
    }

    std::tuple<std::string, std::vector<const Stop*>, bool> SetBus(std::string& line, TransportCatalogue& catalogue) {
        std::vector<const Stop*> route_stops;
        std::string route_number = line.substr(1, line.find_first_of(':') - 1);
        bool is_circle = false;
        std::string stop_name;

        line.erase(0, line.find_first_of(':') + 2);
        auto pos = line.find('>') != line.npos ? '>' : '-';

        while (line.find(pos) != line.npos) {
            stop_name = line.substr(0, line.find_first_of(pos) - 1);
            route_stops.push_back(catalogue.FindStop(stop_name));
            line.erase(0, line.find_first_of(pos) + 2);
        }
        stop_name = line.substr(0, line.npos - 1);
        route_stops.push_back(catalogue.FindStop(stop_name));

        if (pos == '>') {
            is_circle = true;
        }

        return { route_number, route_stops, is_circle };
    }

} // namespace trans_cat