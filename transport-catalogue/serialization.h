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
#include "transport_catalogue.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <unordered_map>

namespace serialization {
/*!
 * @brief Реализация класса сериализации транспортного каталога
 * @class Serialization
 * 
 * Класс хранит транспортный справочник в сериализованном виде.
 * Позволяет сериализовать и десериализовать транспортный справочник.
 * Позволяет загружать и сохранять транспортный справочник в сериализованном виде в заданный файл
 */

// using Path = std::filesystem::path;

class Serialization {
public:
	void SetFilePath(std::string file_path);
	
    void InitSerializationCatalog(const catalog::TransportCatalogue& catalog);
    
    void SaveTo() const;
    
    void DeserializeTransportCatalogue(catalog::TransportCatalogue& catalog);
    
    void LoadFrom();
    
private:
	std::string file_;
    catalog_buf::Catalog serialization_catalog_;
};
}
