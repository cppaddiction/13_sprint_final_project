#pragma once
#include "geo.h"
#include <map>
#include <string>

namespace transport_system {

    namespace detail {

        struct StopData
        {
            std::string name;
            geo::Coordinates coords;
            std::map<std::string, double> connections;
        };

    }

}
