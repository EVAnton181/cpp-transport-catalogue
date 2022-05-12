/*!
 * @file transport_catalogue.h
 * @author Elistratov Anton
 * @date Май 2022
 * @version 1.0
 * 
 * @brief Заголовочный файл с описанием класса TransportCatalogue
 * 
 * Данный файл содержит в себе определение каласса 
 * транспортного каталога.
*/
#pragma once

#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <unordered_set>


#include <iostream>

#include "geo.h"

namespace catalog {
/*!
 * @brief Реализация класса транспортного каталога
 * @class TransportCatalogue
 * 
 * Класс хранения и доступа к информации об автобусных остановках и 
 * автобусных маршрутах.
 * Данный класс не работает с вводом и выводом.
 */
    class TransportCatalogue {
    private:
        struct Stop;
        struct Bus;
        
    public:
        TransportCatalogue() {}
        
        /*!
         * Добавляет новый маршрут в каталог
         * 
         * @param name Имя маршрута
         * @param stops_name Список остановок маршрута
         * @param flag Флаг является ли маршрут кольцевым
         * 
         * @return None
        */
        void AddBus(std::string_view name, std::vector<std::string_view> stops_name, bool flag);
        
        /*!
         * Добавляет новую остановку в каталог
         * 
         * @param name Имя остановки
         * @param lat Географическая широта остановки в градусах
         * @param lng Географическая долгота остановки в градусах
         * 
         * @return None
        */
        void AddStop(std::string_view name, double lat, double lng);
        
		/*!
         * Добавляет расстояние между остановками в контейнер с расстояниями
         * 
         * @param key_pair ключ из указателей на остановки между которыми определена дистанция
         * @param distance растояние между остановками
         * 
         * @return None
        */
        void AddDistance(std::pair<Stop*, Stop*> key_pair, double distance);
		
        /*!
         * Ищет информацию об остановке в каталоге
         * 
         * @param name Имя остановки которую необходимо найти
         * 
         * @return Ссылку на структуру с описанием остановки
        */
        Stop* FindStop(std::string name);
                
        /*!
         * Ищет информацию о маршруте в каталоге
         * 
         * @param name Имя маршрута который необходимо найти
         * 
         * @return Ссылку на структуру с описанием маршрута, если маршрут не найден - nullptr
        */
        Bus* FindBus(std::string name);
        
        /*!
         * Преобразует данные о маршруте "name" в общее кол-во сотановок, 
         * кол-во уникальных остановок и длину маршрута и "кривизну" маршрута
         * 
         * @param[in] name Имя маршрута, который необходимо найти
         * 
         * @return tuple: кол-во остановок на маршруте, кол-во уникальних остановок, длину маршрута, "кривизну маршрута"
         * 
         */
        std::tuple<int, int, double, double>  GetBusInfo(std::string name);
        
        /*!
         * Получает список маршрутов проходящих через заданную остановку
         * 
         * @param[in] name Имя остановки
         * 
         * @return вектор с названиями маршрутов
         * 
         */
        std::vector<std::string_view> GetBusList(std::string name);
        
    private:
        /// Структура хранит информацию об остановке
        struct Stop {
            std::string stop_name;
            Coordinates geo_point;
            
            Stop(std::string p_name, double p_latitude, double p_longitude)
                : stop_name(std::move(p_name)), geo_point(p_latitude, p_longitude) {
                }
        };
        
        std::deque<Stop> stops_; /// Список всех остановок
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_; /// Контейнер для быстрого поиска остановки по названию
        
        std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stopname_to_buses_; /// set имен маршрутов, от имени остановки
        
        /// Структура хранит информацию о маршрутах
        struct Bus {
            std::string bus;
            std::vector<Stop*> stops;
            bool round_trip;
            int uni_stops;
            
            Bus(std::string p_bus, std::vector<Stop*> p_stops, bool p_flag, int p_uni) 
                : bus(std::move(p_bus)), stops(std::move(p_stops)), round_trip(p_flag), uni_stops(p_uni) {
            }
        };
 
        std::deque<Bus> buses_;    /// Список всех маршрутов
        std::unordered_map<std::string_view, Bus*> busname_to_bus_; /// Контейнер для быстрого поиска маршрутов по названию
        
        /// Хэшер для пары указателей
        struct pair_hash {
            size_t operator() (const std::pair<Stop*, Stop*> &p) const {
                auto h1 = std::hash<const void*>{}(p.first);
                auto h2 = std::hash<const void*>{}(p.second);
                
                return h1 * 47 + h2;
            }
        };
        
        using KeyStops = std::pair<Stop*, Stop*>;
        std::unordered_map<KeyStops, double, pair_hash> distance_; /// Контейнер с дистанциями между остановками
       
    };
}
