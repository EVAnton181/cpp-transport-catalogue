/*!
 * @file domain.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с описанием структур
 * 
*/
#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "geo.h"
namespace domain {
    
/// Структура с описанием остановки
struct Stop {
    std::string stop_name;                                  ///< Название остановки
    geo::Coordinates geo_point;                             ///< Географические координаты остановки
    size_t stop_id;                                         ///< Порядковый номер остановки
    
    Stop(std::string p_name, double p_latitude, double p_longitude, size_t id)
		:stop_name(std::move(p_name))
		,geo_point({p_latitude, p_longitude}) 
        ,stop_id(id)
	{
	}
};

/// Структура с описанием маршрута
struct Bus {
    std::string bus;                                        ///< Название маршрута
    std::vector<Stop*> stops;                               ///< Вектор указателей на остановки входящие в маршрут
    bool round_trip;                                        ///< Флаг является ли маршрут кольцевым
    int uni_stops;                                          ///< Колличество уникальных остановок
    
    Bus(std::string p_bus, std::vector<domain::Stop*> p_stops, bool p_flag, int p_uni) 
        :bus(std::move(p_bus))
		,stops(std::move(p_stops))
		,round_trip(p_flag)
		,uni_stops(p_uni) 
	{
    }
};

/// Структура со статискикой маршрута
struct BusStat {
  double curvature = 0;                                     ///< Кривизна маршрут (отнашение реальной длины маршрута (по дорогам) к сумме географических растояний (по прямой) между остановками)
  double route_length = 0;                                  ///< Реальная длина маршрута
  int stop_count = 0;                                       ///< Колличество остановок на маршруте
  int unique_stop_count = 0;                                ///< Колличество уникальных остановок
};

/// Структура с настройками для поиска кратчайших маршрутов
struct RoutingSetting {
  int wait_time = 0;                                        ///< Время ожидания автобуса на остановке (мин.)
  int bus_velocity = 0;                                     ///< Средняя скорость автобуса между остановками (км./ч.)
};

/// Структура с информацией о маршруте
struct RouteInfo {
    std::string_view wait_stop;                             ///< Название остановки ожидания автобуса
    double wait_time;                                       ///< Время ожидания автобуса
    std::string_view bus_name;                              ///< Название маршрута
    int span_count;                                         ///< Количество остановок которое надо проехать на данном автобусе
    double time;                                            ///< Время затраченное на проезд на данном автобусе
};

/// Структура для сериализации графа
struct ForSerializationGraph {
  int from;                                                 ///< Id остановки отправления (Вершина графа)
  int to;                                                   ///< Id остановки прибытия (Вершина графа)
  double weight;                                            ///< Вес ребра
  int stops_count;                                          ///< Колличество остановок на пути между вершинами
  std::string bus_name;                                     ///< Название маршрута
};

}    //namespace domain
