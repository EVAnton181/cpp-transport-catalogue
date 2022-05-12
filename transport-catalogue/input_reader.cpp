#include "input_reader.h"

namespace detail {
        
    std::pair<std::string_view, std::string_view> SeparateRequests(std::string_view line, char by) {
        size_t pos = line.find(by);
        std::string_view left = line.substr(0, pos);

        if (pos < line.size() && pos + 1 < line.size()) {
            return {left, line.substr(pos + 1)};
        } else {
            return {left, std::string_view()};
        }
    }

    std::string_view Retrieve(std::string_view line) {
        std::string_view word = "";
        auto pos_start = line.find_first_not_of(' ', 0);
        if (pos_start == line.npos) {
            return word;
        }
        
        auto pos_last = line.find_last_not_of(' ', line.size());
        
        word = line.substr(pos_start, pos_last+1-pos_start);
        
        return word;
    }

    std::vector<std::string_view> SplitIntoWords(const std::string_view text, char by) {
        std::vector<std::string_view> words;
        int64_t pos = 0;
        const int64_t pos_end = text.npos;
        while (true) {
            int64_t space = text.find(by, pos);
            words.push_back(space == pos_end ? Retrieve(text.substr(pos)) : Retrieve(text.substr(pos, space - pos)));
            if (space == pos_end) {
                break;
            } else {
                pos = space + 1;
            }
        }

        return words;
    }
}

namespace filling {

// В данной функции не использую константную ссылку у lines_with_stops, так как в процессе (на 66 строке) изменяю строку, убирая из нее координаты остановки!
void AddStopInCat(catalog::TransportCatalogue& catalog, std::vector<std::string>& lines_with_stops) {
    for (auto& stop_line : lines_with_stops) {
        
		auto [name, coordinates] = detail::SeparateRequests(stop_line, ':');
        
		name = detail::Retrieve(name);
        
		auto [lat_s, line_residue] = detail::SeparateRequests(coordinates, ',');
        auto [lng_s, line_distance] = detail::SeparateRequests(line_residue, ',');        
        
		double lat = std::stod(std::string(detail::Retrieve(lat_s)));
        double lng = std::stod(std::string(detail::Retrieve(lng_s)));

		catalog.AddStop(name, lat, lng);  
        
		stop_line = std::string(name) + ": " + std::string(line_distance);
    }
}



void AddStopDistance(catalog::TransportCatalogue& catalog, const std::vector<std::string>& stop_distance_lines) {
	for (auto& distance_line_to_stop : stop_distance_lines) {
		auto [name, distance_line] = detail::SeparateRequests(distance_line_to_stop, ':');
		
		auto distances = detail::SplitIntoWords(distance_line, ',');
		
		for (auto dist_to_stop : distances) {
			auto [dist_s, to_stop] = detail::SeparateRequests(dist_to_stop, ' ');
            
            if (to_stop == "") {
                continue;
            }
			
			auto [to, stop] = detail::SeparateRequests(to_stop, ' ');
			            
			double dist = std::stod(std::string(dist_s));
			
			auto departure_stop = catalog.FindStop(std::string(name));
			auto arrival_stop = catalog.FindStop(std::string(stop));
			
			catalog.SetDistance(departure_stop, arrival_stop, dist);
		}
		
	}
}

void AddBusInCat(catalog::TransportCatalogue& catalog, const std::vector<std::string>& lines_with_buses) {
    for (auto& bus_line : lines_with_buses) {
        bool round = false;
        if (bus_line.find('>') != bus_line.npos) {
            round = true;
        }
        auto [name, stops_line] = detail::SeparateRequests(bus_line, ':');
        name = detail::Retrieve(name);
        
        char by = '-';
        if (round) {
            by = '>';
        }
        auto stops = detail::SplitIntoWords(stops_line, by);
 
        catalog.AddBus(name, stops, round);  
    }
}


void FillingCatalog(catalog::TransportCatalogue& catalog, std::istream& input) {
    int n = 0;
    input >> n;
    std::string s;
    getline(input, s);
    
    std::unordered_map<std::string, std::vector<std::string>> query_for_add;

    for (int i = 0; i < n; ++i) {
    std::string line;
    getline(input, line);
        auto [query_name, query_string] = detail::SeparateRequests(line, ' ');
            query_for_add[std::string(query_name)].push_back(std::string(query_string));        
    }
    
    AddStopInCat(catalog, query_for_add["Stop"]);
	
	AddStopDistance(catalog, query_for_add["Stop"]);
    
    AddBusInCat(catalog, query_for_add["Bus"]);
}
}
