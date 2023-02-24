/*!
 * @file geo.h
 * @brief Заголовочный файл с функциями для работы с 
 * географическими координатами
 * 
 * @author Practicum
 * 
 * @version 1.0
 * @date Май 2022
 * 
 * Данный файл содержит в себе структуру с координатами 
 * и функцию вычисления дистанции между двумя точками
*/
#pragma once

namespace geo {

/// Структура с географическими координатами
struct Coordinates {
    double lat; ///< Широта
	double lng; ///< Долгота
    
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

/*!
 * Функция вычисления растояния между координатами с учетом кривизны Земли
 * 
 * @param from Координаты откуда
 * @param to Координаты куда
 */
double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo
