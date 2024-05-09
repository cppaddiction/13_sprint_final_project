#include "request_handler.h"

using namespace std;
using namespace map_entities;
using namespace transport_system;

int main()
{
    TransportCatalogue catalogue;
    MapRenderer renderer(catalogue);
    TransportRouter router(catalogue);
    JsonReader j_reader(catalogue, renderer, router);
    RequestHandler r_handler(catalogue, renderer, router);
    j_reader.LoadData(cin);

    graph::Router<EdgeWeight> router_(router.MakeGraph());

    r_handler.FormAnswer(router_, cout);
    return 0;
}
