#pragma once
#include "domain.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace transport_system {

    class TransportCatalogue
    {
        public:
            const std::map<std::string, std::vector<std::string>>& GivePathDataReadAccess() const {return path_data__;}
            const std::map<std::string, detail::StopData>& GiveStopDataReadAccess() const {return stop_data__;}
            const std::vector<std::pair<int, std::vector<std::string>>>& GiveRequestDataReadAccess() const {return request_data__;}
            const std::map<std::string, std::set<std::string>>& GiveBusThroughStopDataReadAccess() const {return bus_through_stop_data__;}
            void PushPathData(const std::string& key, const std::vector<std::string>& value) {path_data__[key]=value;}
            void PushStopData(const std::string& key, const detail::StopData& value) {stop_data__[key]=value;}
            void PushRequestData(const std::pair<int, std::vector<std::string>>& value) {request_data__.push_back(value);}
            void PushBusThroughStopData(const std::string& key) {bus_through_stop_data__[key];}
            void PushBusThroughStopData(const std::string& key, const std::string& value) {bus_through_stop_data__[key].insert(value);}

        private:
            std::map<std::string, std::vector<std::string>> path_data__;
            std::map<std::string, detail::StopData> stop_data__;
            std::vector<std::pair<int, std::vector<std::string>>> request_data__;
            std::map<std::string, std::set<std::string>> bus_through_stop_data__;
    };

}
