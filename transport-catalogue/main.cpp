#include <fstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
// #include "test_functions.h"

int main() {
//     DistanceTest();
    
    catalog::TransportCatalogue catalog;

    filling::FillingCatalog(catalog);
    statquery::Query(catalog);
  
    return 0;
}
