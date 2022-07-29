/*!
 * @file serialization.h
 * @author Elistratov Anton
 * @date Июль 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с описанием класса Serialization
 * 
 * Данный файл содержит в себе определение каласса 
 * для сериализации и десериализации транспортного каталога.
*/
#pragma once


#include <transport_catalogue.pb.h>
#include "svg.h"

#include "transport_catalogue.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace serialization {
/*!
 * @brief Реализация класса сериализации транспортного каталога
 * @class Serialization
 * 
 * Класс хранит транспортный справочник в сериализованном виде.
 * Позволяет сериализовать и десериализовать транспортный справочник.
 * Позволяет загружать и сохранять транспортный справочник в сериализованном виде в заданный файл
 */

class Serialization {
public:
	void SetFilePath(std::string file_path);
	
	void InitSerializationStop(std::string stop_name,  double lat, double lng);
	
	void InitSerializationDistance(int stop_id_from, int stop_id_to,  double distance);
	
	void InitSerializationBus(std::string bus_name, bool round_trip, std::vector<int> bus_stops);
	
	void InitRoutingSettings(int wait_time,  double bus_velocity);
	
	void InitRenderSettiingsParam(double width, double heidht, double padding, double line_width, double stop_radius, int bus_lable_font_size, int stop_lable_font_size, double underlayer_width);
	
	void InitRenderPoint(double bus_x, double bus_y, double stop_x, double stop_y);
	
	void InitRenderColor(svg::Color underlayer_color, std::vector<svg::Color> color_palette);
//     void InitSerializationCatalog(const catalog::TransportCatalogue& catalog);
    int GetStopCount();

	std::tuple<std::string, double, double> GetStopData(int i);
	
	int GetMapSize();

	std::tuple<int, int, double> GetMapData(int i);

	int GetBusCount();

	std::string GetBusName(int i);
	
	bool GetRoundTripFlag(int i);

	std::vector<int> GetStopsId(int i);
    
    void DeserializeTransportCatalogue(catalog::TransportCatalogue& catalog);
	
	double GetRenderWidth();
	
	double GetRenderHeight();

	double GetRenderPadding();

	double GetRenderLineWidth();

	double GetRenderStopRadius();

	int GetRenderBusLab();

	int GetRenderStopLab();

	double GetRenderUnderlayerWidth();
	
	svg::Color GetColorUnder();
	
	int GetPaletteSize();
	
	svg::Color GetPaletteColor(int i);
	
	std::pair<double, double> GetOffset(std::string what);
	
    void LoadFrom();
    
    void SaveTo() const;
private:
	std::string file_;
    catalog_buf::Catalog serialization_catalog_;
    
    catalog_buf::Color ConvertColor(std::monostate);
    catalog_buf::Color ConvertColor(std::string color);
    catalog_buf::Color ConvertColor(svg::Rgb color);
    catalog_buf::Color ConvertColor(svg::Rgba color);
    svg::Color ConvertBack(catalog_buf::Color& color);
};
}
