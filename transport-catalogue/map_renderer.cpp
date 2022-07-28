#include "map_renderer.h"

using namespace map_renderer;


void MapRanderer::SetSettings(RenderSettings set) {
	settings_ = set;
}

void MapRanderer::SetConverter(std::vector<geo::Coordinates>& all_geo_coordinates) {
	SphereProjector converter{all_geo_coordinates.begin(), all_geo_coordinates.end(), settings_.width, settings_.height, settings_.padding};
	converter_ = std::move(converter);
}

void MapRanderer::AddBusOnMap(std::vector<geo::Coordinates>& stops_geo_coordinates, bool round_tip, int number_bus) {
	int num_color = number_bus % settings_.color_palette.size();
	svg::Color color_line = settings_.color_palette[num_color];
	svg::Polyline bus;
	bus.SetStrokeWidth(settings_.line_width).SetStrokeColor(color_line).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor({});
	for ( auto& stop_geo_coordinates : stops_geo_coordinates) {
		const svg::Point screen_coord = converter_(stop_geo_coordinates);
		bus.AddPoint(screen_coord);
	}
	
	if (!round_tip) {
		for (auto stop_it = stops_geo_coordinates.rbegin()+1; stop_it < stops_geo_coordinates.rend(); ++stop_it) {
			const svg::Point screen_coord = converter_(*stop_it);
		bus.AddPoint(screen_coord);
		}
	}
	
	rander_.Add(std::move(bus));
}

void MapRanderer::AddBusNameOnMap(const std::string& name, geo::Coordinates& geo_coordinates, int number_bus) {
	int num_color = number_bus % settings_.color_palette.size();
	svg::Color fill_color = settings_.color_palette[num_color];
	
	const svg::Point screen_coord = converter_(geo_coordinates);
	
	svg::Text bus_name;
	
	bus_name.SetFontFamily("Verdana")
                .SetPosition(screen_coord)
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontWeight("bold")
                .SetData(name)
				.SetFillColor(fill_color);
	
	svg::Text bus_underlayer;
	
	bus_underlayer.SetFontFamily("Verdana")
                .SetPosition(screen_coord)
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontWeight("bold")
                .SetData(name)
				.SetFillColor(settings_.underlayer_color)
				.SetStrokeColor(settings_.underlayer_color)
				.SetStrokeWidth(settings_.underlayer_width)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
				
	rander_.Add(std::move(bus_underlayer));
	rander_.Add(std::move(bus_name));
}
// 
void MapRanderer::AddStopOnMap(const geo::Coordinates& geo_coordinates){
	const svg::Point screen_coord = converter_(geo_coordinates);
	
	svg::Circle stop;

	stop.SetCenter(screen_coord)
		.SetRadius(settings_.stop_radius)
		.SetFillColor("white");
    
	rander_.Add(std::move(stop));
}

void MapRanderer::AddStopNameOnMap(const std::string& name, const geo::Coordinates& geo_coordinates){
	const svg::Point screen_coord = converter_(geo_coordinates);
	
	svg::Text stop_name;
	
	stop_name.SetFontFamily("Verdana")
                .SetPosition(screen_coord)
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetData(name)
				.SetFillColor("black");
	
	svg::Text stop_underlayer;
	
	stop_underlayer.SetFontFamily("Verdana")
                .SetPosition(screen_coord)
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetData(name)
				.SetFillColor(settings_.underlayer_color)
				.SetStrokeColor(settings_.underlayer_color)
				.SetStrokeWidth(settings_.underlayer_width)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
				
	rander_.Add(std::move(stop_underlayer));
	rander_.Add(std::move(stop_name));
}

void MapRanderer::PrintRander(std::ostream& out) {
	rander_.Render(out);
}

RenderSettings MapRanderer::GetSettings() const {
    return settings_;
}
