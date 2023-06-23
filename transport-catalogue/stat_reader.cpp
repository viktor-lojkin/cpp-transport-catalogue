#include "stat_reader.h"

#include <iomanip>

namespace trans_cat {

    void ProcessRequests(std::istream& in, TransportCatalogue& catalogue, std::ostream& out) {
        size_t requests_count;
        in >> requests_count;
        for (size_t i = 0; i < requests_count; ++i) {
            std::string keyword, line;
            in >> keyword;
            std::getline(in, line);
            if (keyword == "Bus") {
                PrintBus(line, catalogue, out);
            }
            if (keyword == "Stop") {
                PrintStop(line, catalogue, out);
            }
        }
    }

    void PrintBus(std::string& line, TransportCatalogue& catalogue, std::ostream& out) {
        std::string bus_name = line.substr(1, line.npos);
        if (catalogue.FindBus(bus_name)) {
            BusStat bus_stat = catalogue.BusInformation(bus_name);
            out << "Bus " << bus_name << ": "
                << bus_stat.stops_count << " stops on route, "
                << bus_stat.unique_stops_count << " unique stops, "
                << bus_stat.route_length << " route length, "
                << std::setprecision(6)
                << bus_stat.curvature << " curvature"
                << std::endl;
        } else {
            out << "Bus " << bus_name << ": not found" << std::endl;
        }
    }

    void PrintStop(std::string& line, TransportCatalogue& catalogue, std::ostream& out) {
        std::string stop_name = line.substr(1, line.npos);
        if (catalogue.FindStop(stop_name)) {
            out << "Stop " << stop_name << ": ";
            auto buses = catalogue.GetBusesOnStop(stop_name);
            if (!buses.empty()) {
                out << "buses ";
                for (const auto& bus : buses) {
                    out << bus << " ";
                }
                out << std::endl;
            } else {
                out << "no buses" << std::endl;
            }
        } else {
            out << "Stop " << stop_name << ": not found" << std::endl;
        }
    }

} // namespace trans_cat