
#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <variant>


namespace svg {

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

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& cap);
std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& join);

struct Rgb {
	Rgb() = default;
	Rgb(uint8_t r, uint8_t g, uint8_t b)
		: red(r)
		, green(g)
		, blue(b) 
	{
	}
	
	uint8_t red = 0u;
	uint8_t green = 0u;
	uint8_t blue = 0u;
};

struct Rgba {
	Rgba() = default;
	Rgba(uint8_t r, uint8_t g, uint8_t b, double o)
		: red(r)
		, green(g)
		, blue(b) 
		, opacity(o) 
	{
	}
	
	uint8_t red = 0u;
	uint8_t green = 0u;
	uint8_t blue = 0u;
	double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>; 
inline const Color NoneColor{"none"};

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

void PrintColor(std::ostream& out, std::monostate);
void PrintColor(std::ostream& out, std::string color);
void PrintColor(std::ostream& out, Rgb);
void PrintColor(std::ostream& out, Rgba);

template<typename T, typename... Ts>
std::ostream& operator<<(std::ostream& out, const std::variant<T, Ts...>& color) {
	std::visit([&out](auto&& value) {
             PrintColor(out, value);
        }, color);

		return out;
}

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
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


/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
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
		stroke_width_ = width;
		return AsOwner();
	}

	Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
		stroke_line_cap_ = line_cap;
		return AsOwner();
	}
	
	Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
		stroke_line_join_ = line_join;
		return AsOwner();
	}
    
protected:
    ~PathProps() = default;
	
    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

		if (fill_color_) {
            out << "fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_ > 0) {
			out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
		}
		if (stroke_line_cap_) {
			out << " stroke-linecap=\""sv;
			out << *stroke_line_cap_;
			out << "\""sv;
		}
		if (stroke_line_join_) {
			out << " stroke-linejoin=\""sv;
			out << *stroke_line_join_;
			out << "\""sv;
		}
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
	std::optional<double> stroke_width_;
	std::optional<StrokeLineCap> stroke_line_cap_;
	std::optional<StrokeLineJoin> stroke_line_join_;	
};

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object , public PathProps<Circle>{
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline>{
public:
    Polyline& AddPoint(Point point);
	
private:
	void RenderObject(const RenderContext& context) const override;
	
	std::string polyline_ = "";
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text>{
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:
	void RenderObject(const RenderContext& context) const override;
	
	Point pos_;
	Point offset_;
	uint32_t size_ = 1;
	std::string font_weight_ = "";
	std::string font_family_ = "";
	std::string data_ = "";
};

class ObjectContainer {
public:
	template <typename Obj>
    void Add(Obj obj) {
		std::unique_ptr<Obj> obj_new = std::make_unique<Obj>(std::move(obj));
		AddPtr(std::move(obj_new));
	}

    // Добавляет в svg-документ объект-наследник svg::Object
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
	
	virtual ~ObjectContainer() = default;
};

class Document : public ObjectContainer {
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
    */
	template <typename Obj>
    void Add(Obj obj) {
		objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
	}

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj);

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
	std::vector<std::unique_ptr<Object>> objects_;
    // Прочие методы и данные, необходимые для реализации класса Document
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& g) const = 0;
	
	virtual ~Drawable() = default; 
}; 

}  // namespace svg
