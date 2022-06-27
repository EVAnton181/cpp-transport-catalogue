/*!
 * @file json_reader.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 2.0
 * 
 * @brief Заголовочный файл с функциями для работы с 
 * входными данными в формате json
 * 
 * Данный файл содержит в себе функции считывания данных из json файла.
 * Обработку запросов на добавление элементов в каталог и запросов статистики.
 * 
 * 08/06/2022 Изменен способ формирования ответа на запросы. 
 * Выходная json структура формируется с использованием json_builder'a.
*/
#pragma once

#include <sstream>

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

/*!
	* Обрабатывает json структуру содержащую запрос на добавление 
	* остановки в каталог
	* 
	* @param catalog ссылка на транспортный каталог
	* @param map_with_stop json структура с описанием параметров остановки
	* 
	* @return None
*/
void AddStopInCatalog(catalog::TransportCatalogue& catalog, const json::Dict& map_with_stop);

/*!
	* Обрабатывает json структуру содержащую параметры остановки,
	* для добавления растояний между остановками в каталог
	* 
	* @param catalog ссылка на транспортный каталог
	* @param map_with_stop json структура с описанием параметров остановки
	* 
	* @return None
*/
void AddStopDistance(catalog::TransportCatalogue& catalog, const json::Dict& map_with_stop);

/*!
	* Обрабатывает json структуру содержащую запрос на добавление маршрутов в каталог
	* 
	* @param catalog ссылка на транспортный каталог
	* @param map_with_bus json структура с описанием параметров маршрута
	* 
	* @return None
*/
void AddBusInCatalog(catalog::TransportCatalogue& catalog, const json::Dict& map_with_bus);

/*!
	* Обрабатывает json структуру содержащую запрос на добавление настроек маршрута (время ожидания автобуса и скорость автобуса)
	* 
	* @param catalog ссылка на транспортный каталог
	* @param map_with_setting json структура с настройками маршрута
	* 
	* @return None
*/
void AddRoutingSettingInCatalog(catalog::TransportCatalogue& catalog, const json::Node& map_with_setting);

/*!
    * Создаем ??????
    * 
    * @param catalog ссылка на транспортный каталог
    * 
    * @return None
*/
void BuildGraph(catalog::TransportCatalogue& catalog);

/*!
	* Обрабатывает json с базой данных и вызывает соответствующую функцию
	* обработки запросов
	* 
	* @param catalog ссылка на транспортный каталог
	* @param base_requests json база данных для добавления в каталог
	* 
	* @return None
*/
void CompleteCatalog(catalog::TransportCatalogue& catalog, const json::Node& base_requests);

/*!
	* Получает цвет из json структуры в фармате строки, RGB или RGBA
	* 
	* 
	* @param param json структура содержащая цвет в одном из форматов
	* 
	* @return цвет
*/
std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> GetColor(json::Node& param);

/*!
	* Устанавливает параметры отрисовки транспортного справочника, полученные из json документа
	* 
	* @param map ссылка на карту (графическое представление транспортного каталога)
	* @param render_settings сjson структура содержащая параметры отрисовки транспортного справочника
	* 
	* 
	* @return None
*/
void SetRenderSetting(map_renderer::MapRanderer& map, const json::Node& render_settings);

/*!
	* Формирует ответ в json формате на запрос информации о маршруте
	* 
	* @param handler ссылка на класс содержащий информацию о транспрортном справочкике и ссылку на карту
	* @param requests запрос о маршруте
	* 
	* 
	* @return json словарь с информацией о маршруте
*/
json::Dict MakeBusDict(const RequestHandler& handler, const json::Node& requests);

/*!
	* Формирует ответ в json формате на запрос информации об остановке
	* 
	* @param handler ссылка на класс содержащий информацию о транспрортном справочкике и ссылку на карту
	* @param requests запрос об остановке
	* 
	* 
	* @return json словарь с информацией об остановке
*/
json::Dict MakeStopDict(const RequestHandler& handler, const json::Node& requests);

/*!
	* Формирует ответ в json формате на запрос о выводе карты
	* 
	* @param handler ссылка на класс содержащий информацию о транспрортном справочкике и ссылку на карту
	* @param requests запрос об остановке
	* 
	* 
	* @return json словарь с о карте в формате svg
*/
json::Dict MakeMapDict(const RequestHandler& handler, const json::Node& requests);

/*!
	* Формирует ответ в json формате на запрос о пути между остановками
	* 
	* @param handler ссылка на класс содержащий информацию о транспрортном справочкике и ссылку на карту
	* @param requests запрос об остановке
	* 
	* 
	* @return json словарь с пути между остановками
*/
json::Dict MakeRouteDict(const RequestHandler& handler, const json::Node& requests);

/*!
	* Выдает статистику о маршрутах, остановках и выводит из в out
	* 
	* @param handler ссылка на класс содержащий информацию о транспрортном справочкике и ссылку на карту
	* @param stat_requests массив запросов
	* @param out выходной поток
	* 
	* @return None
*/
void GetStatistic(const RequestHandler& handler, const json::Node& stat_requests, std::ostream& out);

/*!
	* Формирует json массив из входного потока и передает управление функциям обработчикам запросов
	* 
	* @param catalog ссылка на транспортный каталог
	* @param map ссылка на карту (графическое представление транспортного каталога)
	* @param input входной поток
	* @param out выходной поток
	* 
	* @return None
*/
void LoadJSON(catalog::TransportCatalogue& catalog, map_renderer::MapRanderer& map, std::istream& input = std::cin, std::ostream& out  = std::cout);
