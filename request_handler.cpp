#include "request_handler.h"

void transport_system::RequestHandler::FormAnswer(const graph::Router<EdgeWeight>& rtr, std::ostream& out) const
{
    bool is_first=true;
    out<<'[';
    for(const auto& request: requests_)
    {
        if(!is_first)
        {
            out<<',';
        }
        if(request.second.size()==2) //route
        {
            out<<Print(ProcessRouteRequest(rtr, request.second[0], request.second[1], request.first));
        }
        else if(request.second.size()==0) //map
        {
            out<<Print(ProcessMapRequest(request.first));
        }
        else if(request.second[0][0]=='B') //bus
        {
            out<<Print(ProcessBusRequest(request.second[0], request.first));
        }
        else //if(request.second[0][0]=='S') - stop
        {
            out<<Print(ProcessStopRequest(request.second[0].substr(5, request.second.size()-5), request.first));
        }
        is_first=false;
    }
    out<<']';
}

json::Dict transport_system::RequestHandler::ProcessBusRequest(const std::string& request, int id) const
{
    json::Builder answer;
    if(paths_.find(request)!=paths_.end())
    {
        const auto& v=paths_.at(request);
        std::set<std::string> s(v.begin(), v.end());

        auto stops_on_route=v.size()-1;
        auto unique_stops=s.size()-1;

        double distance=0;
        double real_distance=0;
        bool is_round_path=">"==v[v.size()-1]?true:false;

        for(int i=1; i<static_cast<int>(v.size())-1; i++)
        {
            const auto& stop1=stops_.at(v[i]);
            const auto& stop2=stops_.at(v[i-1]);
            distance+=geo::ComputeDistance(geo::Coordinates{stop1.coords.lat, stop1.coords.lng}, geo::Coordinates{stop2.coords.lat, stop2.coords.lng});
            if(is_round_path)
            {
                try
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
            }
            else
            {
                try
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
                try
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
            }
        }

        if(!is_round_path)
        {
            distance*=2;
            stops_on_route=stops_on_route*2-1;
        }

        double curve=real_distance/distance;

        return answer.StartDict().Key("curvature").Value(curve).Key("request_id").Value(id).Key("route_length").Value(real_distance).Key("stop_count").Value(int(stops_on_route)).Key("unique_stop_count").Value(int(unique_stops)).EndDict().Build().AsMap();

    }
    else
    {
        return answer.StartDict().Key("request_id").Value(id).Key("error_message").Value("not found").EndDict().Build().AsMap();
    }
}

json::Dict transport_system::RequestHandler::ProcessStopRequest(const std::string& request, int id) const
{
    json::Builder answer;
    if(BTS_.find(request)!=BTS_.end())
    {
        const auto& item=BTS_.at(request);
        json::Array tempv;
        if(item.size()!=0)
        {
            std::vector<std::string> temp(item.begin(), item.end());
            std::sort(temp.begin(), temp.end());
            for(const auto& t: temp)
            {
                tempv.push_back(json::Node(t.substr(4, t.size()-4)));
            }
        }
        return answer.StartDict().Key("buses").Value(tempv).Key("request_id").Value(id).EndDict().Build().AsMap();
    }
    else
    {
        return answer.StartDict().Key("request_id").Value(id).Key("error_message").Value("not found").EndDict().Build().AsMap();
    }
}

json::Dict transport_system::RequestHandler::ProcessMapRequest(int id) const
{
    std::ostringstream answer;
    MR_.Draw(answer);
    json::Builder one;
    return one.StartDict().Key("map").Value(answer.str()).Key("request_id").Value(id).EndDict().Build().AsMap();
}

json::Dict transport_system::RequestHandler::ProcessRouteRequest(const graph::Router<EdgeWeight>& rtr, const std::string& from, const std::string& to, int id) const
{   
    return TR_.Build(rtr, from, to, id);
}