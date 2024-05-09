#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <variant>

namespace svg {

struct Rgb {
    Rgb()=default;
    Rgb(uint8_t r, uint8_t g, uint8_t b):red(r), green(g), blue(b) {}
    uint8_t red=0;
    uint8_t green=0;
    uint8_t blue=0;
};

struct Rgba {
    Rgba()=default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o):red(r), green(g), blue(b), opacity(o) {}
    uint8_t red=0;
    uint8_t green=0;
    uint8_t blue=0;
    double opacity=1.0;
};

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
inline const Color NoneColor{"none"};

struct OstreamColorPrinter {
    std::ostream& out;

    void operator()(std::monostate) const {
        out << "none";
    }

    void operator()(std::string color) const {
        out << color;
    }

    void operator()(svg::Rgb rgb) const {
        out<<"rgb("<<std::to_string(rgb.red)<<','<<std::to_string(rgb.green)<<','<<std::to_string(rgb.blue)<<')';
    }

    void operator()(svg::Rgba rgba) const {
        out << "rgba("<<std::to_string(rgba.red)+','<<std::to_string(rgba.green)<<','<<std::to_string(rgba.blue)<<',';
        out<<rgba.opacity<<')';
    }
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        width_=width;
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_=line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_=line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        if (fill_color_) {
            out << " fill=\""sv;
            visit(OstreamColorPrinter{out}, *fill_color_);
            out<< "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv;
            visit(OstreamColorPrinter{out}, *stroke_color_);
            out<< "\""sv;
        }
        if (width_) {
            out<<" stroke-width=\""sv << *width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv;
            if(*line_cap_==StrokeLineCap::BUTT)
                out<<"butt";
            if(*line_cap_==StrokeLineCap::ROUND)
                out<<"round";
            if(*line_cap_==StrokeLineCap::SQUARE)
                out<<"square";
            out<< "\""sv;
        }
        if (line_join_) {
            out<<" stroke-linejoin=\""sv;
            if(*line_join_==StrokeLineJoin::ARCS)
                out<<"arcs";
            if(*line_join_==StrokeLineJoin::BEVEL)
                out<<"bevel";
            if(*line_join_==StrokeLineJoin::MITER)
                out<<"miter";
            if(*line_join_==StrokeLineJoin::MITER_CLIP)
                out<<"miter-clip";
            if(*line_join_==StrokeLineJoin::ROUND)
                out<<"round";
            out<< "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};

class Object {
public:
    void Render(const RenderContext& context) const;
    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    Point center_;
    double radius_ = 1.0;
    void RenderObject(const RenderContext& context) const override;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);
    int get_lenth() {return points_.size();}

private:
    std::vector<Point> points_;
    void RenderObject(const RenderContext& context) const override;
};

class Text final : public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);

private:
    Point pos_;
    Point offset_;
    uint32_t size_=1;
    std::string font_family_="";
    std::string font_weight_="";
    std::string data_="";
    void RenderObject(const RenderContext& context) const override;
};

class ObjectContainer {
    public:
        template <typename Obj>
        void Add(Obj obj)
        {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }
        virtual void AddPtr(std::unique_ptr<Object>&& obj)=0;
        ~ObjectContainer()=default;

    protected:
        std::vector<std::unique_ptr<Object>> objects_;
};

class Document final : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override
    {
        objects_.push_back(std::move(obj));
    }

    void Render(std::ostream& out) const
    {
        RenderContext RC(out);
        out<< "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"<<std::endl;
        out<<"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"<<std::endl;
        for(const auto& obj: objects_)
        {
            out<<"  ";
            obj->Render(RC);
        }
        out<<"</svg>";
    }
};

class Drawable {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
};

}
