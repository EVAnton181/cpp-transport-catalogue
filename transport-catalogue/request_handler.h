/*!
 * @file request_handler.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с интерфейсом для работы с TransportCatalogue
 * и MapRanderer
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

// #include "log_duration.h"

class RequestHandler {
public:
    RequestHandler(const catalog::TransportCatalogue& catalog, map_renderer::MapRanderer& renderer) 
		: db_(catalog)
		, renderer_(renderer)
	{
	}
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
};
