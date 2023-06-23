#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>

namespace trans_cat {

    void SetTransportCatalogue(std::istream& in, TransportCatalogue& catalogue);

    std::pair<std::string, geo::Coordinates> SetStop(std::string& line);
    void SetStopDistances(std::string& line, TransportCatalogue& catalogue);
    std::tuple<std::string, std::vector<const Stop*>, bool> SetBus(std::string& line, TransportCatalogue& catalogue);

} // namespace trans_cat