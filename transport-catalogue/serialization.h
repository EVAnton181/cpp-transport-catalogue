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

#include <transport_catalog.pb.h>

#include "transport_catalog.h"

#include <iostream>
#include <filesystem>
#include <fstream>

namespace serialization {
/*!
 * @brief Реализация класса сериализации транспортного каталога
 * @class Serialization
 * 
 * Класс хранит транспортный справочник в сериализованном виде.
 * Позволяет сериализовать и десериализовать транспортный справочник.
 * Позволяет загружать и сохранять транспортный справочник в сериализованном виде в заданный файл
 */

using Path = std::filesystem::path;

class Serialization {
public:
	void SetFilePath(std::string file_path);
	
    void InitSerializationCatalog(const catalog::TransportCatalogue& catalog);
    
    void SaveTo() const;
    
    catalog::TransportCatalogue DeserializeTransportCatalogue();
    
    void LoadFrom();
    
private:
	Path file_;
    catalog_buf::Catalog serialization_catalog_;
};
}
