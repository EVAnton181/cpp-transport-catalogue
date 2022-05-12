/*!
 * @file stat_reader.h
 * @brief Заголовочный файл с описанием функций чтения запросов 
 * на вывод и сам вывод
 * 
 * @author Elistratov Anton
 * 
 * @version 1.0
 * @date Май 2022
 * 
 * Данный файл содержит функции чтения запросов на вывод 
 * и функции вывода
*/

#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <string_view>

#include "transport_catalogue.h"
#include "input_reader.h"

namespace statquery {
    
void Query(catalog::TransportCatalogue& cat);

}
