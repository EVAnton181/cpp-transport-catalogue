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

void RequestHandler::InitSerializationCatalog() {
    // сериализуем остановки
    for(auto& stop_name : db_.GetAllStopName()) {
        auto coordinate = db_.GetStopCoordinate(stop_name);
        serialization_.InitSerializationStop(std::string(stop_name),  coordinate.lat,  coordinate.lng);
    }
    
    // сериализуем расстояния между остановками
    std::vector<std::tuple<int, int, double>> distances = db_.GetDistances();
    for (auto& distance : distances) {
        serialization_.InitSerializationDistance(std::get<0>(distance),  std::get<1>(distance), std::get<2>(distance));
    }
    
    //  сериализуем маршруты
    for (auto& bus_name : db_.GetAllBusesName()) {
        const std::vector<int> bus_stops = db_.GetStopsNumToBus(bus_name);
        serialization_.InitSerializationBus(std::string(bus_name), db_.IsRoundTrip(bus_name), bus_stops);
    }
    
    // сериализация настройки пути
    auto router_settings = db_.GetRoutingSetting();
    serialization_.InitRoutingSettings(router_settings.wait_time, router_settings.bus_velocity);

    // сериализация настроек 
    {
        auto renderer_settings = renderer_.GetSettings();
        serialization_.InitRenderSettiingsParam(renderer_settings.width,
                    renderer_settings.height, 
                    renderer_settings.padding, 
                    renderer_settings.line_width,
                    renderer_settings.stop_radius,
                    renderer_settings.bus_label_font_size,
                    renderer_settings.stop_label_font_size,
                    renderer_settings.underlayer_width);
                    
        double bus_x = renderer_settings.bus_label_offset.x;
        double bus_y = renderer_settings.bus_label_offset.y;
        double stop_x = renderer_settings.stop_label_offset.x;
        double stop_y = renderer_settings.stop_label_offset.y;
        serialization_.InitRenderPoint(bus_x, bus_y, stop_x, stop_y);
        
        serialization_.InitRenderColor(renderer_settings.underlayer_color, renderer_settings.color_palette);
    }
    
    
}

void RequestHandler::SaveSerializationCatalog() {
	serialization_.SaveTo();
}

void RequestHandler::DeserializeCatalogue() {
    DeserializeStop();

    DeserializeMapDistance();

    DeserializeBus();
    
    DeserializeRenderMap();
    
}

void RequestHandler::DeserializeStop() {
    int stop_count = serialization_.GetStopCount();
    for (int i = 0; i < stop_count; ++i) {
        std::tuple<std::string, double, double> stop_data = serialization_.GetStopData(i); 
        db_.AddStop(std::get<0>(stop_data), std::get<1>(stop_data), std::get<2>(stop_data));
    }
}

void RequestHandler::DeserializeMapDistance() {
    int map_size = serialization_.GetMapSize();
    for (int i = 0; i < map_size; ++i) {
        std::tuple<int, int, double> map_data = serialization_.GetMapData(i); 
        auto stop_from = db_.FindStop(std::string(db_.GetStopNameFromId(std::get<0>(map_data))));
        auto stop_to = db_.FindStop(std::string(db_.GetStopNameFromId(std::get<1>(map_data))));
        db_.SetDistance(stop_from, stop_to,  std::get<2>(map_data));
    }
}

void RequestHandler::DeserializeBus() {
  int bus_count = serialization_.GetBusCount();
  for (int i = 0; i < bus_count; ++i) {
	std::string bus_name = serialization_.GetBusName(i);
	bool round_trip = serialization_.GetRoundTripFlag(i);
	std::vector<int> stops_id = serialization_.GetStopsId(i);
	std::vector<std::string_view> stops;
	for (auto id : stops_id) {
	  stops.push_back(db_.GetStopNameFromId(id));
	}
	db_.AddBus(bus_name, stops, round_trip);
  }
}

void RequestHandler::DeserializeRenderMap() {
  map_renderer::RenderSettings settings;
  settings.width = serialization_.GetRenderWidth();
  settings.height = serialization_.GetRenderHeight();
  settings.padding = serialization_.GetRenderPadding();
  settings.line_width = serialization_.GetRenderLineWidth();
  settings.stop_radius = serialization_.GetRenderStopRadius();
  settings.bus_label_font_size = serialization_.GetRenderBusLab();
  settings.stop_label_font_size = serialization_.GetRenderStopLab();
  settings.underlayer_width = serialization_.GetRenderUnderlayerWidth();
  
  auto point = serialization_.GetOffset("bus");
  svg::Point bus(point.first,  point.second);
  settings.bus_label_offset = bus;

  point = serialization_.GetOffset("stop");
  svg::Point stop(point.first,  point.second);
  settings.stop_label_offset = stop;
  
  settings.underlayer_color = serialization_.GetColorUnder();
  
  int palette_size = serialization_.GetPaletteSize();
  
  for (int i = 0; i < palette_size; ++i) {
	settings.color_palette.push_back(serialization_.GetPaletteColor(i));
  }
  
  renderer_.SetSettings(settings);
}
