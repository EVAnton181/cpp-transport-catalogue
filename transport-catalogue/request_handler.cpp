#include "request_handler.h"

/// Возвращает информацию о маршруте
const std::optional<domain::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const{
	return db_.GetBusStat(bus_name);
}

/// Возвращает маршруты, проходящие через осановку
const std::optional<std::vector<std::string_view>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	return db_.GetBusesByStop(stop_name);
}

/// Возвращаем информацию о пути
const std::optional<std::tuple<double, std::vector<domain::RouteInfo>>> RequestHandler::GetRouter(const std::string_view& stop_from, const std::string_view& stop_to) const {
    const auto from = db_.GetStopId(stop_from);
    const auto to = db_.GetStopId(stop_to);
    auto router = transport_router_.GetRouter(from, to);
    
    if (!router) {
        return {};
    }
    
    auto vector_info = std::get<1>(router.value());
    
    std::vector<domain::RouteInfo> anser;
    
    int wait_time = db_.GetWaitTime();
    
    for (auto& info : vector_info) {
        domain::RouteInfo added_anser;
        added_anser.wait_stop = db_.GetStopNameFromId(info.wait_stop);
        added_anser.wait_time = wait_time;
        added_anser.bus_name = info.bus->bus;
        added_anser.span_count = info.span_count;
        added_anser.time  = info.time - wait_time;
        
        anser.push_back(added_anser);
    }
    
    
    return  std::make_tuple(std::get<0>(router.value()), anser);
}
    
void RequestHandler::MakeRenderMap() {
	
	std::vector<geo::Coordinates> all_geo_coordinates = db_.GetAllRenderGeoCoordinates();
	
	renderer_.SetConverter(all_geo_coordinates);
	
	std::vector<const domain::Bus*> sort_buses = db_.GetSortBusesToRender();
	
	int i = 0;
	for (auto& bus : sort_buses) {
		std::vector<geo::Coordinates> stops_geo_coordinates;
		for( auto& stop : bus->stops) {
			stops_geo_coordinates.push_back(stop->geo_point);
        }
		renderer_.AddBusOnMap(stops_geo_coordinates, bus->round_trip, i);
		++i;
    }
    
    i = 0;
    for (auto& bus : sort_buses) {
		renderer_.AddBusNameOnMap(bus->bus, bus->stops.front()->geo_point, i);
		if (!(bus->round_trip) && ( (bus->stops.front() != bus->stops.back()))) {
			renderer_.AddBusNameOnMap(bus->bus, bus->stops.back()->geo_point, i);
		}
		++i;
	}
	
	std::vector<const domain::Stop*> sort_stops = db_.GetSortStopsToRender();
	    
    for (auto& stop : sort_stops) {
			renderer_.AddStopOnMap(stop->geo_point);
	}
	
	for (auto& stop : sort_stops) {
			renderer_.AddStopNameOnMap(stop->stop_name, stop->geo_point);
	}
}

void RequestHandler::RenderMap(std::ostream& out) const{
	renderer_.PrintRander(out);
}

void RequestHandler::SaveSerializationCatalog() {
	serialization_.InitSerializationCatalog(db_);
	serialization_.SaveTo();
}
