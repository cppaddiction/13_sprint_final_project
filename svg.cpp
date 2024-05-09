#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos) {
        pos_=pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_=offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        size_=size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_=font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_=font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_=data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out<<"<text";
        RenderAttrs(out);
        out<<" x=\""<<pos_.x<<"\" y=\""<<pos_.y<<"\" dx=\""<<offset_.x<<"\" dy=\""<<offset_.y<<"\" font-size=\""<<size_<<"\"";
        if(font_family_.size()>0)
            out<<" font-family=\""<<font_family_<<"\"";
        if(font_weight_.size()>0)
            out<<" font-weight=\""<<font_weight_<<"\"";
        out<<">";
        std::string help="";
        for(const auto& h: data_)
        {
            if(h=='"')
                help += "&quot;";
            else if(h=='\'')
                help += "&apos;";
            else if(h=='<')
                help += "&lt;";
            else if(h=='>')
                help += "&gt;";
            else if(h=='&')
                help += "&amp;";
            else
                help += h;
        }
        out<<help;
        out<<"</text>";
    }

    Polyline& Polyline::AddPoint(Point point)
    {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out<<"<polyline points=\"";
        if(points_.size()>0)
        {
            bool first=true;
            for(const auto& p: points_)
            {
                if(first)
                {
                    out <<  p.x << ',' << p.y;
                    first=false;
                }
                else
                {
                    out << ' ' << p.x << ',' << p.y;
                }
            }
        }
        out<<"\"";
        RenderAttrs(out);
        out<<"/>";
    }

}
