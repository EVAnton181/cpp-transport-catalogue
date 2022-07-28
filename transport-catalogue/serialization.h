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
	
	void InitRenderSettiingsParam(double width, double heidht, double padding, double line_width, double stop_radius, int bus_lable_font_size, int stop_lable_font_size, double underlayer_width);
	
	void InitRenderPoint(double bus_x, double bus_y, double stop_x, double stop_y);
	
	void InitRenderColor(svg::Color underlayer_color, std::vector<svg::Color> color_palette);
//     void InitSerializationCatalog(const catalog::TransportCatalogue& catalog);
    
    void SaveTo() const;
    
    void DeserializeTransportCatalogue(catalog::TransportCatalogue& catalog);
    
    void LoadFrom();
    
private:
	std::string file_;
    catalog_buf::Catalog serialization_catalog_;
    
    catalog_buf::Color ConvertColor(std::monostate);
    catalog_buf::Color ConvertColor(std::string color);
    catalog_buf::Color ConvertColor(svg::Rgb color);
    catalog_buf::Color ConvertColor(svg::Rgba color);
};
}
