#include "transport_catalogue.h"

using namespace catalog;

TransportCatalogue::TransportCatalogue(TransportCatalogue& other) 
  : stops_(other.stops_)
  , stopname_to_stop_(other.stopname_to_stop_)
  , stopname_to_buses_(other.stopname_to_buses_)
  , buses_(other.buses_)
  , busname_to_bus_(other.busname_to_bus_)
  , distance_(other.distance_)
  , routing_setting_(other.routing_setting_)
  , router_graph_(other.router_graph_)
{
}

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
    const double to_m = 1000;
    const double to_min = 60;
    double convert_bus_velocity = routing_setting_.bus_velocity * to_m /to_min;
    graph::Edge<double> added_edge;
    for (auto& bus : buses_) {
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
								
                added_edge.weight = sum_distance /  convert_bus_velocity + routing_setting_.wait_time;
                
                added_edge.stops_count = j - i;
								
                router_graph_.AddEdge(added_edge);
								
                if (!bus.round_trip) {
                    added_edge.from = bus.stops.at(j)->stop_id;
                    added_edge.to = bus.stops.at(i)->stop_id;
									
                    auto distance_back = GetDistance(bus.stops.at(j), bus.stops.at(j-1));
                    if (distance_back && distance_back != distance) {
                        sum_back_distance += distance_back.value();
                    }
                    else {
                        sum_back_distance += distance.value();
                    }
									
                    added_edge.weight = sum_back_distance /   convert_bus_velocity + routing_setting_.wait_time;
									
                    router_graph_.AddEdge(added_edge);
                }
                
            }
        }
    }
}

void TransportCatalogue::InitDeserializeRouterGraph(std::vector<graph::Edge<double>> edges, std::vector<std::vector<size_t>> incidence_lists) {
  router_graph_ = graph::DirectedWeightedGraph<double>(incidence_lists , edges);
//   router_graph_.InitEdges(edges); 
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

graph::DirectedWeightedGraph<double>& TransportCatalogue::GetGraph() {
    return router_graph_;
}

// size_t TransportCatalogue::GetStopId(std::string_view stop_name) const {
//     return stopname_to_stop_.at(stop_name)->stop_id;
// }


double TransportCatalogue::GetWaitTime() const {
    return routing_setting_.wait_time;
}

const std::vector<std::string_view> TransportCatalogue::GetAllStopName() const {
    std::vector<std::string_view> stop_names;
    stop_names.reserve(stops_.size());
    for (auto& stop : stops_) {
        stop_names.push_back(stop.stop_name);
    }
    return stop_names;
}

const geo::Coordinates TransportCatalogue::GetStopCoordinate(std::string_view stop_name) const {
    return stopname_to_stop_.at(stop_name)->geo_point;
}

const std::vector<std::string_view> TransportCatalogue::GetAllBusesName() const {
    std::vector<std::string_view> bus_names;
    
    bus_names.reserve(buses_.size());
    
    for (auto& bus : buses_) {
        bus_names.push_back(bus.bus);
    }
    
    return bus_names;
}

const bool TransportCatalogue::IsRoundTrip(std::string_view name) const {
    return busname_to_bus_.at(name)->round_trip;
}

const std::vector<int> TransportCatalogue::GetStopsNumToBus(std::string_view name) const {
    std::vector<int> stops_num;
    
    stops_num.reserve(busname_to_bus_.at(name)->stops.size());
    
    for (auto& stop : busname_to_bus_.at(name)->stops) {
        stops_num.push_back(static_cast<int>(stop->stop_id));
    }
    
    return stops_num;
}

std::vector<std::string_view> TransportCatalogue::GetStopsNameFromId(std::vector<int> ids) const {
  std::vector<std::string_view> stops_name;
  stops_name.reserve(stops_.size());
  
  for (auto id : ids) {
	stops_name.push_back(GetStopNameFromId(static_cast<size_t>(id)));
  }
  
  return stops_name;
}

std::vector<std::tuple<int, int, double>> TransportCatalogue::GetDistances() const {
	std::vector<std::tuple<int, int, double>> map_distances;
	
	for (auto stop_from : stops_) {
	  for (auto stop_to : stops_) {
		auto distance = GetDistance(stopname_to_stop_.at(stop_from.stop_name), stopname_to_stop_.at(stop_to.stop_name));
		if (distance.has_value()) {
            map_distances.push_back(std::make_tuple(static_cast<int>(stop_from.stop_id), static_cast<int>(stop_to.stop_id), distance.value()));
		}
	  }
	}
	
	return map_distances;
}

domain::RoutingSetting TransportCatalogue::GetRoutingSetting() const{
    return routing_setting_;
}

std::string_view TransportCatalogue::GetStopNameFromId(size_t id) const {
    return stops_.at(id).stop_name;
}
