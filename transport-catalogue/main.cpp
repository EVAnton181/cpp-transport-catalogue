#include <fstream>
#include <iostream>
#include <chrono>
#include <string>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"

// #include "log_duration.h"

int main() {
// 	для gdb
	std::ifstream in("input12-2.json");
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!
    catalog::TransportCatalogue catalog;
	map_renderer::MapRanderer map;

	LoadJSON(catalog, map);
	
    return 0;
}
