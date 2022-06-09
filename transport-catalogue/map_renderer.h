/*!
 * @file map_renderer.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с классами для отрисовки транспортного каталога на карту
 * 
*/
#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <cmath>
#include <set>
#include <sstream>

#include "svg.h"
#include "geo.h"
#include "domain.h"

namespace map_renderer {

/// Структура с настройками рендера
struct RenderSettings {
	double width;  // ширина области рисования в пикселях
	double height;  // высота области рисования в пикселях
	double padding;  // ширина отступов от краев
    double line_width; // толщина линий, которыми рисуются автобусные маршруты.
    double stop_radius; // радиус окружностей, которыми обозначаются остановки.
    int bus_label_font_size; // размер текста, которым написаны названия автобусных маршрутов. 
    svg::Point bus_label_offset; // смещение надписи с названием маршрута относительно координат конечной остановки на карте
    int stop_label_font_size; // размер текста, которым отображаются названия остановок
    svg::Point stop_label_offset; // смещение названия остановки относительно её координат на карте.
    svg::Color underlayer_color; //цвет подложки под названиями остановок и маршрутов
    double underlayer_width; //  толщина подложки под названиями остановок и маршрутов
    std::vector<svg::Color> color_palette;  // цветовая палитра.
};

inline const double EPSILON = 1e-6;

class SphereProjector {
public:
	SphereProjector() {}
		
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
		if (!(std::abs(max_lat_ - min_lat) < EPSILON)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
		if (!(std::abs(max_lat_ - min_lat) < EPSILON)) {
			height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

	// Проецирует широту и долготу в координаты внутри SVG-изображения
	svg::Point operator()(geo::Coordinates coords) const {
		return {
			(coords.lng - min_lon_) * zoom_coeff_ + padding_,
			(max_lat_ - coords.lat) * zoom_coeff_ + padding_
		};
	}

private:
    double padding_ = 0;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

class MapRanderer {
public:
	MapRanderer() {}
	void SetSettings(RenderSettings set);
	
	void SetConverter(std::vector<geo::Coordinates>& all_geo_coordinates);
	
	void AddBusOnMap(std::vector<geo::Coordinates>& stops_geo_coordinates, bool round_tip, int number_bus);
	
	void AddBusNameOnMap(const std::string& name, geo::Coordinates& geo_coordinates, int number_bus);
	
	void AddStopOnMap(const geo::Coordinates& geo_coordinates);
	
	void AddStopNameOnMap(const std::string& name, const geo::Coordinates& geo_coordinates);
	
	void PrintRander(std::ostream& out = std::cout);
private:
	RenderSettings settings_;
	svg::Document rander_;
	SphereProjector converter_;
};
} // namespace map_renderer
