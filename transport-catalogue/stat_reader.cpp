#include "stat_reader.h"


void statquery::BusOut(catalog::TransportCatalogue& catalog, std::string& name, std::ostream& out) {
	auto bus_info = catalog.GetBusInfo(name);
	
	if (bus_info.stops_on_route == 0) {
		out << "Bus " << name << ": not found" << std::endl;
	} 
	else {
		out << "Bus " << name << ": "<< bus_info.stops_on_route << " stops on route, " << bus_info.unique_stops << " unique stops, " << bus_info.route_length << " route length, " << bus_info.curvature << " curvature" << std::endl;
	}   
}

void statquery::StopOut(catalog::TransportCatalogue& catalog, std::string& name, std::ostream& out) {
	out << "Stop " << name << ": ";
            
	if (!catalog.FindStop(name)) {
		out << "not found" << std::endl;
		return;
	}
	
	auto buses = catalog.GetBusList(name);

	if (buses.empty()) {
		out << "no buses" << std::endl;
		return;
	}
	
	out << "buses";
	
	for (auto bus : buses) {
		out << " " << bus;
	}
	out << std::endl;
		
}

void statquery::Query(catalog::TransportCatalogue& catalog, std::istream& input, std::ostream& out) {
    int n = 0;
    input >> n;
    std::string line_break;
    getline(input, line_break);
    
    for (int i = 0; i < n; ++i) {
    std::string line;
    getline(input, line);
    
    auto [query_name, query_string] = detail::SeparateRequests(line, ' ');
    auto name = std::string(detail::Retrieve(query_string));
        if (query_name == "Bus") {
			statquery::BusOut(catalog, name);
        }
        else {
            statquery::StopOut(catalog, name);
        }
    }
}
    

