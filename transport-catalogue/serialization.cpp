#include "serialization.h"

using namespace serialization;

void Serialization::SetFilePath(std::string file_path) {
  file_ = std::move(file_path);
}

void Serialization::InitSerializationStop(std::string stop_name,  double lat, double lng) {
    catalog_buf::Stop stop_pb;
    stop_pb.set_stop_name(stop_name);
    catalog_buf::Coordinates coordinate_pb;
    coordinate_pb.set_lat(lat);
    coordinate_pb.set_lng(lng);
    *stop_pb.mutable_point() = std::move(coordinate_pb);
    
    serialization_catalog_.add_stop();
    *serialization_catalog_.mutable_stop(serialization_catalog_.stop_size()-1) = std::move(stop_pb);
}


void Serialization::InitSerializationDistance(int stop_id_from, int stop_id_to,  double distance) {
    catalog_buf::Distance distance_pb;
    distance_pb.set_stop_id_from(stop_id_from);
    distance_pb.set_stop_id_to(stop_id_to);
    distance_pb.set_distance(distance);
    
    serialization_catalog_.add_map_distance();
    *serialization_catalog_.mutable_map_distance(serialization_catalog_.map_distance_size()-1) = std::move(distance_pb);
}

void Serialization::InitSerializationBus(std::string bus_name, bool round_trip, std::vector<int> bus_stops) {
    catalog_buf::Bus bus_pb;
    bus_pb.set_bus_name(bus_name);
    bus_pb.set_round_trip(round_trip);
    
    for (auto& stop_num : bus_stops) {
        bus_pb.add_stop_num(stop_num);
    }
    
    serialization_catalog_.add_bus();
    *serialization_catalog_.mutable_bus(serialization_catalog_.bus_size()-1) = std::move(bus_pb);
}

void Serialization::InitRoutingSettings(int wait_time,  double bus_velocity) {
    const double to_km = 1/1000;
    const double to_h = 1/60;
    
    catalog_buf::RoutingSetting settings_pb;
    settings_pb.set_wait_time(wait_time);
    settings_pb.set_bus_velocity(static_cast<int>(bus_velocity / to_h * to_km));
    
    *serialization_catalog_.mutable_routing_setting() = std::move(settings_pb);
}

void Serialization::InitRenderSettiingsParam(double width, double heidht, double padding, double line_width, double stop_radius, int bus_lable_font_size, int stop_lable_font_size, double underlayer_width) {
    catalog_buf::RenderSetting settings_pb;
    settings_pb.set_width(width);
	settings_pb.set_height(heidht);
	settings_pb.set_padding(padding);
    settings_pb.set_line_width(line_width);
    settings_pb.set_stop_radius(stop_radius);
    settings_pb.set_bus_label_font_size(bus_lable_font_size);
    settings_pb.set_stop_label_font_size(stop_lable_font_size);
    settings_pb.set_underlayer_width(underlayer_width);
    
    *serialization_catalog_.mutable_render_settings() = std::move(settings_pb);
}

void Serialization::InitRenderPoint(double bus_x, double bus_y, double stop_x, double stop_y) {
    catalog_buf::Point bus_point_pb;
    bus_point_pb.set_x(bus_x);
    bus_point_pb.set_y(bus_y);
    
    catalog_buf::Point stop_point_pb;
    stop_point_pb.set_x(stop_x);
    stop_point_pb.set_y(stop_y);
    
    catalog_buf::RenderSetting settings_pb = *serialization_catalog_.mutable_render_settings();
    
    *settings_pb.mutable_bus_label_offset() = std::move(bus_point_pb);
    *settings_pb.mutable_stop_label_offset() = std::move(stop_point_pb);
    
    *serialization_catalog_.mutable_render_settings() = std::move(settings_pb);
}

catalog_buf::Color Serialization::ConvertColor(std::monostate) {
    catalog_buf::Color conver_color;
    
    return conver_color;
}

catalog_buf::Color Serialization::ConvertColor(std::string color) {
    catalog_buf::SColor string_color;
    string_color.set_color_name(color);
    
    catalog_buf::Color conver_color;
    *conver_color.mutable_color() = std::move(string_color);
    
    return conver_color;
}

catalog_buf::Color Serialization::ConvertColor(svg::Rgb color) {
    catalog_buf::Rgb rgb_color;
    rgb_color.set_red(static_cast<int>(color.red));
    rgb_color.set_green(static_cast<int>(color.green));
    rgb_color.set_blue(static_cast<int>(color.blue));
    
    catalog_buf::Color conver_color;
    *conver_color.mutable_rgb() = std::move(rgb_color);
    
    return conver_color;
}

catalog_buf::Color Serialization::ConvertColor(svg::Rgba color) {
    catalog_buf::Rgba rgba_color;
    rgba_color.set_red(static_cast<int>(color.red));
    rgba_color.set_green(static_cast<int>(color.green));
    rgba_color.set_blue(static_cast<int>(color.blue));
    rgba_color.set_opacity(color.opacity);
    
    catalog_buf::Color conver_color;
    *conver_color.mutable_rgba() = std::move(rgba_color);
    
    return conver_color;
}

void Serialization::InitRenderColor(svg::Color underlayer_color, std::vector<svg::Color> color_palette) {
    catalog_buf::RenderSetting settings_pb = *serialization_catalog_.mutable_render_settings();
    
    catalog_buf::Color underlayer_color_pb;
    std::visit([&] (auto&& value) { underlayer_color_pb = ConvertColor(value); }, underlayer_color);
    *settings_pb.mutable_underlayer_color() = std::move(underlayer_color_pb);
    
    for (auto& color : color_palette) {
        catalog_buf::Color color_pb;
        std::visit([&] (auto&& value) { color_pb = ConvertColor(value); }, color);
        settings_pb.add_color_palette();
        *settings_pb.mutable_color_palette(settings_pb.color_palette_size() - 1) = std::move(color_pb);
    }
    
    *serialization_catalog_.mutable_render_settings() = std::move(settings_pb);
}

