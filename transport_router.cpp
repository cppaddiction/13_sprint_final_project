#include "transport_router.h"

TransportRouter::TransportRouter(const transport_system::TransportCatalogue& tc) : TC_(tc) {}

void TransportRouter::SetBusWaitTime(int time) { bus_wait_time_ = time; }

void TransportRouter::SetBusVelocity(double bus_velocity) { bus_velocity_ = bus_velocity; }

const graph::DirectedWeightedGraph<EdgeWeight>& TransportRouter::MakeGraph()
{
    for (auto it = stops_.begin(); it != stops_.end(); it++)
    {
        stops_by_index_.insert(it->first);
    }

    graph.init(stops_by_index_.size());

    for (auto it = paths_.begin(); it != paths_.end(); it++)
    {
        if (it->second.size() > 0)
        {
            if (it->second[it->second.size() - 1] == ">")
            {
                int offset = 1;
                for (auto itt = it->second.begin(); itt != it->second.end() - 2; itt++)
                {
                    double delta = 0;
                    auto stop1 = stops_.at(*itt);
                    for (auto ittt = next(it->second.begin(), offset); ittt != it->second.end() - 1; ittt++)
                    {
                        auto stop2 = stops_.at(*ittt);
                        try
                        {
                            delta += stop1.connections.at(*ittt);
                        }
                        catch (...)
                        {
                            delta += stop2.connections.at(*next(ittt, -1));
                        }
                        stop1 = stop2;
                        double time_required = delta / 1000 / bus_velocity_ * 60 + bus_wait_time_;

                        graph::Edge<EdgeWeight> bar;
                        bar.from = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((*itt))));
                        bar.to = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((*ittt))));
                        EdgeWeight ew{};
                        ew.time_ = time_required;
                        bar.weight = ew;

                        graph.AddEdge(bar);
                        vec_.push_back(EdgeInfo{ it->first.substr(4, it->first.size() - 4), static_cast<int>(std::distance(itt, ittt)), delta / 1000 / bus_velocity_ * 60, *itt , *ittt });
                    }
                    offset += 1;
                }
            }
            else
            {
                int offset = -3;
                for (int k = it->second.size() - 2; k > 0; k--)
                {
                    double delta_forw = 0;
                    double delta_backw = 0;
                    auto stop1 = stops_.at(it->second[k]);
                    for (int i = it->second.size() + offset; i >= 0; i--)
                    {
                        auto stop2 = stops_.at(it->second[i]);
                        bool forward_path_avail = false;
                        bool backward_path_avail = false;
                        double dist_forw = 0;
                        double dist_backw = 0;
                        try
                        {
                            dist_forw = stop1.connections.at(it->second[i]);
                            forward_path_avail = true;
                        }
                        catch (...)
                        {

                        }
                        try
                        {
                            dist_backw = stop2.connections.at(it->second[i + 1]);
                            backward_path_avail = true;
                        }
                        catch (...)
                        {

                        }
                        if (forward_path_avail && backward_path_avail)
                        {
                            delta_forw += dist_forw;
                            delta_backw += dist_backw;
                            double time_required_forw = delta_forw / 1000 / bus_velocity_ * 60 + bus_wait_time_;
                            double time_required_backw = delta_backw / 1000 / bus_velocity_ * 60 + bus_wait_time_;

                            graph::Edge<EdgeWeight> bar_forw;
                            bar_forw.from = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((it->second[k]))));
                            bar_forw.to = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((it->second[i]))));
                            EdgeWeight ew_forw{};
                            ew_forw.time_ = time_required_forw;
                            bar_forw.weight = ew_forw;

                            graph::Edge<EdgeWeight> bar_backw;
                            bar_backw.from = bar_forw.to;
                            bar_backw.to = bar_forw.from;
                            EdgeWeight ew_backw{};
                            ew_backw.time_ = time_required_backw;
                            bar_backw.weight = ew_backw;

                            graph.AddEdge(bar_forw);
                            graph.AddEdge(bar_backw);
                            vec_.push_back(EdgeInfo{ it->first.substr(4, it->first.size() - 4), std::abs(i - k), dist_forw / 1000 / bus_velocity_ * 60, it->second[k] , it->second[i] });
                            vec_.push_back(EdgeInfo{ it->first.substr(4, it->first.size() - 4), std::abs(i - k), dist_backw / 1000 / bus_velocity_ * 60, it->second[i] , it->second[k] });
                        }
                        else
                        {
                            double dist;
                            if (forward_path_avail)
                            {
                                dist = dist_forw;
                            }
                            else
                            {
                                dist = dist_backw;
                            }

                            delta_forw += dist;
                            delta_backw += dist;
                            double time_required_forw = delta_forw / 1000 / bus_velocity_ * 60 + bus_wait_time_;
                            double time_required_backw = delta_backw / 1000 / bus_velocity_ * 60 + bus_wait_time_;

                            graph::Edge<EdgeWeight> bar_forw;
                            bar_forw.from = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((it->second[k]))));
                            bar_forw.to = static_cast<size_t>(std::distance(stops_by_index_.begin(), stops_by_index_.find((it->second[i]))));
                            EdgeWeight ew_forw{};
                            ew_forw.time_ = time_required_forw;
                            bar_forw.weight = ew_forw;

                            graph::Edge<EdgeWeight> bar_backw;
                            bar_backw.from = bar_forw.to;
                            bar_backw.to = bar_forw.from;
                            EdgeWeight ew_backw{};
                            ew_backw.time_ = time_required_backw;
                            bar_backw.weight = ew_backw;

                            graph.AddEdge(bar_forw);
                            graph.AddEdge(bar_backw);
                            vec_.push_back(EdgeInfo{ it->first.substr(4, it->first.size() - 4), std::abs(i - k), dist_forw / 1000 / bus_velocity_ * 60, it->second[k] , it->second[i] });
                            vec_.push_back(EdgeInfo{ it->first.substr(4, it->first.size() - 4), std::abs(i - k), dist_backw / 1000 / bus_velocity_ * 60, it->second[i] , it->second[k] });

                        }
                        stop1 = stop2;
                    }
                    offset -= 1;
                }
            }
        }
    }
    return graph;
}

json::Dict TransportRouter::Build(const graph::Router<EdgeWeight>& rtr, const std::string& from, const std::string& to, int id) const
{
    json::Builder result;
    result.StartDict().Key("request_id").Value(id);

    auto from_ = static_cast<size_t>(distance(stops_by_index_.begin(), stops_by_index_.find(from)));
    auto to_ = static_cast<size_t>(distance(stops_by_index_.begin(), stops_by_index_.find(to)));

    auto response = rtr.BuildRoute(from_, to_);
    if (response)
    {
        result.Key("total_time").Value((*response).weight.time_);
        result.Key("items").StartArray();
        auto edges = (*response).edges;
        for (int i = 0; i < edges.size(); i++)
        {
            auto info = vec_[edges[i]];
            result.StartDict().Key("type").Value("Wait").Key("stop_name").Value(info.from).Key("time").Value(bus_wait_time_).EndDict();
            result.StartDict().Key("type").Value("Bus").Key("bus").Value(info.bus_name).Key("span_count").Value(info.span_count).Key("time").Value(info.time).EndDict();
        }
        result.EndArray();
    }
    else
    {
        result.Key("error_message").Value("not found");
    }
    return result.EndDict().Build().AsMap();
}