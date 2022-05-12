/*!
 * @file input_reader.h
 * @brief Заголовочный файл с описанием функций чтения запросов 
 * на заполненеи базы
 * 
 * @author Elistratov Anton
 * 
 * @version 1.0
 * @date Май 2022
 * 
 * Данный файл содержит функции чтения запросов
*/

#pragma once

#include <string>
#include <string_view>

#include <vector>
#include <unordered_map>
#include <tuple>
#include <iostream>

#include "transport_catalogue.h"

namespace detail{
std::pair<std::string_view, std::string_view> SeparateRequests(std::string_view line, char by);

std::string_view Retrieve(std::string_view line);

std::vector<std::string_view> SplitIntoWords(const std::string_view text, char by);
    
}

namespace filling{
void AddStopInCat(catalog::TransportCatalogue& catalog, std::vector<std::string>& lines_with_stops);

void AddStopDistance(catalog::TransportCatalogue& catalog, const std::vector<std::string>& stop_distance_lines);

void AddBusInCat(catalog::TransportCatalogue& catalog, const std::vector<std::string>& lines_with_buses);

void FillingCatalog(catalog::TransportCatalogue& catalog, std::istream& input = std::cin);
}
