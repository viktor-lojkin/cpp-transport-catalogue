#include "request_handler.h"
#include "json_builder.h"

void RequestHandler::ProcessRequests(const json::Node& stat_requests) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") result.push_back(PrintStop(request_map).AsDict());
        if (type == "Bus") result.push_back(PrintRoute(request_map).AsDict());
        if (type == "Map") result.push_back(PrintMap(request_map).AsDict());
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node RequestHandler::PrintRoute(const json::Dict& request_map) const {
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string& bus_name = request_map.at("name").AsString();

    if (!catalogue_.FindBus(bus_name)) {
        result = json::Builder{}.StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        auto bus_stat = GetBusStat(bus_name);

        result = json::Builder{}.StartDict()
            .Key("request_id").Value(id)
            .Key("curvature").Value(bus_stat->curvature)
            .Key("route_length").Value(bus_stat->route_length)
            .Key("stop_count").Value(static_cast<int>(bus_stat->stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(bus_stat->unique_stops_count))
            .EndDict()
            .Build();
    }

    return result;
}

const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string& stop_name = request_map.at("name").AsString();

    if (!catalogue_.FindStop(stop_name)) {
        result = json::Builder{}.StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array buses;
        for (auto& bus : GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result = json::Builder{}.StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }

    return result;
}

const json::Node RequestHandler::PrintMap(const json::Dict& request_map) const {
    json::Node result;
    std::ostringstream strm;
    svg::Document map = RenderMap();

    const int id = request_map.at("id").AsInt();
    map.Render(strm);

    result = json::Builder{}.StartDict()
        .Key("request_id").Value(id)
        .Key("map").Value(strm.str())
        .EndDict()
        .Build();

    return result;
}

std::optional<trans_cat::BusStat> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    trans_cat::BusStat bus_stat{};
    const trans_cat::Bus* bus = catalogue_.FindBus(bus_number);

    if (!bus) {
        throw std::invalid_argument("bus not found");
    }
    if (bus->is_circle) {
        bus_stat.stops_count = bus->stops_of_this_bus.size();
    }
    else {
        bus_stat.stops_count = bus->stops_of_this_bus.size() * 2 - 1;
    }

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops_of_this_bus.size() - 1; ++i) {
        auto from = bus->stops_of_this_bus[i];
        auto to = bus->stops_of_this_bus[i + 1];
        if (bus->is_circle) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = catalogue_.UniqueStopsCount(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name)->buses_on_this_stop;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetAllBuses());
}