/*!
 * @file request_handler.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с интерфейсом для работы с TransportCatalogue, 
 * TransportRoutet и MapRanderer
 * Serialization
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
#include "serialization.h"

// #include "log_duration.h"

class RequestHandler {
public:
    RequestHandler(catalog::TransportCatalogue& catalog, map_renderer::MapRanderer& renderer, graph::DirectedWeightedGraph<double>& graph, 
	serialization::Serialization serialization) 
		: db_(catalog)
		, renderer_(renderer)
		, transport_router_(graph)
		, serialization_(serialization)		
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
    
    void InitSerializationCatalog();
    
    void SaveSerializationCatalog();
    
    void DeserializeCatalogue();
    
    void DeserializeRenderMap();
private:
    catalog::TransportCatalogue& db_;
    map_renderer::MapRanderer& renderer_;
    transport_router::TransportRouter transport_router_;
    serialization::Serialization serialization_;
    
    void DeserializeStop();
    
    void DeserializeMapDistance();
    
    void DeserializeBus();
};

