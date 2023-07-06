#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <cstdint>
#include <iostream>
#include <memory>

namespace svg {

struct Rgb {
    Rgb()
        : red_(0), green_(0), blue_(0) {}

    Rgb(uint8_t red, uint8_t green, uint8_t blue)
        : red_(red), green_(green), blue_(blue) {}

    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
};

struct Rgba : public Rgb {
    Rgba()
        : Rgb(), opacity_(1.0) {}

    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
        : Rgb(red, green, blue), opacity_(opacity) {}

    double opacity_;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const Color NoneColor{ std::monostate() };

std::ostream& operator<<(std::ostream& out, Color& color);

struct ColorPrinter {
    std::ostream& out_;
    
    void operator()(std::monostate) const;
    void operator()(std::string color) const;
    void operator()(Rgb color) const;
    void operator()(Rgba color) const;
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

std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap);
std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join);

struct Point {
    Point() = default;
    Point(double x, double y) : x_(x), y_(y) {}
    double x_ = 0;
    double y_ = 0;
};


struct RenderContext {
    RenderContext(std::ostream& out)
        : out_(out) {}

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out_(out), indent_step_(indent_step), indent_(indent) {}

    RenderContext Indented() const {
        return { out_, indent_step_, indent_ + indent_step_ };
    }

    void RenderIndent() const {
        for (int i = 0; i < indent_; ++i) {
            out_.put(' ');
        }
    }

    std::ostream& out_;
    int indent_step_ = 0;
    int indent_ = 0;
};

class Object {
private:
    virtual void RenderObject(const RenderContext& context) const = 0;

public:
    virtual ~Object() = default;

    void Render(const RenderContext& context) const;
};

class ObjectContainer {
public:
    template <typename T>
    void Add(T obj) {
        AddPtr(std::make_unique<T>(std::move(obj)));
    }

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

protected:
    ~ObjectContainer() = default;
};

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual void Draw(ObjectContainer& container) const = 0;
};

template <typename Owner>
class PathProps {
private:
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;

    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

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
        width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv;
            std::visit(ColorPrinter{ out }, *fill_color_);
            out << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv;
            std::visit(ColorPrinter{ out }, *stroke_color_);
            out << "\""sv;
        }
        if (width_) {
            out << " stroke-width=\""sv << *width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
    }
};

class Circle final : public Object, public PathProps<Circle> {
private:
    Point center_;
    double radius_ = 1.0;
    
    void RenderObject(const RenderContext& context) const override;

public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

};

class Polyline final : public Object, public PathProps<Polyline> {
private:
    std::vector<Point> points_;

    void RenderObject(const RenderContext& context) const override;

public:
    Polyline& AddPoint(Point point);
};

class Text final : public Object, public PathProps<Text> {
private:
    Point pos_ = { 0.0, 0.0 };
    Point offset_ = { 0.0, 0.0 };
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;

    void RenderObject(const RenderContext& context) const override;

public:
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);
};

class Document : public ObjectContainer {
private:
    std::vector<std::unique_ptr<Object>> objects_;

public:
    void AddPtr(std::unique_ptr<Object>&& obj) override;
    
    void Render(std::ostream& out) const;
};

}  // namespace svg