#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace trans_cat {

class TransRouter {
public:
	TransRouter() = default;

	TransRouter(const int bus_wait_time, const double bus_velocity)
		: bus_wait_time_(bus_wait_time)
		, bus_velocity_(bus_velocity) {}

	TransRouter(const TransRouter& settings, const TransCatalogue& catalogue) {
		bus_wait_time_ = settings.bus_wait_time_;
		bus_velocity_ = settings.bus_velocity_;
		BuildGraph(catalogue);
	}

	const graph::DirectedWeightedGraph<double>& BuildGraph(const TransCatalogue& catalogue);
	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
	const graph::DirectedWeightedGraph<double>& GetGraph() const;

private:
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;

	graph::DirectedWeightedGraph<double> graph_;
	std::map<std::string, graph::VertexId> stop_ids_;
	std::unique_ptr<graph::Router<double>> router_;
};

} // namespace trans_cat