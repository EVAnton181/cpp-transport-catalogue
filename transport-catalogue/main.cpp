#include <fstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
// #include "test_functions.h"

int main() {
//     DistanceTest();
    
    std::ifstream in("/home/anton/yandex_praktikum/cpp-transport-catalogue/transport-catalogue/pt3/tsC_case1_input.txt");
    std::cin.rdbuf(in.rdbuf());

    catalog::TransportCatalogue cat;

    filling::FillingCatalog(cat);
    statquery::Query(cat);
  
    return 0;
}
