#pragma once
#include "json_reader.h"
#include "json_builder.h"

namespace transport_system {

    class RequestHandler {
        public:
            explicit RequestHandler(const TransportCatalogue& tc, map_entities::MapRenderer& mr, const TransportRouter& tr) : TC_(tc), MR_(mr), TR_(tr) {}
            void FormAnswer(const graph::Router<EdgeWeight>& rtr, std::ostream& out) const;
            json::Dict ProcessBusRequest(const std::string& request, int id) const;
            json::Dict ProcessStopRequest(const std::string& request, int id) const;
            json::Dict ProcessMapRequest(int id) const;
            json::Dict ProcessRouteRequest(const graph::Router<EdgeWeight>& rtr, const std::string& from, const std::string& to, int id) const;

        private:
            const TransportCatalogue& TC_;
            map_entities::MapRenderer& MR_;
            const TransportRouter& TR_;
            const std::map<std::string, std::vector<std::string>>& paths_=TC_.GivePathDataReadAccess();
            const std::map<std::string, detail::StopData>& stops_=TC_.GiveStopDataReadAccess();
            const std::vector<std::pair<int, std::vector<std::string>>>& requests_=TC_.GiveRequestDataReadAccess();
            const std::map<std::string, std::set<std::string>>& BTS_=TC_.GiveBusThroughStopDataReadAccess();
    };

}
