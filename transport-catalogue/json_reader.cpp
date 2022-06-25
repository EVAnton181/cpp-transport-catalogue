#include "json_reader.h"

using namespace std::literals;


void AddStopInCatalog(catalog::TransportCatalogue& catalog, const json::Dict& map_with_stop) {
	std::string name = map_with_stop.at("name").AsString();
	double lat = map_with_stop.at("latitude").AsDouble();
	double lng = map_with_stop.at("longitude").AsDouble();
	
	catalog.AddStop(name, lat, lng);  
}

void AddStopDistance(catalog::TransportCatalogue& catalog, const json::Dict& map_with_stop) {
	
	std::string stop_name = map_with_stop.at("name").AsString();
    auto departure_stop = catalog.FindStop(std::string(stop_name));
	auto map_with_dist = map_with_stop.at("road_distances").AsDict();
	
	for (auto& stop : map_with_dist) {
        auto arrival_stop = catalog.FindStop(stop.first);
		double dist = stop.second.AsDouble();
		catalog.SetDistance(departure_stop, arrival_stop, dist);
	}
}

void AddBusInCatalog(catalog::TransportCatalogue& catalog, const json::Dict& map_with_bus) {
    
    std::string bus_name = map_with_bus.at("name").AsString();
    auto node_stops = map_with_bus.at("stops").AsArray();
    std::vector<std::string_view> stops;
    stops.reserve(node_stops.size());
    
	for (auto& stop : node_stops) {
        stops.push_back(stop.AsString());
    }
    
    bool round = map_with_bus.at("is_roundtrip").AsBool();
    
    catalog.AddBus(bus_name, stops, round);  
}


void CompleteCatalog(catalog::TransportCatalogue& catalog, const json::Node& base_requests) {
    for (auto& input_modul : base_requests.AsArray()) {
		if (input_modul.AsDict().at("type").AsString() == "Stop") {
			AddStopInCatalog(catalog, input_modul.AsDict());
		}
	}
    for (auto& input_modul : base_requests.AsArray()) {
		if (input_modul.AsDict().at("type").AsString() == "Stop") {
			AddStopDistance(catalog, input_modul.AsDict());
		}
	}
	for (auto& input_modul : base_requests.AsArray()) {
		if (input_modul.AsDict().at("type").AsString() == "Bus") {
			AddBusInCatalog(catalog, input_modul.AsDict());
		}
	}
}

void AddRoutingSettingInCatalog(catalog::TransportCatalogue& catalog, const json::Node& map_with_setting) {
    int wait_time =  map_with_setting.AsDict().at("bus_wait_time").AsInt();
    int bus_velocity =  map_with_setting.AsDict().at("bus_velocity").AsInt();
    
    catalog.AddRoutingSetting(wait_time, bus_velocity); 
}

void BuildRouter(catalog::TransportCatalogue& catalog) {
    catalog.InitRouterGraph();
    catalog.AddEdgeInRouterGraph();
}

std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> GetColor(const json::Node& param) {
	if (param.IsString()) {
		return param.AsString();
	} else if (param.IsArray()) {
		auto array = param.AsArray();
		if (array.size() == 3) {
			return svg::Rgb{static_cast<uint8_t>(array[0].AsInt()), static_cast<uint8_t>(array[1].AsInt()), static_cast<uint8_t>(array[2].AsInt())};
		} else {
			return svg::Rgba{static_cast<uint8_t>(array[0].AsInt()), static_cast<uint8_t>(array[1].AsInt()), static_cast<uint8_t>(array[2].AsInt()), array[3].AsDouble()};
		}
	}
	return nullptr;
}

void BuildGraph(catalog::TransportCatalogue& catalog) {
    catalog.InitRouterGraph();
    catalog.AddEdgeInRouterGraph();
}

void SetRenderSetting(map_renderer::MapRanderer& map, const json::Node& render_settings) {
    map_renderer::RenderSettings settings;
    std::ostringstream out;
    for (const auto& [param_name, param]: render_settings.AsDict()) {
		if (param_name == "width") {
			settings.width = param.AsDouble();
		}
		if (param_name == "height") {
			settings.height = param.AsDouble();
		}
		if (param_name == "padding") {
			settings.padding = param.AsDouble();
		}
		if (param_name == "stop_radius") {
			settings.stop_radius = param.AsDouble();
		}
		if (param_name == "line_width") {
			settings.line_width = param.AsDouble();
		}
		if (param_name == "bus_label_font_size") {
			settings.bus_label_font_size = param.AsDouble();
		}
        if (param_name == "bus_label_offset") { 
			settings.bus_label_offset.x = param.AsArray()[0].AsDouble();
			settings.bus_label_offset.y = param.AsArray()[1].AsDouble();
		}
		if (param_name == "stop_label_font_size") {
			settings.stop_label_font_size = param.AsDouble();
		}
		if (param_name == "stop_label_offset") {
			settings.stop_label_offset.x = param.AsArray()[0].AsDouble();
			settings.stop_label_offset.y = param.AsArray()[1].AsDouble();
		}
		if (param_name == "underlayer_color") {
			settings.underlayer_color = GetColor(param);
		}
		if (param_name == "underlayer_width") {
			settings.underlayer_width = param.AsDouble();
		}
		if (param_name == "color_palette") {
			for (const auto json_color : param.AsArray()) {
				auto color = GetColor(json_color);
				settings.color_palette.push_back(color);
			}
		}
	}
	map.SetSettings(settings);
}
    
