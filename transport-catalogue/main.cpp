#include <fstream>
#include <iostream>
#include <chrono>
#include <string>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "serialization.h"
// #include "request_handler.h"

// #include "log_duration.h"

// int main() {
// 	для gdb
// LOG_DURATION("total");
//     catalog::TransportCatalogue catalog;
// 	map_renderer::MapRanderer map;
// 
// 	LoadJSON(catalog, map);
	
using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
	std::ifstream in("./make_base.json");
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!
    
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);
	
    if (mode == "make_base"sv) {
		catalog::TransportCatalogue catalog;
		map_renderer::MapRanderer map;
		serialization::Serialization serialization;
		
		InitBaseJSON(catalog, map, serialization);
		
		
    } else if (mode == "process_requests"sv) {
// 		catalog::TransportCatalogue catalog;
// 		map_renderer::MapRanderer map;
// 		serialization::Serialization serialization;
		
        RequestJSON();
		
    } else {
        PrintUsage();
        return 1;
    }
    
    return 0;
}
