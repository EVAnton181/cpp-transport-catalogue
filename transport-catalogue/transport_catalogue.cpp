#include "transport_catalogue.h"

using namespace catalog;

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view>& stops_name, bool flag) {
    std::vector<domain::Stop*> ptr_stops;
    
    for (auto stop_name : stops_name) {
        ptr_stops.push_back(stopname_to_stop_.at(stop_name));
    }

    std::sort(stops_name.begin(), stops_name.end());
    auto last = std::unique(stops_name.begin(), stops_name.end());
    int uni = last - stops_name.begin();

    auto& ref = buses_.emplace_back(std::string(name), ptr_stops, flag, uni);
    std::string_view bus_sw = ref.bus;
    busname_to_bus_[bus_sw] = &ref;
    
    for (auto stop_name : ptr_stops) {
        stopname_to_buses_[stop_name->stop_name].emplace(bus_sw);
    }
}

void TransportCatalogue::AddStop(std::string_view name, double lat, double lng) {
    auto& ref = stops_.emplace_back(std::string(name), lat, lng, stops_.size());
    std::string_view stop_sw = ref.stop_name;
    stopname_to_stop_[stop_sw] = &ref;
    stopname_to_buses_[stop_sw];
    
}

void TransportCatalogue::SetDistance(domain::Stop* departure_stop, domain::Stop* arrival_stop, double distance) {
	auto key_pair = std::make_pair(departure_stop, arrival_stop);
	
	distance_[key_pair] = distance;
}

void TransportCatalogue::AddRoutingSetting(int wait_time, int bus_velocity)  {
    routing_setting_.wait_time = wait_time;
    routing_setting_.bus_velocity = bus_velocity;
}

void TransportCatalogue::InitRouterGraph() {
    router_graph_ = graph::DirectedWeightedGraph<double>(stops_.size());
}

void TransportCatalogue::AddEdgeInRouterGraph() {
    for (auto& bus : buses_) {
				graph::Edge<double> added_edge;
				added_edge.bus = &bus;
        for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
            double sum_distance = 0;
						double sum_back_distance = 0;
            for (size_t j = i + 1; j < bus.stops.size(); ++j) {
								added_edge.from = bus.stops.at(i)->stop_id;
								added_edge.to = bus.stops.at(j)->stop_id;
								
                auto distance = GetDistance(bus.stops.at(j-1), bus.stops.at(j));
								
								if (!distance) {
									distance = GetDistance(bus.stops.at(j), bus.stops.at(j-1));
								}
								
								sum_distance += distance.value();
								
								added_edge.weight = sum_distance / routing_setting_.bus_velocity + routing_setting_.wait_time;
                
								added_edge.stops_count = j - i;
								
								router_graph_.AddEdge(added_edge);
								
                if (!bus.round_trip) {
									added_edge.from = bus.stops.at(j)->stop_id;
									added_edge.to = bus.stops.at(i)->stop_id;
									
									auto distance_back = GetDistance(bus.stops.at(j), bus.stops.at(j-1));
									if (distance_back != nullptr && distance_back != distance) {
										sum_back_distance += distance_back.value();
									}
									else {
										sum_back_distance += distance;
									}
									
									added_edge.weight = sum_distance / routing_setting_.bus_velocity + routing_setting_.wait_time;
									
									router_graph_.AddEdge(added_edge);
                }
                
            }
        }
//         for (auto stop : bus.stops) {
//             
//         }
    }
    
//     size_t stops_count = stops_.size();
//     
//     for (size_t i = 0; i < stops_count; ++i) {
//         std::string_view stop_from = stops_.at(i);
//         auto buses_by_stop = GetBusesByStop(stop_from);
//         if (!buses_by_stop) {
//             continue;
//         }
//         
//         for (size_t j = 0; j < stops_count; ++j) {
//             std::string_view stop_to = stops_.at(j);
//             if (stop_from == stop_to) {
//                 continue;
//             }
//             
//         }
//     }
//     
//     added_edge.frome =
//     added_edge.to = 
//     added_edge.weight = 
//     added_edge.stops_count = 
//     added_edge.bus = 
//     router_graph_.AddEdge()
}

domain::Stop* TransportCatalogue::FindStop(const std::string& name) {
    if (stopname_to_stop_.count(name) != 0) {
        return stopname_to_stop_.at(name);
    }
    else {
        return nullptr;
    }
}

domain::Bus* TransportCatalogue::FindBus(const std::string_view& name) const {
    if (busname_to_bus_.count(name) != 0) {
        return busname_to_bus_.at(name);
    }
    else {
        return nullptr;
    }
}

