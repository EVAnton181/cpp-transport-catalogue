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


#include "domain.h"

namespace catalog {
/*!
 * @brief Реализация класса транспортного каталога
 * @class TransportCatalogue
 * 
 * Класс хранения и доступа к информации об автобусных остановках и 
 * автобусных маршрутах.
 * Данный класс не работает с вводом и выводом.
 */
    using KeyStops = std::pair<domain::Stop*, domain::Stop*>;

    class TransportCatalogue {
//     private:
//         struct Stop;
//         struct Bus;
        
    public:
// 		struct BusInfo {
// 			int stops_on_route;
// 			int unique_stops;
// 			double route_length;
// 			double curvature;
// 			
// // 			BusInfo()
// 		};
		
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
		// Передать stops_name по константной ссылке не могу, так как в данном методе для получения колличества уникальных остановок применяю алгоритм std::unique который модифицирует вектор
        void AddBus(std::string_view name, std::vector<std::string_view>& stops_name, bool flag);
        
        /*!
         * Добавляет новую остановку в каталог
         * 
         * @param name Имя остановки
         * @param lat Географическая широта остановки в градусах
         * @param lng Географическая долгота остановки в градусах
         * 
         * @return None
        */
		// В модул geo.h добавил конструктор, который создает структуру из двух double, а добавляющий не знает как храняться данные, он просто передает широту и долготу
        void AddStop(std::string_view name, double lat, double lng);
        
		/*!
         * Добавляет расстояние между остановками в контейнер с расстояниями
         * 
         * @param departure_stop остановка отправлния 
		 * @param arrival_stop остановка прибытия
		 * @param distance растояние между остановками
         * 
         * @return None
        */
        void SetDistance(domain::Stop* departure_stop, domain::Stop* arrival_stop, double distance);
		
        /*!
         * Ищет информацию об остановке в каталоге
         * 
         * @param name Имя остановки которую необходимо найти
         * 
         * @return Ссылку на структуру с описанием остановки
        */
        domain::Stop* FindStop(const std::string& name);
                
        /*!
         * Ищет информацию о маршруте в каталоге
         * 
         * @param name Имя маршрута который необходимо найти
         * 
         * @return Ссылку на структуру с описанием маршрута, если маршрут не найден - nullptr
        */
        domain::Bus* FindBus(const std::string_view& name) const;
        
        /*!
         * Преобразует данные о маршруте "name" в общее кол-во сотановок, 
         * кол-во уникальных остановок и длину маршрута и "кривизну" маршрута
         * 
         * @param[in] bus_name Имя маршрута, который необходимо найти
         * 
         * @return tuple: кол-во остановок на маршруте, кол-во уникальних остановок, длину маршрута, "кривизну маршрута"
         * 
         */
        std::optional<domain::BusStat> GetBusStat(const std::string_view& bus_name) const;
        
        /*!
         * Получает список маршрутов проходящих через заданную остановку
         * 
         * @param[in] name Имя остановки
         * 
         * @return вектор с названиями маршрутов
         * 
         */
        std::optional<std::vector<std::string_view>> GetBusesByStop(const std::string_view& stop_name) const;
        
        /*!
         * Возвращает расстояние между остановками
         * 
         * @param stop1 Указатель на остановку от которой ищем расстояние
         * @param stop2 Указатель на остановку до которой ищем расстояние
         * 
         * @return Если в массиве расстояний записано расстояние, то возвращаем его, в противном случае nullptr.
         * 
         */
        std::optional<double> GetDistance(domain::Stop* stop1, domain::Stop* stop2) const;
        
		/*!
         * Возвращает список названий маршрутов проходящих через заданную остановку
         * 
         * @param stop_name Имя остановки, в которой ищутся проходящие маршруты
         * 
         * @return список имен остановок
         * 
         */
// 		const std::unordered_set<std::string_view>* GetBusesSet(const std::string_view& stop_name) const;
		
		/*!
         * Возвращает отсортированный список маршрутов, в которые входит хотябы одна остановка
         * 
         * @return отсортированный vector с указателями на маршруты, в коротых есть хотябы одна остановка
         * 
         */
		std::vector<const domain::Bus*> GetSortBusesToRender() const;
		
		/*!
         * Возвращает список маршрутов, в которые входит хотябы одна остановка
         * 
         * @return set с указателями на маршруты, в коротых есть хотябы одна остановка
         * 
         */
        std::unordered_set<const domain::Bus*> GetBusesToRender() const;
		
		
		/*!
         * Возвращает вектор координат остановкок через которые проходит хотябы один маршрут
         * 
         * @return отсортированный vector координат остановкок через которые проходит хотябы один маршрут
         * 
         */
		std::vector<geo::Coordinates> GetAllRenderGeoCoordinates() const;
		
		/*!
         * Возвращает отсортированный вектор остановкок через которые проходит хотябы один маршрут
         * 
         * @return отсортированный vector с указателями на остановки входящие хотябы в один маршрут
         * 
         */
		std::vector<const domain::Stop*> GetSortStopsToRender() const;
		
		/*!
         * Возвращает остановки через которые проходит хотябы один маршрут
         * 
         * @return set с указателями на остановки входящие хотябы в один маршрут
         * 
         */
		std::unordered_set<const domain::Stop*> GetStopsToRender() const;
    private:
    
        std::deque<domain::Stop> stops_; /// Список всех остановок
        std::unordered_map<std::string_view, domain::Stop*> stopname_to_stop_; /// Контейнер для быстрого поиска остановки по названию
        
        std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stopname_to_buses_; /// set имен маршрутов, от имени остановки
 
        std::deque<domain::Bus> buses_;    /// Список всех маршрутов
        std::unordered_map<std::string_view, domain::Bus*> busname_to_bus_; /// Контейнер для быстрого поиска маршрутов по названию
        
        /// Хэшер для пары указателей
        struct pair_hash {
            size_t operator() (const std::pair<domain::Stop*, domain::Stop*> &p) const {
                auto h1 = std::hash<const void*>{}(p.first);
                auto h2 = std::hash<const void*>{}(p.second);
                
                return h1 * 47 + h2;
            }
        };
        
        using KeyStops = std::pair<domain::Stop*, domain::Stop*>;
        std::unordered_map<KeyStops, double, pair_hash> distance_; /// Контейнер с дистанциями между остановками
       
    };
}
