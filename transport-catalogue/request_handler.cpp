#include "request_handler.h"

const std::optional<domain::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const{
	return db_.GetBusStat(bus_name);
//     auto find_bus = db_.FindBus(bus_name);
//     if(find_bus == nullptr) {
//         return {};
//     }
//     
// 	domain::BusStat bus_stat;
//     
//     bus_stat.stop_count = find_bus->stops.size();
//     bus_stat.unique_stop_count = find_bus->uni_stops;
//     
//     double coordinate_lengh = 0;
//     
//     /// Считаем длину маршрута по контейнеру расстояний и по координатам
//     for (size_t i = 0; i < find_bus->stops.size() - 1; ++i) {
//         coordinate_lengh += geo::ComputeDistance(find_bus->stops[i]->geo_point, find_bus->stops[i+1]->geo_point);
// 
// 		auto distance = db_.GetDistance(find_bus->stops[i], find_bus->stops[i+1]);
//         
// 		if (distance.has_value()) {
//             bus_stat.route_length += distance.value();
//         } else {
//             bus_stat.route_length += db_.GetDistance(find_bus->stops[i+1], find_bus->stops[i]).value();
//         }
//     }
//     
//     /// Если маршрут не круговой, то по контейнеру расстояний проходим другими парами, чтобы найти отличные расстояния в разных направлениях
//     if (!(find_bus->round_trip)) {
//         for (size_t i = find_bus->stops.size() - 1; i > 0; --i) {
//           
//             auto distance = db_.GetDistance(find_bus->stops[i], find_bus->stops[i-1]);
//             
//             if (distance.has_value()) {
//                 bus_stat.route_length += distance.value();
//             } else {
//                 bus_stat.route_length += db_.GetDistance(find_bus->stops[i-1], find_bus->stops[i]).value();
//             }
//         }
//         
//         coordinate_lengh *= 2; /// географическую длину увеличиваем в двое
//         bus_stat.stop_count = bus_stat.stop_count * 2 - 1; /// в общем количестве остановок учитываем обратные остановки
//     }
//     
//     bus_stat.curvature = bus_stat.route_length/coordinate_lengh;
//     
//     return bus_stat;
}

/// Возвращает маршруты, проходящие через осановку
const std::optional<std::vector<std::string_view>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	return db_.GetBusesByStop(stop_name);
// 	if (auto buses_list = db_.GetBusesSet(stop_name)) {
// 		std::vector<std::string_view> sort_buses(std::move_iterator(buses_list->begin()), std::move_iterator(buses_list->end()));
// 		std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& lhs, const auto& rhs) {
//             return lhs < rhs;
// 			});
// 		return sort_buses;
// 	} else {
// 		return {};
// 	}
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
