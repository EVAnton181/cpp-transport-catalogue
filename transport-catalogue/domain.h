#pragma once

#include <string>
#include <vector>

#include "geo.h"
namespace domain {
    
struct Stop {
    std::string stop_name;
    geo::Coordinates geo_point;
    
    Stop(std::string p_name, double p_latitude, double p_longitude)
		:stop_name(std::move(p_name))
		,geo_point({p_latitude, p_longitude}) 
	{
	}
};


struct Bus {
    std::string bus;
    std::vector<Stop*> stops;
    bool round_trip;
    int uni_stops;
    
    Bus(std::string p_bus, std::vector<domain::Stop*> p_stops, bool p_flag, int p_uni) 
        :bus(std::move(p_bus))
		,stops(std::move(p_stops))
		,round_trip(p_flag)
		,uni_stops(p_uni) 
	{
    }
};

struct BusStat {
  double curvature = 0;
  double route_length = 0;
  int stop_count = 0;
  int unique_stop_count = 0;
};

}    //namespace domain
