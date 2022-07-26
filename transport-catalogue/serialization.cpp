#include "serialization.h"

void SetFilePath(std::string file_path) {
  file_ = file_path;
}

// Сериализует каталог
void Serialization::InitSerializationCatalog(const catalog::TransportCatalogue& catalog) {
    
    for (auto& stop_name : catalog.GetAllStopName()) {
        catalog_buf::Stop stop_pb;
        stop_pb.set_stop_name(std::string(stop_name));
        catalog_buf::Coordinate coordinate_pb;
        auto coordinate = catalog.GetStopCoordinate(stop_name);
        coordinate_pb.set_lat(coordinate.lat);
        coordinate_pb.set_lng(coordinate.lng);
        *stop_pb.mutable_point() = move(coordinate_pb);
        
        serialization_catalog_.add_stop();
        *serialization_catalog_.mutable_stop(serialization_catalog_.stop_size()-1) = move(stop_pb);
    }
    
    std::map<std::piar<int, int>,  double> distances = catalog.GetDistances();
    for (auto& [key,  distance] : distances) {
		catalog_buf::Distance dist;
		dist.set_stop_id_from(key.first);
		dist.set_stop_id_to(key.second);
		dist.set_distance(distance);
		
		 serialization_catalog_.add_map_distance();
        *serialization_catalog_.mutable_map_distance(serialization_catalog_.map_distance_size()-1) = move(dist);
	}
    
    for (auto& bus_name : catalog.GetAllBusesName()) {
        catalog_buf::Bus bus_pb;
        bus_pb.set_bus_name(std::string(bus_name));
        bus_pb.set_round_trip(catalog.IsRoundTrip(bus_name));
        std::vector<int> bus_stops = catalog.GetStopsNumToBus();
        for (auto& stop_num : bus_stops) {
            bus_pb.add_bus(stop_num);
//             *bus_pb.mutable_stop_num(bus_pb.stop_num_size()-1) = move(stop_num);
        }
        serialization_catalog_.add_bus();
        *serialization_catalog_.mutable_bus(serialization_catalog_.bus_size()-1) = move(bus_pb);
    }
}

// Сохраняет сериализованный каталог в поток output
void Serialization::SaveTo() const {
	std::ofstream out(file_, ios::binary);
    serialization_catalog_.SerializeToOstream(&out);
}

// Возвращает десериализованный каталог
catalog::TransportCatalogue Serialization::DeserializeTransportCatalogue() {
    catalog::TransportCatalogue load_catalog;
    
    std::vector<catalog_buf::Stop> stops = serialization_catalog_.stop();
    
    for (auto& stop : stops) {
// 	  string stop_name = 1;
//    Coordinates poin = 2;
	  load_catalog.AddStop(stop.stop_name(), stop.point().lat(), stop.point().lng());
	}
	
	std::vector<catalog_buf::Distance> map_dist = serialization_catalog_.map_distance();
	
	for (auto& dist : map_dist) {
	  load_catalog.SetDistance(load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_from()))),  load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_to()))), dist.distance());
	}
	
	std::vector<catalog_buf::Bus> buses = serialization_catalog_.bus();
	
	for (auto& bus : buses) {
	  std::vector<std::string_view> stops;
	  if (bus.has_stop_num()) {
		stops = std::move(load_catalog.GetStopsNameFromId(bus.stop_num()));
	  }
	  load_catalog.AddBusWithStopId(bus.bus_name(), stops, bus.round_trip());
	}
}

// Загружает сериализованный каталог из file_
void Serialization::LoadFrom() {
	ifstream ifs(file_, ios::binary);
    serialization_catalog_.ParseFromIstream(&ifs);
}