// // Сериализует каталог
/*
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
}*/

// Сохраняет сериализованный каталог в поток output
void Serialization::SaveTo() const {
	std::ofstream out_file(file_, std::ios::binary);
    serialization_catalog_.SerializeToOstream(&out_file);
}

int Serialization::GetStopCount() {
    return serialization_catalog_.stop_size();
}

std::tuple<std::string, double, double> Serialization::GetStopData(int i) {
    auto stop = serialization_catalog_.stop(i);
    return std::make_tuple(stop.stop_name(), stop.point().lat(), stop.point().lng());
}

int Serialization::GetMapSize() {
    return serialization_catalog_.map_distance_size();
}

std::tuple<int, int, double> Serialization::GetMapData(int i) {
    auto dist = serialization_catalog_.map_distance(i);
    return std::make_tuple(dist.stop_id_from(), dist.stop_id_to(), dist.distance());
}

int Serialization::GetBusCount() {
	return serialization_catalog_.bus_size();
}

std::string Serialization::GetBusName(int i) {
  return serialization_catalog_.bus(i).bus_name();
}

bool Serialization::GetRoundTripFlag(int i) {
  return serialization_catalog_.bus(i).round_trip();
}

std::vector<int> Serialization::GetStopsId(int i) {
  std::vector<int> stops_id;
  for (auto& id :  serialization_catalog_.bus(i).stop_num()) {
	stops_id.push_back(id);
  }
  return stops_id;
}

// Десериализуем каталог
void Serialization::DeserializeTransportCatalogue(catalog::TransportCatalogue& load_catalog) {
   
    int size = GetStopCount();
    
    for (int i = 0; i < size; ++i) {
        auto stop = serialization_catalog_.stop(i);
	  load_catalog.AddStop(stop.stop_name(), stop.point().lat(), stop.point().lng());
	}
	
	size = serialization_catalog_.map_distance_size();
    for (int i = 0; i < size; ++i) {
    auto dist = serialization_catalog_.map_distance(i);
	load_catalog.SetDistance(load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_from()))),  load_catalog.FindStop(std::string(load_catalog.GetStopNameFromId(dist.stop_id_to()))), dist.distance());
	}
	
    size = serialization_catalog_.bus_size();
	for (auto& bus : serialization_catalog_.bus()) {
     
        std::vector<std::string_view> stops;

        for (auto& stop_id : bus.stop_num()) {
            stops.push_back(load_catalog.GetStopNameFromId(stop_id));
        }

        load_catalog.AddBus(bus.bus_name(), stops, bus.round_trip());
	}
	
	load_catalog.AddRoutingSetting(serialization_catalog_.routing_setting().wait_time(), serialization_catalog_.routing_setting().bus_velocity());	
}

double Serialization::GetRenderWidth() {
	return serialization_catalog_.render_settings().width();
}
double Serialization::GetRenderHeight() {
    return serialization_catalog_.render_settings().height();
}

double Serialization::GetRenderPadding() {
    return serialization_catalog_.render_settings().padding();
}

double Serialization::GetRenderLineWidth() {
	  return serialization_catalog_.render_settings().line_width();
}

double Serialization::GetRenderStopRadius() {
    return serialization_catalog_.render_settings().stop_radius();
}

int Serialization::GetRenderBusLab() {
    return serialization_catalog_.render_settings().bus_label_font_size();
}

int Serialization::GetRenderStopLab() {
    return serialization_catalog_.render_settings().stop_label_font_size();
}

double Serialization::GetRenderUnderlayerWidth() {
    return serialization_catalog_.render_settings().underlayer_width();
}

std::pair<double, double> Serialization::GetOffset(std::string what) {
  catalog_buf::Point point_pb;
  if (what ==  "bus") {
	  point_pb = serialization_catalog_.render_settings().bus_label_offset();
  }
  else if (what == "stop") {
	  point_pb = serialization_catalog_.render_settings().stop_label_offset();
  }
  return std::make_pair(point_pb.x(),  point_pb.y());
}

svg::Color Serialization::ConvertBack(catalog_buf::Color& color) {
  svg::Color anser;
  if (color.has_color()) {
	anser = color.color().color_name();
  } 
  else if (color.has_rgb()) {
	svg::Rgb rgb(color.rgb().red(), color.rgb().green(), color.rgb().blue());
	anser = rgb;
  }
  else if (color.has_rgba()) {
	svg::Rgba rgba(color.rgba().red(), color.rgba().green(), color.rgba().blue(),  color.rgba().opacity());
	anser = rgba;
  }
  
  return anser;
}

svg::Color Serialization::GetColorUnder() {
  catalog_buf::Color color = serialization_catalog_.render_settings().underlayer_color();
  return ConvertBack(color);
}

svg::Color Serialization::GetPaletteColor(int i) {
  catalog_buf::Color color = serialization_catalog_.render_settings().color_palette(i);
  return ConvertBack(color);;
}

int Serialization::GetPaletteSize() {
  return serialization_catalog_.render_settings().color_palette_size();
}


// Загружает сериализованный каталог из file_
void Serialization::LoadFrom() {
	std::ifstream ifs(file_, std::ios::binary);
    if (!serialization_catalog_.ParseFromIstream(&ifs)) {
        std::cout << "Fatal ERROR" << std::endl;
    }
}
