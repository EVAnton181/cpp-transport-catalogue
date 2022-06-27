/*!
 * @file request_handler.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с интерфейсом для работы с TransportCatalogue, 
 * TransportRoutet и MapRanderer
 * 
 * 
*/
#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <string_view>
#include <set>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "domain.h"
#include "transport_router.h"

// #include "log_duration.h"

class RequestHandler {
public:
    RequestHandler(const catalog::TransportCatalogue& catalog, map_renderer::MapRanderer& renderer, const graph::DirectedWeightedGraph<double>& graph) 
		: db_(catalog)
		, renderer_(renderer)
		, transport_router_(graph)
	{
	}
	
	// Возвращаем информацию о пути
	const std::optional<std::tuple<double, std::vector<domain::RouteInfo>>> GetRouter(const std::string_view& stop_from, const std::string_view& stop_to) const;
    
    // Возвращает информацию о маршруте (запрос Bus)
    const std::optional<domain::BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::optional<std::vector<std::string_view>> GetBusesByStop(const std::string_view& stop_name) const;
        
    void MakeRenderMap();

	void RenderMap(std::ostream& out = std::cout) const;
private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const catalog::TransportCatalogue& db_;
    map_renderer::MapRanderer& renderer_;
    transport_router::TransportRouter transport_router_;
};
