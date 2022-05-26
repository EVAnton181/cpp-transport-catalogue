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
    auto& ref = stops_.emplace_back(std::string(name), lat, lng);
    std::string_view stop_sw = ref.stop_name;
    stopname_to_stop_[stop_sw] = &ref;
    stopname_to_buses_[stop_sw];
    
}

void TransportCatalogue::SetDistance(domain::Stop* departure_stop, domain::Stop* arrival_stop, double distance) {
	auto key_pair = std::make_pair(departure_stop, arrival_stop);
	
	distance_[key_pair] = distance;
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

domain::BusStat TransportCatalogue::GetBusInfo(std::string name) {
    auto find_bus = FindBus(name);
	
	domain::BusStat bus_info;
	bus_info.stop_count = 0;
    bus_info.unique_stop_count = 0;
    bus_info.route_length = 0;
    bus_info.curvature = 0;
        
    if (find_bus != nullptr) {
        bus_info.stop_count = find_bus->stops.size();
        bus_info.unique_stop_count = find_bus->uni_stops;
        
        double coordinate_lengh = 0;
        
        /// Считаем длину маршрута по контейнеру расстояний и по координатам
        for (size_t i = 0; i < find_bus->stops.size() - 1; ++i) {
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
            bus_info.stop_count = bus_info.stop_count * 2 - 1; /// в общем количестве остановок учитываем обратные остановки
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


std::optional<double> TransportCatalogue::GetDistance(domain::Stop* stop1, domain::Stop* stop2) const{
    KeyStops stops_pair = std::make_pair(stop1, stop2);
    if (distance_.count(stops_pair)) {
        return distance_.at(stops_pair);
    } else {
        return {};
    }
}

const std::unordered_set<std::string_view>* TransportCatalogue::GetBusesSet(const std::string_view& stop_name) const {
	if (stopname_to_buses_.count(stop_name) == 0) {
		return nullptr;
	} else {
		return &stopname_to_buses_.at(stop_name);
	}
}

std::unordered_set<const domain::Bus*> TransportCatalogue::GetBusToRender() const {
    std::unordered_set<const domain::Bus*> buses;
    
    for (auto& bus : buses_) {
        if ( !bus.stops.empty() ) {
            buses.insert(&bus);
        }
    }
    
    return buses;
}

std::unordered_set<const domain::Stop*> TransportCatalogue::GetStopToRender() const {
    std::unordered_set<const domain::Stop*> stops;
    
    for (auto& stop : stops_) {
        if ( !stopname_to_buses_.at(stop.stop_name).empty()) {
            stops.insert(&stop);
        }
    }
    
    return stops;
}
