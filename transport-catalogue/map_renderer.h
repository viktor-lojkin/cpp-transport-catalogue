#pragma once

#include "svg.h"
#include "geo.h"
#include "json.h"
#include "domain.h"

#include <algorithm>

namespace renderer {

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding)
        : padding_(padding)
    {
        if (points_begin == points_end) {
            return;
        }

        double max_lon = 0;
        {
            auto [min_it, max_it] = std::minmax_element(
                points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lng < rhs.lng;
                }
            );
            min_lon_ = min_it->lng;
            max_lon = max_it->lng;
        }

        double min_lat = 0;
        {
            auto [min_it, max_it] = std::minmax_element(
                points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lat < rhs.lat;
                }
            );
            min_lat = min_it->lat;
            max_lat_ = max_it->lat;
        }

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
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    // ���������� ������ � ������� � ���������� ������ SVG-�����������
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
    double width = 0.0;
    double height = 0.0;
    double padding = 0.0;
    double stop_radius = 0.0;
    double line_width = 0.0;
    int bus_label_font_size = 0;
    svg::Point bus_label_offset = { 0.0, 0.0 };
    int stop_label_font_size = 0;
    svg::Point stop_label_offset = { 0.0, 0.0 };
    svg::Color underlayer_color = { svg::NoneColor };
    double underlayer_width = 0.0;
    std::vector<svg::Color> color_palette {};
};

class MapRenderer {
public:
    MapRenderer(const RenderSettings& render_settings)
        : render_settings_(render_settings)
    {}

    std::vector<svg::Polyline> GetRouteLines(const std::map<std::string_view, const trans_cat::Bus*>& buses, const SphereProjector& sp) const;
    std::vector<svg::Text> GetBusLabel(const std::map<std::string_view, const trans_cat::Bus*>& buses, const SphereProjector& sp) const;
    std::vector<svg::Circle> GetStopsSymbols(const std::map<std::string_view, const trans_cat::Stop*>& stops, const SphereProjector& sp) const;
    std::vector<svg::Text> GetStopsLabels(const std::map<std::string_view, const trans_cat::Stop*>& stops, const SphereProjector& sp) const;

    svg::Document GetSVG(const std::map<std::string_view, const trans_cat::Bus*>& buses) const;

private:
    const RenderSettings render_settings_;
};

} // namespace renderer