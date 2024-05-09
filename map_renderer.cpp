#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

int get_index(const std::vector<std::string>& v, const std::string& item)
{
    for(int i=0; i<static_cast<int>(v.size()); i++)
    {
        if(v[i]==item)
        {
            return i;
        }
    }
    return -1;
}

void map_entities::MapRenderer::IncrementIndex()
{
    if(index_!=static_cast<int>(settings_.color_palette_.size())-1)
    {
        index_++;
    }
    else
    {
        index_=0;
    }
}

void map_entities::MapRenderer::PushLine(svg::Polyline pline)
{
    doc_.Add(pline);
}

void map_entities::MapRenderer::PushText(svg::Text txt)
{
    doc_.Add(txt);
}

void map_entities::MapRenderer::PushCircle(svg::Circle circle)
{
    doc_.Add(circle);
}

void map_entities::MapRenderer::Draw(std::ostream& out)
{
    using namespace std;
    vector<string> bus_names;
    for(auto it=paths_.begin(); it!=paths_.end(); it++)
    {
        bus_names.push_back(it->first);
    }
    sort(bus_names.begin(), bus_names.end());
    vector<int> sizes;
    vector<geo::Coordinates> v;
    vector<vector<string>> routes;
    vector<string> all_stops;
    for(const auto& bus: bus_names)
    {
        auto response=GetDrawData(bus);
        sizes.push_back(response.first.size());
        routes.push_back(response.second);
        all_stops.insert(all_stops.end(), response.second.begin(), response.second.end()-2);
        v.insert(v.end(), response.first.begin(), response.first.end());
    }
    set<string> sss(all_stops.begin(), all_stops.end());
    vector<string> all_stops_unique(sss.begin(), sss.end());
    sort(all_stops_unique.begin(), all_stops_unique.end());
    int index=0;
    int current=0;
    map_entities::SphereProjector sp{v.begin(), v.end(), GetWidth(), GetHeight(), GetPadding()};
    vector<pair<svg::Text, svg::Text>> texts1;
    vector<svg::Circle> circles(all_stops_unique.size());
    vector<pair<svg::Text, svg::Text>> texts2(all_stops_unique.size());
    for(const auto size: sizes)
    {
        if(size>0)
        {
            svg::Polyline line;
            bool is_round_path=">"==routes[index][routes[index].size()-2]?true:false;
            auto color=GetColor();
            for(int i=current; i<current+size; i++)
            {
                if(i==current)
                {
                    auto t1=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetBusLabelOffsetX(), GetBusLabelOffsetY())).SetFontSize(GetBusLabelFontSize()).SetFontFamily("Verdana").SetFontWeight("bold").SetData(routes[index][routes[index].size()-1]).SetFillColor(GetUnderlayerColor()).SetStrokeColor(GetUnderlayerColor()).SetStrokeWidth(GetUnderlayerWidth()).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    auto t2=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetBusLabelOffsetX(), GetBusLabelOffsetY())).SetFontSize(GetBusLabelFontSize()).SetFontFamily("Verdana").SetFontWeight("bold").SetData(routes[index][routes[index].size()-1]).SetFillColor(color);
                    texts1.push_back(make_pair(t1, t2));
                }
                if(!is_round_path)
                {
                    if(i==current+(int((size)/2))&&routes[index][routes[index].size()-3]!=routes[index][0])
                    {
                        auto t1=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetBusLabelOffsetX(), GetBusLabelOffsetY())).SetFontSize(GetBusLabelFontSize()).SetFontFamily("Verdana").SetFontWeight("bold").SetData(routes[index][routes[index].size()-1]).SetFillColor(GetUnderlayerColor()).SetStrokeColor(GetUnderlayerColor()).SetStrokeWidth(GetUnderlayerWidth()).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                        auto t2=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetBusLabelOffsetX(), GetBusLabelOffsetY())).SetFontSize(GetBusLabelFontSize()).SetFontFamily("Verdana").SetFontWeight("bold").SetData(routes[index][routes[index].size()-1]).SetFillColor(color);
                        texts1.push_back(make_pair(t1, t2));
                    }
                }
                line.AddPoint(sp(v[i]));
                if(!(sp(v[i]).x==sp(v[current]).x&&sp(v[i]).y==sp(v[current]).y)||i==current)
                {
                    if(i-current>=0&&i-current<static_cast<int>(routes[index].size())&&routes[index][i-current]!="-"&&routes[index][i-current]!="<"&&routes[index][i-current]!=routes[index][routes[index].size()-1])
                    {
                        auto ind=get_index(all_stops_unique, routes[index][i-current]);
                        circles[ind]=svg::Circle().SetCenter(sp(v[i])).SetRadius(GetRadius()).SetFillColor("white");
                        auto t1=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetStopLabelOffsetX(), GetStopLabelOffsetY())).SetFontSize(GetStopLabelFontSize()).SetFontFamily("Verdana").SetData(routes[index][i-current]).SetFillColor(GetUnderlayerColor()).SetStrokeColor(GetUnderlayerColor()).SetStrokeWidth(GetUnderlayerWidth()).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                        auto t2=svg::Text().SetPosition(sp(v[i])).SetOffset(svg::Point(GetStopLabelOffsetX(), GetStopLabelOffsetY())).SetFontSize(GetStopLabelFontSize()).SetFontFamily("Verdana").SetData(routes[index][i-current]).SetFillColor("black");
                        texts2[ind]=make_pair(t1, t2);
                    }
                }
            }
            current+=size;
            line.SetStrokeColor(color);
            line.SetFillColor(svg::NoneColor);
            line.SetStrokeWidth(GetLineWidth());
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            PushLine(line);
            index++;
        }
    }
    for(const auto& text: texts1)
    {
        PushText(text.first);
        PushText(text.second);
    }
    for(const auto circle: circles)
    {
        PushCircle(circle);
    }
    for(const auto& text: texts2)
    {
        PushText(text.first);
        PushText(text.second);
    }
    doc_.Render(out);
}

std::pair<std::vector<geo::Coordinates>, std::vector<std::string>> map_entities::MapRenderer::GetDrawData(const std::string& request) const
{
    std::vector<geo::Coordinates> result;
    std::vector<std::string> v;
    if(paths_.find(request)!=paths_.end())
    {
        v=paths_.at(request);
        bool is_round_path=">"==v[v.size()-1]?true:false;
        for(int i=0; i<static_cast<int>(v.size())-1; i++)
        {
            const auto& stop=stops_.at(v[i]);
            result.push_back(stop.coords);
        }
        if(!is_round_path)
        {
            for(int i=static_cast<int>(v.size())-3; i>=0; i--)
            {
                const auto& stop=stops_.at(v[i]);
                result.push_back(stop.coords);
            }
        }
        v.push_back(request.substr(4, request.size()-4));
    }
    return std::make_pair(result, v);
}
