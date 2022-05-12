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

#include <cmath>

struct Coordinates {
    double lat;
    double lng;
    
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
    
    Coordinates(double p_latitude, double p_longitude)
        : lat(p_latitude), lng(p_longitude) {
        }
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * 6371000;
}