json::Dict MakeBusDict(const RequestHandler& handler, const json::Node& requests) {
    auto anser = handler.GetBusStat(requests.AsDict().at("name").AsString());
    
	if ( !anser ) {
		return json::Builder{}.StartDict()
								.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
						.Key("error_message"s).Value("not found"s)
					.EndDict()
				.Build()
				.AsDict();
    } else {
		return json::Builder{}
					.StartDict()
						.Key("curvature"s).Value(anser.value().curvature)
						.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
						.Key("route_length"s).Value(anser.value().route_length)
						.Key("stop_count"s).Value(anser.value().stop_count)
						.Key("unique_stop_count"s).Value(anser.value().unique_stop_count)
					.EndDict()
				.Build()
				.AsDict();
    }
}

json::Dict MakeStopDict(const RequestHandler& handler, const json::Node& requests) {
    auto anser = handler.GetBusesByStop(requests.AsDict().at("name").AsString());

	if ( !anser ) {
		return json::Builder{}
					.StartDict()
						.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
						.Key("error_message"s).Value("not found"s)
					.EndDict()
				.Build()
				.AsDict();
	} else {
		if (anser.value().empty()) {
			return json::Builder{}
					.StartDict()
						.Key("buses"s).Value(json::Array({}))
						.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
					.EndDict()
				.Build()
				.AsDict();
		} else {
			json::Array buses_names;
			for (auto bus : anser.value()) {
				buses_names.push_back(std::string(bus));
			}
			return json::Builder{}
					.StartDict()
						.Key("buses"s).Value(buses_names)
						.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
					.EndDict()
				.Build()
				.AsDict();
		}
	}
}

json::Dict MakeMapDict(const RequestHandler& handler, const json::Node& requests) {
	std::stringstream out;
	
	handler.RenderMap(out);
	
	return json::Builder{}
					.StartDict()
						.Key("map"s).Value(out.str())
						.Key("request_id"s).Value(requests.AsDict().at("id").AsInt())
					.EndDict()
				.Build()
				.AsDict();
}

void GetStatistic(const RequestHandler& handler, const json::Node& stat_requests, std::ostream& out) {
//     RequestHandler request(catalog);
    json::Array result;
//     RequestHandler req(catalog);
    for (auto& request : stat_requests.AsArray()) {
		auto req = request.AsDict();
        if (req.at("type").AsString() == "Bus") {
            result.push_back(MakeBusDict(handler, request));           
        } else if (request.AsDict().at("type").AsString() == "Stop") {
            result.push_back(MakeStopDict(handler, request));
        } else if (request.AsDict().at("type").AsString() == "Map") {
			result.push_back(MakeMapDict(handler, request));
		}
    }
	
	json::Print(
        json::Document{
            json::Builder{}
            .Value(result)
            .Build()
        },
        out
    );

}

void LoadJSON(catalog::TransportCatalogue& catalog, map_renderer::MapRanderer& map, std::istream& input,  std::ostream& out) {
	
	json::Document input_doc(json::Load(input));
    
	auto base_requests = input_doc.GetRoot().AsDict().at("base_requests");
    
    CompleteCatalog(catalog, base_requests);
    
    if (input_doc.GetRoot().AsDict().count("routing_settings")) {
		auto routing_settings = input_doc.GetRoot().AsDict().at("routing_settings");
		AddRoutingSettingInCatalog(catalog, routing_settings);
        BuildGraph(catalog);
	}
	
	if (input_doc.GetRoot().AsDict().count("render_settings")) {
		auto render_settings = input_doc.GetRoot().AsDict().at("render_settings");
		SetRenderSetting(map, render_settings);
	}
	
	RequestHandler handler(catalog, map);
//     handler.InitTransportRoutet();
	
	if (input_doc.GetRoot().AsDict().count("render_settings")) {
		handler.MakeRenderMap();
	}
	
	if (input_doc.GetRoot().AsDict().count("stat_requests")) {
		auto stat_requests = input_doc.GetRoot().AsDict().at("stat_requests");
		GetStatistic(handler, stat_requests, out);
	}
}
