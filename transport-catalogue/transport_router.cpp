#include "transport_router.h"

using namespace transport_router;

TransportRouter::TransportRouter(const TransportRouter::Graph& graph)
    : router_(graph)
{
}

std::optional<std::tuple<double, std::vector<RouteInfo>>> TransportRouter::GetRouter(graph::VertexId from, graph::VertexId to) const {
    auto router = router_.BuildRoute(from, to);
    
    if (!router) {
        return {};
    }
    double total_time = router.value().weight;
    std::vector<RouteInfo> items;
    
    for (auto edge : router.value().edges) {
        const graph::Edge route_part = router_.GetGraph().GetEdge(edge);
        RouteInfo item;
        
        item.wait_stop = route_part.from;
        item.bus = route_part.bus;
        item.span_count = route_part.stops_count;
        item.time = route_part.weight;
        
        items.push_back(item);
    }
    
    return std::make_tuple(total_time, items);
}
