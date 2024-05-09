#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace transport_system {

    class JsonReader {
        public:
        explicit JsonReader(TransportCatalogue& tc, map_entities::MapRenderer& mr, TransportRouter& tr) : TC_(tc), MR_(mr), TR_(tr) {}
        void LoadData(std::istream& in);
        void LoadBaseRequests(const json::Array& base_requests);
        void LoadStatRequests(const json::Array& stat_requests);
        void LoadRenderSettings(const json::Dict& render_settings);
        void LoadRouterSettings(const json::Dict& router_settings);

        private:
        TransportCatalogue& TC_;
        map_entities::MapRenderer& MR_;
        TransportRouter& TR_;
    };

}
