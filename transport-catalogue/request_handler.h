#pragma once

#include <sstream>

#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


class RequestHandler {
private:
    const trans_cat::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;

public:
    explicit RequestHandler(const trans_cat::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer)
        : catalogue_(catalogue), renderer_(renderer) {}

    void ProcessRequests(const json::Node& stat_requests) const;

    const json::Node PrintRoute(const json::Dict& request_map) const;
    const json::Node PrintStop(const json::Dict& request_map) const;
    const json::Node PrintMap(const json::Dict& request_map) const;

    std::optional<trans_cat::BusStat> GetBusStat(const std::string_view bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;

    svg::Document RenderMap() const;
};