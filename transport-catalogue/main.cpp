#include "json_reader.h"
#include "request_handler.h"

int main() {
    trans_cat::TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.SetCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.SetRenderSettings(render_settings);

    RequestHandler rh(catalogue, renderer);
    rh.ProcessRequests(stat_requests);
}