std::optional<domain::BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const {
    auto find_bus = FindBus(bus_name);
	if (find_bus == nullptr) {
		return {};
	}
	
	domain::BusStat bus_stat;
	bus_stat.stop_count = find_bus->stops.size();
    bus_stat.unique_stop_count = find_bus->uni_stops;
	
	double coordinate_lengh = 0;
        
        /// Считаем длину маршрута по контейнеру расстояний и по координатам
	for (size_t i = 0; i < find_bus->stops.size() - 1; ++i) {
		coordinate_lengh += geo::ComputeDistance(find_bus->stops[i]->geo_point, find_bus->stops[i+1]->geo_point);

		if (distance_.count(std::make_pair(find_bus->stops[i], find_bus->stops[i+1]))) {
                bus_stat.route_length += distance_.at(std::make_pair(find_bus->stops[i], find_bus->stops[i+1]));
            }
            else {
                bus_stat.route_length += distance_.at(std::make_pair(find_bus->stops[i+1], find_bus->stops[i]));
            }
        }
        
        /// Если маршрут не круговой, то по контейнеру расстояний проходим другими парами, чтобы найти отличные расстояния в разных направлениях
        if (!(find_bus->round_trip)) {
            for (size_t i = find_bus->stops.size() - 1; i > 0; --i) {
				
                if (distance_.count(std::make_pair(find_bus->stops[i], find_bus->stops[i-1]))) {
                    bus_stat.route_length += distance_.at(std::make_pair(find_bus->stops[i], find_bus->stops[i-1]));
                }
                else {
                    bus_stat.route_length += distance_.at(std::make_pair(find_bus->stops[i-1], find_bus->stops[i]));
                }
            }
            
            coordinate_lengh *= 2; /// географическую длину увеличиваем в двое
            bus_stat.stop_count = bus_stat.stop_count * 2 - 1; /// в общем количестве остановок учитываем обратные остановки
        }
        
	bus_stat.curvature = bus_stat.route_length/coordinate_lengh;

    return bus_stat;
}

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesByStop(const std::string_view& stop_name) const {
    if (stopname_to_buses_.count(stop_name) == 0) {
		return {};
	}
	
	auto buses_list = &stopname_to_buses_.at(stop_name);
	
	std::vector<std::string_view> sort_buses(std::move_iterator(buses_list->begin()), std::move_iterator(buses_list->end()));
		std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& lhs, const auto& rhs) {
            return lhs < rhs;
			});
		return sort_buses;
}


std::optional<double> TransportCatalogue::GetDistance(domain::Stop* stop1, domain::Stop* stop2) const{
    KeyStops stops_pair = std::make_pair(stop1, stop2);
    if (distance_.count(stops_pair)) {
        return distance_.at(stops_pair);
    } else {
        return {};
    }
}
/*
const std::unordered_set<std::string_view>* TransportCatalogue::GetBusesSet(const std::string_view& stop_name) const {
	if (stopname_to_buses_.count(stop_name) == 0) {
		return nullptr;
	} else {
		return &stopname_to_buses_.at(stop_name);
	}
}*/

std::vector<const domain::Bus*> TransportCatalogue::GetSortBusesToRender() const {
	auto buses = GetBusesToRender();
	
	std::vector<const domain::Bus*> sort_buses(std::move_iterator(buses.begin()), std::move_iterator(buses.end()));
    
	std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->bus < rhs->bus;
        });
	
	return sort_buses;
}

std::unordered_set<const domain::Bus*> TransportCatalogue::GetBusesToRender() const {
    std::unordered_set<const domain::Bus*> buses;
    
    for (auto& bus : buses_) {
        if ( !bus.stops.empty() ) {
            buses.insert(&bus);
        }
    }
    
    return buses;
}

std::vector<geo::Coordinates> TransportCatalogue::GetAllRenderGeoCoordinates() const {
	auto stops = GetStopsToRender();
	
	std::vector<geo::Coordinates> all_geo_coordinates;
	all_geo_coordinates.reserve(stops.size());
	
	for (auto& stop : stops) {
		all_geo_coordinates.push_back(stop->geo_point);
	}
	
	return all_geo_coordinates;	
}

std::vector<const domain::Stop*> TransportCatalogue::GetSortStopsToRender() const {
	auto stops_to_render = GetStopsToRender();
	
	std::vector<const domain::Stop*> sort_stops(std::move_iterator(stops_to_render.begin()), std::move_iterator(stops_to_render.end()));
	
    std::sort(sort_stops.begin(), sort_stops.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->stop_name < rhs->stop_name;
        });
	return sort_stops;
}

std::unordered_set<const domain::Stop*> TransportCatalogue::GetStopsToRender() const {
    std::unordered_set<const domain::Stop*> stops;
    
    for (auto& stop : stops_) {
        if ( !stopname_to_buses_.at(stop.stop_name).empty()) {
            stops.insert(&stop);
        }
    }
    
    return stops;
}

// const std::tuple<int, int> TransportCatalogue::GetRoutingSetting() const {
//     return std::make_tuple(routing_setting_.wait_time, routing_setting_.bus_velocity);
// }
// 
// const size_t TransportCatalogue::GetStopsCount() const {
//     return stops_.size();
// }
