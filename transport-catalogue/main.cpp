#include "input_reader.h"
#include "stat_reader.h"

using namespace trans_cat;

int main() {
    TransportCatalogue catalogue;
    SetTransportCatalogue(std::cin, catalogue);
    ProcessRequests(std::cin, catalogue, std::cout);
}