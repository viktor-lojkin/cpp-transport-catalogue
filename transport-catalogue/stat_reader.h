#pragma once

#include "transport_catalogue.h"

namespace trans_cat {

    void ProcessRequests(std::istream& in, TransportCatalogue& catalogue, std::ostream& out);

    void PrintBus(std::string& line, TransportCatalogue& catalogue, std::ostream& out);
    void PrintStop(std::string& line, TransportCatalogue& catalogue, std::ostream& out);

} // namespace trans_cat