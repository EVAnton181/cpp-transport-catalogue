#include "stat_reader.h"

/*
std::pair<std::string_view, std::string_view> SeparateQuery(std::string_view line, char by) {
    size_t pos = line.find(by);
    std::string_view left = line.substr(0, pos);

    if (pos < line.size() && pos + 1 < line.size()) {
        return {left, line.substr(pos + 1)};
    } else {
        return {left, std::string_view()};
    }
}

std::string_view RetrieveQuery(std::string_view line) {
    std::string_view word = "";
    auto pos_start = line.find_first_not_of(' ', 0);
    if (pos_start == line.npos) {
        return word;
    }
    
    auto pos_last = line.find_last_not_of(' ', line.size());
    
    word = line.substr(pos_start, pos_last+1-pos_start);
    
    return word;
}*/

// using namespace statquery;
void statquery::Query(catalog::TransportCatalogue& cat) {
    int n = 0;
    std::cin >> n;
    std::string s;
    getline(std::cin, s);
    
    for (int i = 0; i < n; ++i) {
    std::string line;
    getline(std::cin, line);
    
    auto [query_name, query_string] = detail::SeparateRequests(line, ' ');
    auto name = std::string(detail::Retrieve(query_string));
        if (query_name == "Bus") {
            auto [stops_on_route, unique_stops, route_length, curvature] = cat.GetBusInfo(name);
            
            if (stops_on_route == 0) {
                std::cout << "Bus " << name << ": not found" << std::endl;
            } 
            else {
                std::cout << "Bus " << name << ": "<< stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << std::endl;
            }    
        }
        else {
            std::cout << "Stop " << name << ": ";
            
            if (!cat.FindStop(name)) {
                std::cout << "not found" << std::endl;
                continue;
            }
            
            auto buses = cat.GetBusList(name);

            if (buses.empty()) {
				std::cout << "no buses" << std::endl;
				continue;
			}
			
            std::cout << "buses";
			
			for (auto bus : buses) {
				std::cout << " " << bus;
			}
			std::cout << std::endl;
            
        }
    }
}
    

