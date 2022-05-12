#include "transport_catalogue.h"

using namespace catalog;

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view>& stops_name, bool flag) {
    std::vector<Stop*> ptr_stops;
    
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
    auto& ref = stops_.emplace_back(std::string(name), lat, lng);
    std::string_view stop_sw = ref.stop_name;
    stopname_to_stop_[stop_sw] = &ref;
    
}

void TransportCatalogue::SetDistance(Stop* departure_stop, Stop* arrival_stop, double distance) {
	auto key_pair = std::make_pair(departure_stop, arrival_stop);
	
	distance_[key_pair] = distance;
}


TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name) {
    if (stopname_to_stop_.count(name) != 0) {
        return stopname_to_stop_.at(name);
    }
    else {
        return nullptr;
    }
}

TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name) {
    if (busname_to_bus_.count(name) != 0) {
        return busname_to_bus_.at(name);
    }
    else {
        return nullptr;
    }
}

TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(std::string name) {
    auto find_bus = FindBus(name);
	
	BusInfo bus_info;
	bus_info.stops_on_route = 0;
    bus_info.unique_stops = 0;
    bus_info.route_length = 0;
    bus_info.curvature = 0;
        
    if (find_bus != nullptr) {
        bus_info.stops_on_route = find_bus->stops.size();
        bus_info.unique_stops = find_bus->uni_stops;
        
        double coordinate_lengh = 0;
        
        /// Считаем длину маршрута по контейнеру расстояний и по координатам
        for (int i = 0; i < find_bus->stops.size() - 1; ++i) {
            coordinate_lengh += ComputeDistance(find_bus->stops[i]->geo_point, find_bus->stops[i+1]->geo_point);

            if (distance_.count(std::make_pair(find_bus->stops[i], find_bus->stops[i+1]))) {
                bus_info.route_length += distance_.at(std::make_pair(find_bus->stops[i], find_bus->stops[i+1]));
            }
            else {
                bus_info.route_length += distance_.at(std::make_pair(find_bus->stops[i+1], find_bus->stops[i]));
            }
        }
        
        /// Если маршрут не круговой, то по контейнеру расстояний проходим другими парами, чтобы найти отличные расстояния в разных направлениях
        if (!(find_bus->round_trip)) {
            for (int i = find_bus->stops.size() - 1; i > 0; --i) {
                if (distance_.count(std::make_pair(find_bus->stops[i], find_bus->stops[i-1]))) {
                    bus_info.route_length += distance_.at(std::make_pair(find_bus->stops[i], find_bus->stops[i-1]));
                }
                else {
                    bus_info.route_length += distance_.at(std::make_pair(find_bus->stops[i-1], find_bus->stops[i]));
                }
            }
            
            coordinate_lengh *= 2; /// географическую длину увеличиваем в двое
            bus_info.stops_on_route = bus_info.stops_on_route * 2 - 1; /// в общем количестве остановок учитываем обратные остановки
        }
        
        bus_info.curvature = bus_info.route_length/coordinate_lengh;
    }

    
    return bus_info;
}

std::vector<std::string_view> TransportCatalogue::GetBusList(std::string name) {
    if (stopname_to_buses_.count(name) == 0) {
		return {};
	}
	
	auto buses = stopname_to_buses_.at(name);
	
	
	
    std::vector<std::string_view> sort_buses(std::move_iterator(buses.begin()), std::move_iterator(buses.end()));
    std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& l, const auto& r) {
            return l < r;
        });

    return sort_buses;
}
