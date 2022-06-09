#include "request_handler.h"

const std::optional<domain::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const{
	return db_.GetBusStat(bus_name);
}

/// Возвращает маршруты, проходящие через осановку
const std::optional<std::vector<std::string_view>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	return db_.GetBusesByStop(stop_name);
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
