#pragma once
#include "svg.h"
#include "domain.h"
#include "transport_catalogue.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

inline const double EPSILON = 1e-6;
bool IsZero(double value);
int get_index(const std::vector<std::string>& v, const std::string& item);

namespace map_entities {

    class SphereProjector {
        public:
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding)
        : padding_(padding)
        {
            if (points_begin == points_end) {
                return;
            }

            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            } else if (width_zoom) {
                zoom_coeff_ = *width_zoom;
            } else if (height_zoom) {
                zoom_coeff_ = *height_zoom;
            }
        }

        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
                };
        }

        private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

	struct RenderSettings {
		RenderSettings()=default;
		double width_=0;
		double height_=0;
		double padding_=0;
		double line_width_=0;
		double stop_radius_=0;
		int bus_label_font_size_=0;
		double bus_label_offsetx_=0;
		double bus_label_offsety_=0;
		int stop_label_font_size_=0;
		double stop_label_offsetx_=0;
		double stop_label_offsety_=0;
		svg::Color underlayer_color_=svg::Rgb(0, 0, 0);
		double underlayer_width_=0;
		std::vector<svg::Color> color_palette_={};

	};

    class MapRenderer {
        public:
            MapRenderer(const transport_system::TransportCatalogue& tc): TC_(tc) {}

            void SetWidth(double width) {settings_.width_=width;}
            void SetHeight(double height) {settings_.height_=height;}
            void SetPadding(double padding) {settings_.padding_=padding;}
            void SetLineWidth(double line_width) {settings_.line_width_=line_width;}
            void SetStopRadius(double stop_radius) {settings_.stop_radius_=stop_radius;}
            void SetBusLabelFontSize(int bus_label_font_size) {settings_.bus_label_font_size_=bus_label_font_size;}
            void SetBusLabelOffsetX(double bus_label_offsetx) {settings_.bus_label_offsetx_=bus_label_offsetx;}
            void SetBusLabelOffsetY(double bus_label_offsety) {settings_.bus_label_offsety_=bus_label_offsety;}
            void SetStopLabelFontSize(int stop_label_font_size) {settings_.stop_label_font_size_=stop_label_font_size;}
            void SetStopLabelOffsetX(double stop_label_offsetx) {settings_.stop_label_offsetx_=stop_label_offsetx;}
            void SetStopLabelOffsetY(double stop_label_offsety) {settings_.stop_label_offsety_=stop_label_offsety;}
            void SetUnderlayerColor(svg::Color underlayer_color) {settings_.underlayer_color_=underlayer_color;}
            void SetUnderlayerWidth(double underlayer_width) {settings_.underlayer_width_=underlayer_width;}
            void SetColorPalette(const std::vector<svg::Color>& color_palette) {settings_.color_palette_=color_palette;}

            double GetWidth() const {return settings_.width_;}
            double GetHeight() const {return settings_.height_;}
            double GetPadding() const {return settings_.padding_;}
            double GetLineWidth() const {return settings_.line_width_;}
            svg::Color GetColor() {auto temp=settings_.color_palette_[index_]; IncrementIndex(); return temp;}
            double GetRadius() const {return settings_.stop_radius_;}
            double GetBusLabelOffsetX() const {return settings_.bus_label_offsetx_;}
            double GetBusLabelOffsetY() const {return settings_.bus_label_offsety_;}
            int GetBusLabelFontSize() const {return settings_.bus_label_font_size_;}
            svg::Color GetUnderlayerColor() const {return settings_.underlayer_color_;}
            double GetUnderlayerWidth() const {return settings_.underlayer_width_;}
            double GetStopLabelOffsetX() const {return settings_.stop_label_offsetx_;}
            double GetStopLabelOffsetY() const {return settings_.stop_label_offsety_;}
            int GetStopLabelFontSize() const {return settings_.stop_label_font_size_;}

            void Draw(std::ostream& out);

        private:
            void IncrementIndex();
            void PushLine(svg::Polyline pline);
            void PushText(svg::Text txt);
            void PushCircle(svg::Circle circle);
            std::pair<std::vector<geo::Coordinates>, std::vector<std::string>> GetDrawData(const std::string& request) const;
            int index_=0;
            svg::Document doc_;
            RenderSettings settings_;
            const transport_system::TransportCatalogue& TC_;
            const std::map<std::string, std::vector<std::string>>& paths_=TC_.GivePathDataReadAccess();
            const std::map<std::string, transport_system::detail::StopData>& stops_=TC_.GiveStopDataReadAccess();
    };

}
