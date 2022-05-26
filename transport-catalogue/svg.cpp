#include "svg.h"

namespace svg {

using namespace std::literals;

/// Переопределение оператора вывода для StrokeLineCap
std::ostream& operator<<(std::ostream& out, const StrokeLineCap& cap) {
	switch (cap) {
		case StrokeLineCap::BUTT:
			out << "butt";
			break;
		case StrokeLineCap::ROUND:
			out << "round";
			break;
		case StrokeLineCap::SQUARE:
			out << "square";
			break;
	}
	
	return out;
}

/// Переопределение оператора вывода для StrokeLineJoin
std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& join) {
	switch (join) {
		case StrokeLineJoin::ARCS:
			out << "arcs";
			break;
		case StrokeLineJoin::BEVEL:
			out << "bevel";
			break;
		case StrokeLineJoin::MITER:
			out << "miter";
			break;
		case StrokeLineJoin::MITER_CLIP:
			out << "miter-clip";
			break;
		case StrokeLineJoin::ROUND:
			out << "round";
			break;
	}
	
	return out;
}

/// Функции вывода цвета для разных вариантов хранения
void PrintColor(std::ostream& out, std::monostate) {
    out << "none"sv;
}
void PrintColor(std::ostream& out, std::string color) {
    out << color;
}
// rgb(155,155,255)
void PrintColor(std::ostream& out, Rgb rgb) {
    out << "rgb("sv << unsigned(rgb.red) << ","sv << unsigned(rgb.green) << "," << unsigned(rgb.blue) << ")";
}
// rgba(15,15,25,0.7)
void PrintColor(std::ostream& out, Rgba rgba) {
    out << "rgba("sv << unsigned(rgba.red) << ","sv << unsigned(rgba.green) << "," << unsigned(rgba.blue) << "," << rgba.opacity << ")";
}

///Вывод содержимого
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

/// ---------- Circle ------------------
/// Установка центра окружности
Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}
/// Установка радиуса окружности
Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}
/// Вывод тега окружности
// <circle cx="20" cy="20" r="10" />
void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
	PathProps::RenderAttrs(context.out);
    out << "/>"sv;
}

// ------------ Polyline ----------------
/// Добавление вершины полилинии
Polyline& Polyline::AddPoint(Point point) {
	std::stringstream output;
	if (!polyline_.empty()) {
		output << " ";
	}
	output << point.x << "," << point.y;
	polyline_ += output.str();
	return *this;
}

/// Вывод тега полилинии
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv << polyline_ << "\" "sv;
	PathProps::RenderAttrs(context.out);
	out << "/>"sv;
}

// ------------ Text ---------------
/// Установка опорной точни для текста
Text& Text::SetPosition(Point pos) {
	pos_ = pos;
	return *this;
}

/// Установка смещения относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
	offset_ = offset;
	return *this;	
}

/// Установка размера шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
	size_ = size;
	return *this;
}

/// Установка названия шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
	font_family_ = std::move(font_family);
	return *this;
}

/// Установка толщины шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
	font_weight_ = std::move(font_weight);
	return *this;
}

/// Установка текстовго содержимого объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
	data_ = std::move(data);
	return *this;
}

/// Вывод тега текста
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text ";
	PathProps::RenderAttrs(context.out);
	out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv;
	out << offset_.x << "\" dy=\""sv << offset_.y << "\" font-size=\""sv << size_;
	out << "\"";
	if (!font_family_.empty()) {
		out << " font-family=\""sv << font_family_ << "\""sv;
	}
	if (!font_weight_.empty()) {
		out << " font-weight=\""sv << font_weight_ << "\""sv;
	}
    out << ">"sv;

	for (auto ch : data_) {
		switch (ch) {
			case '"':
				out << "&qout;";
				break;
			case '\'':
				out << "&apos;";
				break;
			case '<':
				out << "&lt;";
				break;
			case '>':
				out << "&gl;";
				break;
			case '&':
				out << "&amp;";
				break;
			default:
				out << ch;				
		}
	}
	out << "</text>"sv;
}

/// Добавление объекта в документ
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
	objects_.emplace_back(std::move(obj));
}

/// Вывод документа
void Document::Render(std::ostream& out) const {
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
	
	RenderContext ctx(out, 2, 2);

	for (auto& a : objects_) {
		a->Render(ctx);
	}
	out << "</svg>"sv;
}


}  // namespace svg
