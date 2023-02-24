/*!
 * @file transport_router.h
 * @brief Заголовочный файл с описанием структуры  информации о марщруте, 
 * и определением класса маршрутов.
 * 
 * @author Elistratov Anton
 * 
 * @version 1.0
 * @date Май 2022
 */
#pragma once

#include <optional>
#include <tuple>
#include <vector>

#include "router.h"
#include "transport_catalogue.h"
#include "domain.h"

#include "graph.h"

namespace transport_router {

    struct RouteInfo {
        size_t wait_stop;
        domain::Bus* bus;
        int span_count;
        double time;
    };
    
    class TransportRouter {
    private:
        using Graph = graph::DirectedWeightedGraph<double>;
        
    public:
        TransportRouter(const Graph& graph);
        
        std::optional<std::tuple<double, std::vector<RouteInfo>>> GetRouter(graph::VertexId from, graph::VertexId to) const;
        
    private:
        graph::Router<double> router_;
    };

}
