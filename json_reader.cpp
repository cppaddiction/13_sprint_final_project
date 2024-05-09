#include "json_reader.h"

void transport_system::JsonReader::LoadData(std::istream& in)
{
    std::string input_str;
    std::string input_str_total="";

    while(std::getline(in, input_str))
    {
        input_str_total+=input_str;
        if(input_str=="}")
        {
            break;
        }
    }

    auto ans=json::LoadJSON(input_str_total).GetRoot().AsMap();
    LoadBaseRequests(ans["base_requests"].AsArray());
    LoadStatRequests(ans["stat_requests"].AsArray());
    LoadRenderSettings(ans["render_settings"].AsMap());
    LoadRouterSettings(ans["routing_settings"].AsMap());
}

void transport_system::JsonReader::LoadBaseRequests(const json::Array& base_requests)
{
    for(const auto& item: base_requests)
    {
        auto m=item.AsMap();
        if(m["type"].IsString()&&m["type"].AsString()=="Stop")
        {
            detail::StopData result;
            result.coords.lat=m["latitude"].AsDouble();
            result.coords.lng=m["longitude"].AsDouble();
            auto stops=m["road_distances"].AsMap();
            if(stops.size()>0)
            {
                for(auto it=stops.begin(); it!=stops.end(); it++)
                {
                    result.connections[it->first]=stops[it->first].AsDouble();
                }
            }
            if(m["name"].IsString())
            {
                TC_.PushStopData(m["name"].AsString(), result);
                TC_.PushBusThroughStopData(m["name"].AsString());
            }
        }
    }
    for(const auto& item: base_requests)
    {
        auto m=item.AsMap();
        if(m["type"].IsString()&&m["type"].AsString()=="Bus")
        {
            auto stops=m["stops"].AsArray();
            std::vector<std::string> r;
            for(const auto& stop: stops)
            {
                if(stop.IsString())
                {
                    r.push_back(stop.AsString());
                }
            }
            if(m["is_roundtrip"].AsBool())
            {
                r.push_back(">");
            }
            else
            {
                r.push_back("-");
            }
            if(m["name"].IsString())
            {
                TC_.PushPathData("Bus " + m["name"].AsString(), r);
                for(int i=0; i<static_cast<int>(r.size())-1; i++)
                {
                    TC_.PushBusThroughStopData(r[i], "Bus " + m["name"].AsString());
                }
            }
        }
    }
}

void transport_system::JsonReader::LoadStatRequests(const json::Array& stat_requests)
{
    for(const auto& item: stat_requests)
    {
        auto m=item.AsMap();
        int x=m["id"].AsInt();
        std::vector<std::string> y;
        if(m["type"].IsString()&&m["type"].AsString()=="Map")
        {
            TC_.PushRequestData(std::make_pair(x, y));
            continue;
        }
        else if(m["type"].IsString()&&m["type"].AsString()=="Route")
        {
            std::string from=m["from"].AsString();
            std::string to=m["to"].AsString();
            y.push_back(from);
            y.push_back(to);
            TC_.PushRequestData(std::make_pair(x, y));
            continue;
        }
        else //stop or bus; original condition was removed / it can be found in previous version
        {
            std::string s=m["type"].AsString() + ' ' + m["name"].AsString();
            y.push_back(s);
            TC_.PushRequestData(std::make_pair(x, y));
        }
    }
}

void transport_system::JsonReader::LoadRenderSettings(const json::Dict& render_settings)
{
    double width=0;
    double height=0;
    double padding=0;
    double line_width=0;
    double stop_radius=0;
    int bus_label_font_size=0;
    double bus_label_offsetx=0;
    double bus_label_offsety=0;
    int stop_label_font_size=0;
    double stop_label_offsetx=0;
    double stop_label_offsety=0;
    svg::Color underlayer_color=svg::Rgb(0, 0, 0);;
    double underlayer_width=0;
    std::vector<svg::Color> color_palette={};

    for(auto it=render_settings.begin(); it!=render_settings.end(); it++)
    {
        if(it->first=="width")
        {
            width=it->second.AsDouble();
            continue;
        }
        else if(it->first=="height")
        {
            height=it->second.AsDouble();
            continue;
        }
        else if(it->first=="padding")
        {
            padding=it->second.AsDouble();
            continue;
        }
        else if(it->first=="line_width")
        {
            line_width=it->second.AsDouble();
            continue;
        }
        else if(it->first=="stop_radius")
        {
            stop_radius=it->second.AsDouble();
            continue;
        }
        else if(it->first=="bus_label_font_size")
        {
            bus_label_font_size=it->second.AsInt();
            continue;
        }
        else if(it->first=="bus_label_offset")
        {
            auto x=it->second.AsArray();
            bus_label_offsetx=x[0].AsDouble();
            bus_label_offsety=x[1].AsDouble();
            continue;
        }
        else if(it->first=="stop_label_font_size")
        {
            stop_label_font_size=it->second.AsInt();
            continue;
        }
        else if(it->first=="stop_label_offset")
        {
            auto x=it->second.AsArray();
            stop_label_offsetx=x[0].AsDouble();
            stop_label_offsety=x[1].AsDouble();
            continue;
        }
        else if(it->first=="underlayer_color")
        {
            if(it->second.IsString())
            {
                underlayer_color=it->second.AsString();
            }
            else
            {
                auto x=it->second.AsArray();
                if(x.size()==3)
                {
                    underlayer_color=svg::Rgb(x[0].AsInt(), x[1].AsInt(), x[2].AsInt());
                }
                else
                {
                    underlayer_color=svg::Rgba(x[0].AsInt(), x[1].AsInt(), x[2].AsInt(), x[3].AsDouble());
                }
            }
            continue;
        }
        else if(it->first=="underlayer_width")
        {
            underlayer_width=it->second.AsDouble();
            continue;
        }

        else
        {
            auto x=it->second.AsArray();
            for(const auto& item: x)
            {
                if(item.IsString())
                {
                    color_palette.emplace_back(item.AsString());
                }
                else
                {
                    auto y=item.AsArray();
                    if(y.size()==3)
                    {
                        color_palette.emplace_back(svg::Rgb(y[0].AsInt(), y[1].AsInt(), y[2].AsInt()));
                    }
                    else
                    {
                        color_palette.emplace_back(svg::Rgba(y[0].AsInt(), y[1].AsInt(), y[2].AsInt(), y[3].AsDouble()));
                    }
                }
            }
        }
    }

    MR_.SetWidth(width);
    MR_.SetHeight(height);
    MR_.SetPadding(padding);
    MR_.SetLineWidth(line_width);
    MR_.SetStopRadius(stop_radius);
    MR_.SetBusLabelFontSize(bus_label_font_size);
    MR_.SetBusLabelOffsetX(bus_label_offsetx);
    MR_.SetBusLabelOffsetY(bus_label_offsety);
    MR_.SetStopLabelFontSize(stop_label_font_size);
    MR_.SetStopLabelOffsetX(stop_label_offsetx);
    MR_.SetStopLabelOffsetY(stop_label_offsety);
    MR_.SetUnderlayerColor(underlayer_color);
    MR_.SetUnderlayerWidth(underlayer_width);
    MR_.SetColorPalette(color_palette);
}

void transport_system::JsonReader::LoadRouterSettings(const json::Dict& router_settings)
{
    for(auto it=router_settings.begin(); it!=router_settings.end(); it++)
    {
        if(it->first=="bus_velocity")
        {
            TR_.SetBusVelocity(it->second.AsDouble());
        }
        else
        {
            TR_.SetBusWaitTime(it->second.AsInt());
        }
    }
}