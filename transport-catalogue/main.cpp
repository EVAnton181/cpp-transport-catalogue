#include <fstream>
#include <iostream>
#include <chrono>
#include <string>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "serialization.h"
#include "request_handler.h"
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
    stream << "Usage: transport_catalogue [make_base|process_requests]\n";
}

int main(int argc, char* argv[]) {
// 	std::ifstream in("./process_requests.json");
//     std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!
    
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);
    
    catalog::TransportCatalogue catalog;
    map_renderer::MapRanderer map;
    serialization::Serialization serialization;
	
    if (mode == "make_base") {

		MakeBaseJSON(catalog, map, serialization);
		
        RequestHandler handler(catalog, map, catalog.GetGraph(), serialization);
        handler.InitSerializationCatalog();
        handler.SaveSerializationCatalog();
		
    } else if (mode == "process_requests") {

        ProcessRequestsJSON(catalog, map, serialization);
        
    } else {
        PrintUsage();
        return 1;
    }
    
    return 0;
}
