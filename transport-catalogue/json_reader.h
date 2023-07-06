#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <iostream>

class JsonReadingError : public std::logic_error {
public:
    using logic_error::logic_error;
};

class JsonReader {
private:
    json::Document input_;
    json::Node node_ = nullptr;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> SetStop(const json::Dict& request_map) const;
    std::tuple<std::string_view, std::vector<const trans_cat::Stop*>, bool> SetBus(const json::Dict& request_map, trans_cat::TransportCatalogue& catalogue) const;
    void SetStopDistances(trans_cat::TransportCatalogue& catalogue) const;

public:
    JsonReader(std::istream& input)
        : input_(json::Load(input)) {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;

    void SetCatalogue(trans_cat::TransportCatalogue& catalogue);
    renderer::MapRenderer SetRenderSettings(const json::Dict& request_map) const;

};