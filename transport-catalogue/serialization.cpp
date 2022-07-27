#include "serialization.h"

using namespace serialization;

void Serialization::SetFilePath(std::string file_path) {
  file_ = std::move(file_path);
}

// Сериализует каталог
void Serialization::InitSerializationCatalog(const catalog::TransportCatalogue& catalog) {
    
    for (auto& stop_name : catalog.GetAllStopName()) {
        catalog_buf::Stop stop_pb;
        stop_pb.set_stop_name(std::string(stop_name));
        catalog_buf::Coordinates coordinate_pb;
        auto coordinate = catalog.GetStopCoordinate(stop_name);
        coordinate_pb.set_lat(coordinate.lat);
        coordinate_pb.set_lng(coordinate.lng);
        *stop_pb.mutable_point() = std::move(coordinate_pb);
        
        serialization_catalog_.add_stop();
        *serialization_catalog_.mutable_stop(serialization_catalog_.stop_size()-1) = std::move(stop_pb);
    }
    
    std::vector<std::tuple<int, int, double>> distances = catalog.GetDistances();
    for (auto& distance : distances) {
		catalog_buf::Distance dist;
		dist.set_stop_id_from(std::get<0>(distance));
		dist.set_stop_id_to(std::get<1>(distance));
		dist.set_distance(std::get<2>(distance));
		
		 serialization_catalog_.add_map_distance();
        *serialization_catalog_.mutable_map_distance(serialization_catalog_.map_distance_size()-1) = std::move(dist);
	}
    
    for (auto& bus_name : catalog.GetAllBusesName()) {
        catalog_buf::Bus bus_pb;
        bus_pb.set_bus_name(std::string(bus_name));
        bus_pb.set_round_trip(catalog.IsRoundTrip(bus_name));
        const std::vector<int> bus_stops = catalog.GetStopsNumToBus(bus_name);
        for (auto& stop_num : bus_stops) {
            bus_pb.add_stop_num(stop_num);
//             *bus_pb.mutable_stop_num(bus_pb.stop_num_size()-1) = move(stop_num);
        }
        serialization_catalog_.add_bus();
        *serialization_catalog_.mutable_bus(serialization_catalog_.bus_size()-1) = std::move(bus_pb);
    }
}

// Сохраняет сериализованный каталог в поток output
void Serialization::SaveTo() const {
	std::ofstream out_file(file_, std::ios::binary);
    serialization_catalog_.SerializeToOstream(&out_file);
}

// Возвращает десериализованный каталог
catalog::TransportCatalogue Serialization::DeserializeTransportCatalogue() {
    catalog::TransportCatalogue load_catalog;
    std::cout <<  "deser" << std::endl;
//     std::vector<catalog_buf::Stop> stops = serialization_catalog_.stop();
    int size = serialization_catalog_.stop_size();
    
        std::cout << "size = " << size << std::endl;
    
    for (int i = 0; i < size; ++i) {
        auto stop = serialization_catalog_.stop(i);
        std::cout << stop.stop_name() << std::endl;
	  load_catalog.AddStop(stop.stop_name(), stop.point().lat(), stop.point().lng());
	}
	
	size = serialization_catalog_.map_distance_size();
	std::cout << "size = " << size << std::endl;
    for (int i = 0; i < size; ++i) {
    auto dist = serialization_catalog_.map_distance(i);
	 std::cout << dist.stop_id_from() << std::endl; load_catalog.SetDistance(load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_from()))),  load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_to()))), dist.distance());
	}
	
// 	std::vector<catalog_buf::Bus> buses = serialization_catalog_.bus();
    size = serialization_catalog_.bus_size();
	for (auto& bus : serialization_catalog_.bus()) {
     
        std::vector<std::string_view> stops;

        for (auto& stop_id : bus.stop_num()) {
            stops.push_back(load_catalog.GetStopNameFromId(stop_id));
        }
//         stops = std::move(load_catalog.GetStopsNameFromId(bus.stop_num()));

        load_catalog.AddBus(bus.bus_name(), stops, bus.round_trip());
	}
	
	std::cout << "size = " << size << std::endl;
	return load_catalog;
}

// Загружает сериализованный каталог из file_
void Serialization::LoadFrom() {
    std::cout << file_ << std::endl;
	std::ifstream ifs(file_, std::ios::binary);
    if (!serialization_catalog_.ParseFromIstream(&ifs)) {
        std::cout << "Fatal ERROR" << std::endl;
    }
}
